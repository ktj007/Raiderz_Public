#include "StdAfx.h"
#include "GBuff.h"
#include "GEntityPlayer.h"
#include "GBuffInfo.h"
#include "CCommandTable.h"
#include "GPartySystem.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GBuffReleaser.h"
#include "GBuffInstantApplier.h"
#include "GBuffModifierApplier.h"
#include "GBuffTriggerApplier.h"
#include "GBuffRouter.h"
#include "GEFfectTimingChecker.h"
#include "GUseCostMgr.h"
#include "GBuffStack.h"
#include "GBuffScript.h"
#include "GBuffPaletteChanger.h"
#include "GBuffEquipmentChanger.h"

GBuff::GBuff( GEntitySync* pOwner, GBuffInfo* pBuffInfo, float fDurationTime, float fPeriodTime, GTalentInfo* pTalentInfo/*=NULL*/, MUID uidUser/*=MUID::Invalid()*/ )
: m_pOwner(pOwner)
, m_pInfo(pBuffInfo)
, m_pTalentInfo(pTalentInfo)
, m_uidUser(uidUser)
, m_bInifinity(false)
, m_nDestroyType(DESTROY_NONE)
, m_pInstantApplyer(NULL)
, m_pModifierApplyer(NULL)
, m_pTriggerApplyer(NULL)
, m_pRouter(NULL)
, m_pStack(NULL)
, m_bGainEffectOccured(false)
, m_pPaletteChanger(NULL)
, m_pEquipmentChanger(NULL)
{
	DCHECK(pOwner);
	DCHECK(pBuffInfo);
	m_pReleaser = new GBuffReleaser(this);
	m_pEffectTimingChecker = new GEFfectTimingChecker();
	m_pBuffScript = new GBuffScript();
	
	if (m_pOwner->IsActor())
	{
		GEntityActor* pOwnerActor = ToEntityActor(m_pOwner);
		pOwnerActor->AttachObserver(m_pReleaser);
		pOwnerActor->AttachObserver(m_pEffectTimingChecker);
	}
	
	UpdateBuffTime(fDurationTime, fPeriodTime);

	m_pInstantApplyer = new GBuffInstantApplier(this);
	m_pModifierApplyer = new GBuffModifierApplier(this);
	m_pTriggerApplyer = new GBuffTriggerApplier(this);
	m_pEffectTimingChecker->AddListener(m_pInfo->m_Condition.nEffectConditionType, m_pInstantApplyer);
	m_pEffectTimingChecker->AddListener(m_pInfo->m_Condition.nEffectConditionType, m_pModifierApplyer);
	m_pEffectTimingChecker->AddListener(m_pInfo->m_TriggeredBuff.nTiming, m_pTriggerApplyer);
	m_pEffectTimingChecker->AddListener(TC_BUFF_DUPLICATED, m_pModifierApplyer); // 중복 버프 처리를 위한 구독

		
	AttachObserver(m_pEffectTimingChecker);
	AttachObserver(m_pBuffScript);

	if (pOwner->IsPlayer())
	{
		if (pBuffInfo->m_bUseChangePalette)
		{
			m_pPaletteChanger = new GBuffPaletteChanger();
			AttachObserver(m_pPaletteChanger);
		}
		
		if (pBuffInfo->m_bUseChangeEquipmentSlot)
		{
			m_pEquipmentChanger = new GBuffEquipmentChanger();
			AttachObserver(m_pEquipmentChanger);
		}
	}
	
	if (IsStackable())
	{
		m_pStack = new GBuffStack(this);
		m_pStack->Increase(fDurationTime);
		m_pEffectTimingChecker->AddListener(TC_BUFF_STACKED, m_pModifierApplyer); // 중복 버프 처리를 위한 구독
	}

	m_pRouter = new GBuffRouter(this);
}

GBuff::~GBuff(void)
{
	if (m_pOwner->IsActor())
	{
		GEntityActor* pOwnerActor = ToEntityActor(m_pOwner);
		pOwnerActor->DetachObserver(m_pReleaser);
		pOwnerActor->DetachObserver(m_pEffectTimingChecker);
	}

	DetachObserver(m_pEffectTimingChecker);

	SAFE_DELETE(m_pEffectTimingChecker);
	SAFE_DELETE(m_pReleaser);
	SAFE_DELETE(m_pInstantApplyer);
	SAFE_DELETE(m_pModifierApplyer);
	SAFE_DELETE(m_pTriggerApplyer);
	SAFE_DELETE(m_pRouter);
	SAFE_DELETE(m_pStack);
	SAFE_DELETE(m_pBuffScript);
	SAFE_DELETE(m_pPaletteChanger);
	SAFE_DELETE(m_pEquipmentChanger);
}

bool GBuff::Start()
{
	// 타이머 작동
	m_rgrExpire.Start();
	m_rgrPeriod.Start();
	m_rgrTickCost.Start();
	
	if (!m_pOwner)
		return false; // Owner 없음

	if (!m_pOwner->GetField())
		return false; // 필드 없음

	return OnGain();
}

bool GBuff::Finish(bool bRoute)
{
	return OnLost(bRoute);
}

void GBuff::PreUpdate( float fDelta )
{
	m_pReleaser->Update(fDelta);
}

bool GBuff::Update( float fDelta )
{
	PFI_BLOCK_THISFUNC(6611); // test
	
	if (!m_bGainEffectOccured)
	{
		OnGain();
	}

	// 유지비용 처리
	Update_Maintenance(fDelta);

	// 주기적인 효과
	if (Update_Period(fDelta))
		return true; // 소유자가 죽음, 이미 this는 삭제되었으므로 완료처리는 하지 않음

	// 만료 처리
	Update_Expired(fDelta);

	return false;
}

void GBuff::CancelForced()
{
	m_nDestroyType = DESTROY_CANCELLED;
}

void GBuff::Cancel()
{
	if (m_pStack)
	{
		m_pStack->Decrease();

		if (!m_pStack->IsExpired())
			return; // 스택이 남아있음
	}

	CancelForced();
}

void GBuff::Dispel()
{
	if (m_pStack)
	{
		m_pStack->Decrease();

		if (!m_pStack->IsExpired())
			return; // 스택이 남아있음
	}

	m_nDestroyType = DESTROY_DISPELLED;
}

int GBuff::GetBuffID()
{
	VALID_RET(m_pInfo, false);
	return m_pInfo->m_nID;
}

MUID GBuff::GetUserUID()
{
	return m_uidUser;
}

GTalentInfo* GBuff::GetUserTalentInfo()
{
	return m_pTalentInfo;
}

bool GBuff::HasPeriodEffect()
{
	return m_rgrPeriod.GetTickTime() > 0.0f;
}

bool GBuff::IsInfinite() const
{
	return m_bInifinity;
}

int GBuff::GetBuffStackSlot()
{
	VALID_RET(m_pInfo, false);
	return m_pInfo->m_nStackSlot;
}

int GBuff::GetBuffStackPower()
{
	VALID_RET(m_pInfo, false);
	return m_pInfo->m_nStackPower;
}

float GBuff::GetDurationTime()
{
	return m_rgrExpire.GetTickTime();
}

float GBuff::GetRemainTime()
{
	return m_rgrExpire.GetTickTime() - m_rgrExpire.GetElapsedTime();
}

float GBuff::GetPeriodTime()
{
	return m_rgrPeriod.GetTickTime();
}

float GBuff::GetRemainNextPeriodTime()
{
	return m_rgrPeriod.GetTickTime() - m_rgrPeriod.GetElapsedTime();
}

void GBuff::UpdateBuffTime(float fDurationTime, float fPeriodTime)
{
	m_bInifinity = (fDurationTime == BUFF_DURATION_INFINITY);

	if (!m_bInifinity &&
		fDurationTime < fPeriodTime)
	{
		fDurationTime = fPeriodTime;
	}

	m_rgrExpire.SetElapsedTime(0.0f);
	m_rgrExpire.SetTickTime(fDurationTime);
	m_rgrPeriod.SetTickTime(fPeriodTime);
	m_rgrTickCost.SetTickTime(1.0f);		// 유지비용은 초단위로 지불함
}

bool GBuff::IsGone() const
{
	return GetDestroyType() != DESTROY_NONE;
}

bool GBuff::OnGain()
{
	VALID_RET(m_pOwner, false);
	VALID_RET(m_pInfo, false);
	
	if (m_bGainEffectOccured)
		return false; // 중복 적용됨

	m_bGainEffectOccured = true;

	if (m_pOwner->IsDead())		
		return false;	

	// 버프 얻음 이벤트 호출
	if (m_pOwner->IsActor())
	{
		ToEntityActor(m_pOwner)->OnGainBuff(m_pInfo->m_nID);
	}
	
	__super::OnGain(m_uidUser, m_pOwner, m_pInfo);
		
	return false;
}

bool GBuff::OnLost(bool bRoute)
{
	VALID_RET(m_pOwner, false);
	VALID_RET(m_pInfo, false);

	__super::OnLost(m_uidUser, m_pOwner, m_pInfo, bRoute);

	// 버프 제거 이벤트 호출
	if (m_pOwner->IsActor())
	{
		ToEntityActor(m_pOwner)->OnLostBuff(m_pInfo->m_nID);
	}
			
	return false;
}

void GBuff::OnDestry()
{
	switch (GetDestroyType())
	{
	case DESTROY_EXPIRED:	{	OnExpired(m_uidUser, m_pOwner, m_pInfo);	}break;
	case DESTROY_CANCELLED:	{	OnCancelled();	}break;
	case DESTROY_DISPELLED:	{	OnDispelled();	}break;
	}
}

bool GBuff::OnPeriod()
{
	VALID_RET(m_pOwner, false);
	VALID_RET(m_pInfo, false);
	if (m_pOwner->IsDead())		
		return false;
	
	// 처리 못했던 틱효과 일괄 처리
	int nTickCounter = m_rgrPeriod.GetTickCounter();
	for (int i=0; i<nTickCounter; i++)
	{
		__super::OnPeriod();
	}

	return false;
}

void GBuff::OnStacked( float fDurationTime, float fPeriodTime )
{
	VALID(m_pStack);
	UpdateBuffTime(fDurationTime, fPeriodTime);
	m_pStack->Increase(fDurationTime);

	__super::OnStacked(fDurationTime, fPeriodTime);

	if (GetStackCount() >= GetInfo()->m_nStackMaxCount)
	{
		__super::OnMaxStacked();
	}
}

void GBuff::OnDuplicated( float fDurationTime, float fPeriodTime, MUID uidUser )
{
	m_uidUser = uidUser;
	UpdateBuffTime(fDurationTime, fPeriodTime);
	__super::OnDuplicated(m_uidUser, m_pOwner, m_pInfo);
}

bool GBuff::CheckEvent(TALENT_CONDITION nCondition)
{
	return (m_pInfo->m_Condition.nEffectConditionType == nCondition);
}

void GBuff::Update_Maintenance( float fDelta )
{
	if (!m_pOwner->IsActor())
		return; // 액터가 아니면 유지비용 계산하지 않음

	if (!m_rgrTickCost.IsReady(fDelta))
		return; // 틱 대기

	// 틱 비용을 지불하기 위한 객체
	GUseCostMgr			UseCostMgr;
	bool bPaid = 
		UseCostMgr.Pay_BuffMaintenanceCost(ToEntityActor(m_pOwner), m_pInfo, m_rgrTickCost.GetTickCounter());
	
	if (!bPaid)
	{
		// 비용을 지불하지 못하면 만료
		m_nDestroyType = DESTROY_EXPIRED;
	}
}

bool GBuff::Update_Period( float fDelta )
{
	if (!HasPeriodEffect())
		return false; // 주기 효과 없음

	if (!m_rgrPeriod.IsReady(fDelta))
		return false; // 틱 대기

	return OnPeriod();
}

void GBuff::Update_Expired( float fDelta )
{
	if (!IsInfinite())
	{
		if (m_pStack)
		{
			m_pStack->Update_Expired(fDelta);
			if (m_pStack->IsExpired())
			{
				m_nDestroyType = DESTROY_EXPIRED;
				return;
			}
		}


		if (m_rgrExpire.IsReady(fDelta))
		{
			// 만료시간 지남
			m_nDestroyType = DESTROY_EXPIRED;
			return;
		}
	}
}

GBuff::DESTROY_TYPE GBuff::GetDestroyType() const
{
	return m_nDestroyType;
}

bool GBuff::IsDebuff() const
{
	if (!GetInfo())
		return false;

	return (GetInfo()->IsDebuff());
}

bool GBuff::IsStackable() const
{
	if (!GetInfo())
		return false;

	return (GetInfo()->m_nStackType != BUFFSTACK_NONE);
}

bool GBuff::IsRideBuff() const
{
	if (!GetInfo())
		return false;

	return (GetInfo()->IsRideBuff());
}


int GBuff::GetStackCount() const
{
	if (!m_pStack)
		return 1; // 스택이 없는 것은 1개짜리 버프

	return m_pStack->GetCount();
}

bool GBuff::IsEchant() const
{
	if (!GetInfo())
		return false;

	return GetInfo()->IsEnchant();
}

bool GBuff::IsChangeEquipment() const
{
	if (!GetInfo())
		return false;

	if (GetInfo()->m_bUseChangeEquipmentSlot)
		return true;

	if (GetInfo()->m_bUseChangePalette)
		return true;

	return false;
}

bool GBuff::IsInstantEffect()
{
	return (GetDurationTime() == 0.0f);
}

