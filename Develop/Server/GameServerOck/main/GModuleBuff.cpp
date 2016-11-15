#include "stdafx.h"
#include "GModuleBuff.h"
#include "GEntityActor.h"
#include "GBuffInfo.h"
#include "GBuff.h"
#include "GConst.h"
#include "GMath.h"
#include "GGlobal.h"
#include "GEntityNPC.h"
#include "GTestSystem.h"
#include "GEntityPlayer.h"
#include "GTalent.h"
#include "GCombatCalculator.h"
#include "GTalentHitRouter.h"
#include "GBuffRequirement.h"

GModuleBuff::GModuleBuff(GEntity* pOwner) 
: GModule(pOwner)
, m_fMoveSpeedMod(0.0f)
, m_fSumTickTime(0.0f)
{
	DisablePassiveExtraAttrib();
	DisableActiveExtraAttrib();	

	static const float TICK_TIME = 0.1f;
	m_rgrTick.SetTickTime(TICK_TIME);
	m_rgrTick.Start();
}

GModuleBuff::~GModuleBuff(void)
{
	LostAll(false);
}

void GModuleBuff::RefreshPassiveExtraAttrib()
{
	DisablePassiveExtraAttrib();

	for each (GBuff* pBuff in m_vecBuff)
	{
		if (!pBuff) 
			continue;

		if (pBuff->IsGone()) 
			continue;

		EnablePassiveExtraAttrib(pBuff);
		ApplyPassiveExtraAttrib(pBuff);
	}
}

void GModuleBuff::EnablePassiveExtraAttrib(GBuff* pBuff)
{
	GBuffInfo* pBuffInfo = pBuff->GetInfo();
	VALID(pBuffInfo);

	if (pBuffInfo->IsInvincibility())	
	{
		m_bInvincibility = true;
	}

	if (pBuffInfo->IsInvisibilityToNPC())
	{
		m_bInvisibilityToNPC = true;
	}

	TALENT_CATEGORY nDisableTalentType = pBuffInfo->IsDisableTalent();
	if (nDisableTalentType > TC_INVALID && nDisableTalentType < TC_MAX)
	{
		m_bDisableTalent[nDisableTalentType] = true;
	}
	else if (nDisableTalentType == TC_MAX)
	{
		m_bDisableAllTalent = true;
	}

	if (pBuffInfo->IsMesmerize())
	{
		m_bMesmerize = true;
	}

	if (pBuffInfo->IsInvisibility())
	{
		m_bInvisibility = true;
	}

	if (pBuffInfo->IsAbsoluteGuard())
	{
		m_bAbsoluteGuard = true;
	}

	if (pBuffInfo->IsDisableQuestPVPDrop())
	{
		m_bDisableQuestPVPDrop = true;
	}	
}

void GModuleBuff::ApplyPassiveExtraAttrib(GBuff* pBuff)
{
	if (!m_pOwner->IsActor())
		return;
		
	GEntityActor* pOwnerActor = ToEntityActor(m_pOwner);
	VALID(pOwnerActor);

	GBuffInfo* pBuffInfo = pBuff->GetInfo();
	VALID(pBuffInfo);

	// Disable Talent 처리
	GTalent* pCurTalent = pOwnerActor->GetCurrentTalent();
	if (pCurTalent)
	{
		if (pOwnerActor->IsDisableTalent(pCurTalent->GetInfo()))
		{
			pOwnerActor->doCancelTalentForce(true);
			return;
		}
	}
}

void GModuleBuff::DisablePassiveExtraAttrib()
{
	m_bInvincibility = false;
	m_bInvisibility = false;
	m_bInvisibilityToNPC = false;
	m_bDisableAllTalent = false;
	for (int i=0; i<TC_MAX; i++)
	{
		m_bDisableTalent[i] = false;
	}
	m_bMesmerize = false;
	m_bAbsoluteGuard = false;
	m_bDisableQuestPVPDrop = false;
}

void GModuleBuff::ApplyActiveExtraAttrib()
{
	if (m_bDieable)
	{
		m_pOwner->doDie();
		m_bDieable = false;
	}

	if (m_pOwner->IsNPC())
	{
		GEntityNPC* pNPCOwner = ToEntityNPC(m_pOwner);

		if (m_bDespawnOnceable)
		{	
			pNPCOwner->doDespawn(true);
			gsys.pTestSystem->DespawnLog(pNPCOwner->GetUID(), __FUNCTIONW__);
			m_bDespawnOnceable = false;
		}

		if (m_bDespawnEternalable)
		{			
			pNPCOwner->doDespawn(false);
			gsys.pTestSystem->DespawnLog(pNPCOwner->GetUID(), __FUNCTIONW__);
			m_bDespawnEternalable = false;
		}
	}	
}

void GModuleBuff::EnableActiveExtraAttrib(GBuff* pBuff)
{
	if (NULL == pBuff) return;

	GBuffInfo* pBuffInfo = pBuff->GetInfo();
	VALID(pBuffInfo);


	if (pBuffInfo->IsDieBuff())
	{
		m_bDieable = true;
	}

	if (pBuffInfo->IsDespawnOnceBuff())
	{
		m_bDespawnOnceable = true;
	}

	if (pBuffInfo->IsDespawnEternalBuff())
	{
		m_bDespawnEternalable = true;
	}	
}

void GModuleBuff::DisableActiveExtraAttrib()
{
	m_bDieable = false;
	m_bDespawnOnceable = false;
	m_bDespawnEternalable = false;
}

void GModuleBuff::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(2003);

	GModule::OnUpdate(fDelta);

	if (GetOwner()->IsDead())
	{
		if (HasBuff())
		{
			bool bRoute = GetOwner()->IsPlayer();
			LostAll(bRoute);
		}

		return;	// 소유자가 죽음
	}

	m_fSumTickTime += fDelta;

	if (!m_rgrTick.IsReady(fDelta))
		return; // 틱 시간이 되지 않음
	
	UpdateLazyGainBuffs();	
	BuffUpdate(m_vecBuff, m_fSumTickTime);
	
	ApplyActiveExtraAttrib();

	m_fSumTickTime = 0.0f;
}

void GModuleBuff::UpdateLazyGainBuffs()
{
	// Gain 이벤트에서 LazyBuff를 또 얻을 수 있음 (재귀 순회 문제)
	vector<LazyGainInfo> vecOldLazyGainBuff = m_vecLazyGainBuffs;
	m_vecLazyGainBuffs.clear();

	for each (const LazyGainInfo& each in vecOldLazyGainBuff)
	{
		GainBuffForced(each.pBuffInfo, each.fDurationTime, each.fPeriodTime, NULL, each.uidUser);
	}
}

int GModuleBuff::GetBuffQty()
{
	return int(m_vecBuff.size());
}

GBuff* GModuleBuff::FindBuffBySlot( int BuffStackSlot )
{
	for each (GBuff* pBuff in m_vecBuff)
	{
		if (BuffStackSlot == pBuff->GetBuffStackSlot())
		{
			return pBuff;
		}
	}
	return NULL;
}

GBuff* GModuleBuff::FindBuffByID( int BuffID )
{
	for each (GBuff* pBuff in m_vecBuff)
	{
		if (BuffID == pBuff->GetBuffID())
		{
			return pBuff;
		}
	}
	return NULL;
}

vector<int> GModuleBuff::GetAllBuffs()
{
	vector<int> vecRet;

	for each (GBuff* pBuff in m_vecBuff)
	{
		DCHECK(pBuff->GetInfo());
		if (pBuff->GetInfo()->IsEnchant())
			continue; // 강화 버프는 공개하지 않음

		vecRet.push_back(pBuff->GetBuffID());
	}

	return vecRet;
}

bool GModuleBuff::IsGained_ForTest( int nBuffID )
{
	for each (GBuff* pBuff in m_vecBuff)
	{
		if (!pBuff)
			continue;

		if (pBuff->IsGone())
			continue;

		if (pBuff->GetBuffID() == nBuffID)	
			return true;
	}

	return false;
}

bool GModuleBuff::IsGained( int nBuffID )
{
	for each (GBuff* pBuff in m_vecBuff)
	{
		if (!pBuff)
			continue;
		
		if (!pBuff->GetInfo())
			continue;

		if (pBuff->GetInfo()->IsEnchant())
			continue;

		if (pBuff->IsGone())
			continue;
			
		if (pBuff->GetBuffID() == nBuffID)	
			return true;
	}

	return false;
}

bool GModuleBuff::IsStackable(int nBuffID)
{
	GBuff* pGainedBuff = FindBuffByID(nBuffID);
	if (!pGainedBuff)										
		return false;

	if (!pGainedBuff->GetInfo())
		return false;

	if (pGainedBuff->GetInfo()->IsEnchant())
		return false;

	if (!pGainedBuff->IsStackable())
		return false;

	return true;
}

bool GModuleBuff::IsGainedPowerfulBuff(int nNewBuffSlot, int nNewBuffPower)
{
	if (nNewBuffSlot == INVALID_ID)
		return false; // 무효한 버프스택슬롯은 무시
	GBuff* pGainedBuff = FindBuffBySlot(nNewBuffSlot);
	if (!pGainedBuff)										
		return false;
	if (pGainedBuff->GetBuffStackPower() <= nNewBuffPower)	
		return false;

	return true;
}

bool GModuleBuff::GainEffect( GBuff* pBuff )
{
	VALID_RET(pBuff, false);
	GBuffInfo* pBuffInfo = pBuff->GetInfo();
	VALID_RET(pBuffInfo, false);

	if (GetOwner()->IsDead())
		return false;	// 소유자가 죽음 (죽은 상태에서 버프가 걸릴 가능성)
	
	if (pBuff->Start())
		return false; // 소유자가 죽음 (버프 얻는 효과에서 죽을 가능성)
	m_vecBuff.push_back(pBuff);	
	RefreshPassiveExtraAttrib();

	if (m_pOwner->IsActor() &&
		ToEntityActor(m_pOwner)->IsBeginUnableAction())
	{
		ToEntityActor(m_pOwner)->OnBeginUnableAction();
	}

	// 다음틱에 바로 효과얻기
	m_rgrTick.SetElapsedTime(m_rgrTick.GetTickTime());
	return true;
}

void GModuleBuff::DestroyBuff( GBuff* pBuff, bool bRoute )
{
	VALID(pBuff);
	GBuffInfo* pBuffInfo = pBuff->GetInfo();
	VALID(pBuffInfo);

	RefreshPassiveExtraAttrib();
	pBuff->Finish(bRoute);

	if (m_pOwner->IsActor() &&
		ToEntityActor(m_pOwner)->IsEndUnableAction())
	{
		ToEntityActor(m_pOwner)->OnEndUnableAction();
	}

	// 다음틱에 바로 효과얻기
	m_rgrTick.SetElapsedTime(m_rgrTick.GetTickTime());
	
	SAFE_DELETE(pBuff);
}

void GModuleBuff::CancelBuff( int nBuffID )
{
	GBuff* pBuff = FindBuffByID(nBuffID);
	if (!pBuff)
		return; // 가지고 있지 않은 버프

	pBuff->Cancel();

	// 다음틱에 바로 효과얻기
	m_rgrTick.SetElapsedTime(m_rgrTick.GetTickTime());
}

bool GModuleBuff::GainBuff( GBuffInfo* pBuffInfo, const CSBuffEnchantInfo& BuffInvokeInfo, GTalentInfo* pTalentInfo/*=NULL*/, GEntityActor* pUser )
{
	VALID_RET(pBuffInfo, false);
	return GainBuffDetail(pBuffInfo, BuffInvokeInfo, pTalentInfo, pUser, pBuffInfo->m_fDuration, pBuffInfo->m_fPeriod);
}

bool GModuleBuff::GainBuffDetail( GBuffInfo* pBuffInfo, const CSBuffEnchantInfo& BuffInvokeInfo, GTalentInfo* pTalentInfo/*=NULL*/, GEntityActor* pUser, float fDuration, float fPeriod )
{
	VALID_RET(pBuffInfo, false);
	if (ProcessGainResist(BuffInvokeInfo, pUser, pTalentInfo))
		return false;

	return GainBuffForced(pBuffInfo, fDuration, fPeriod, pTalentInfo, pUser?pUser->GetUID():MUID::Invalid());
}

bool GModuleBuff::GainBuffForced( GBuffInfo* pBuffInfo, float fDurationTime, float fPeriodTime, GTalentInfo* pTalentInfo/*=NULL*/, MUID uidUser/*=MUID::Invalid()*/ )
{
	RVALID_RET(fDurationTime >= BUFF_DURATION_INFINITY, false);
	RVALID_RET(fPeriodTime >= 0.0f, false);

	// 버프 필요조건 처리
	GBuffRequirement buff_requirement;
	if (m_pOwner->IsActor() &&
		!buff_requirement.ProcessRequirement(ToEntityActor(m_pOwner), pBuffInfo->m_RequireBuff))
	{
		//ToEntityActor(m_pOwner)->RouteImmune(pTalentInfo);
		return false; 
	}

	// 면역 처리
	if (m_pOwner->IsActor() &&
		ToEntityActor(m_pOwner)->GetImmuneHandler().IsBuffImmune(pBuffInfo->m_nPassiveExtraAttrib))
	{
		//ToEntityActor(m_pOwner)->RouteImmune(pTalentInfo);
		return false;
	}

	const int nBuffID = pBuffInfo->m_nID;
	const int nStackSlot = pBuffInfo->m_nStackSlot;
	const int nStackPower = pBuffInfo->m_nStackPower;

	// 버프 스택 처리
	if (IsStackable(nBuffID))
	{
		GBuff* pBuff = FindBuffByID(nBuffID);
		pBuff->OnStacked(fDurationTime, fPeriodTime);
		return true;
	}

	// 이미 같은 슬롯에 강한 버프가 걸려있다면 무시
	if (IsGainedPowerfulBuff(nStackSlot, nStackPower))
		return false;

	if (IsGained(nBuffID))
	{
		// 중복되서 걸린 경우
		GBuff* pBuff = FindBuffByID(nBuffID);
		pBuff->OnDuplicated(fDurationTime, fPeriodTime, uidUser); 
		return true;
	}

	// 최대 버프 소유 개수이상 버프를 걸지 않음
//	if (MAX_OWN_BUFF_NUMBER <= GetBuffQty())
//		return false;

	// 같은 슬롯의 모든 버프를 해제
	if (nStackSlot != INVALID_ID) // 무효한 아이디면 중복검사 하지 않음
	{
		LostBuffStack(nStackSlot);
	}
	
	// 버프 효과를 얻음
	GBuff* pBuff = new GBuff(static_cast<GEntitySync*>(m_pOwner), pBuffInfo, fDurationTime, fPeriodTime, pTalentInfo, uidUser);	
	VALID_RET(pBuff, false);
	
	if (!GainEffect(pBuff))
	{
		SAFE_DELETE(pBuff);
		return false;	
	}

	return true;
}

void GModuleBuff::LazyGainBuff( GBuffInfo* pBuffInfo, float fDurationTime, float fPeriodTime, MUID uidUser )
{
	LazyGainInfo info;
	info.pBuffInfo = pBuffInfo;
	info.fDurationTime = fDurationTime;
	info.fPeriodTime = fPeriodTime;
	info.uidUser = uidUser;

	m_vecLazyGainBuffs.push_back(info);
}

void GModuleBuff::LostBuffStack(int nBuffStackSlot)
{
	for (vector<GBuff*>::iterator iter = m_vecBuff.begin();		iter != m_vecBuff.end();		++iter)
	{
		GBuff* pBuff = (*iter);

		if (pBuff->GetBuffStackSlot() == nBuffStackSlot)
		{
			m_vecBuff.erase(iter);			DestroyBuff(pBuff, true);
			return;
		}
	}
}

void GModuleBuff::LazyLostAll()
{
	for (vector<GBuff*>::iterator iter = m_vecBuff.begin();		iter != m_vecBuff.end();		++iter)
	{
		GBuff* pBuff = (*iter);

		if (pBuff->IsEchant())
			continue; // 강화 버프는 잃지 않음

		pBuff->Cancel();
	}

}

void GModuleBuff::LostAll(bool bRoute)
{
	for (vector<GBuff*>::iterator iter = m_vecBuff.begin();	iter != m_vecBuff.end();)
	{
		GBuff* pBuff = (*iter);

		if (pBuff->IsEchant())
		{
			++iter;
			continue; // 강화 버프는 잃지 않음
		}

		iter = m_vecBuff.erase(iter);
		DestroyBuff(pBuff, bRoute);
	}
}

void GModuleBuff::Dispel(int nLimitPower)
{
	DispelLatestBuff(DISPEL_MAGIC, nLimitPower);
}

void GModuleBuff::CurePoison(int nLimitPower)
{
	DispelLatestBuff(DISPEL_POISON, nLimitPower);	
}

void GModuleBuff::CureDisease(int nLimitPower)
{
	DispelLatestBuff(DISPEL_DISEASE, nLimitPower);	
}

void GModuleBuff::CureCurse(int nLimitPower)
{
	DispelLatestBuff(DISPEL_CURSE, nLimitPower);	
}

void GModuleBuff::DispelMesmerize(int nLimitPower)
{
	DispelLatestBuff(DISPEL_MESMERIZE, nLimitPower);
}

void GModuleBuff::DispelDeathPenalty( int nLimitPower )
{
	DispelLatestBuff(DISPEL_DEATH_PENALTY, nLimitPower);
}

void GModuleBuff::DispelBPart()
{
	DispelLatestBuff(DISPEL_BPART, 9999);
}

void GModuleBuff::DispelBuff(int nLimitPower)
{
	DispelLatestBuff(DISPEL_BUFF, nLimitPower);
}

void GModuleBuff::DispelFocus()
{
	for each (int nFocusBuffID in BUFF_FOCUS_ID)
	{
		DispelSingleBuffImpl(nFocusBuffID);
	}
}

void GModuleBuff::DispelBuff(int nDispellQty, int nLimitPower )
{
	VALID(nDispellQty <= GConst::MAX_BUFF_QTY);
	if (nDispellQty <= 0) 		return;
	if (m_vecBuff.empty())				return;


	// 면역 처리
	if (m_pOwner->IsActor() &&
		ToEntityActor(m_pOwner)->GetImmuneHandler().IsImmune(immune::IMMUNE_DISPEL))
	{
		return;
	}

	// 디스펠 처리
	for (int i=0; i<nDispellQty; ++i)
	{
		int nIndex = GMath::RandomNumber(0, int(m_vecBuff.size())-1);
		GBuff* pPickedBuff = m_vecBuff.at(nIndex);
		GBuffInfo* pBuffInfo = pPickedBuff->GetInfo();
		if (!pBuffInfo)
			continue;
		
		// 디스펠시킬 파워보다 크다면 무시
		if (pPickedBuff->GetBuffStackPower() > nLimitPower)
			continue;

		// 취소 불가능한 버프라면 무시
		if (!pBuffInfo->IsCancelable())
			continue;

		DispelSingleBuffImpl(pPickedBuff->GetBuffID());
		if (m_vecBuff.empty())			return;	}
}

void GModuleBuff::RemoveAllDebuff()
{
	if (m_vecBuff.empty())
		return;

	for each (GBuff* each in m_vecBuff)
	{
		if (each->IsDebuff())
		{
			DispelSingleBuffImpl(each->GetBuffID());
		}
	}
}

void GModuleBuff::RemoveAllChangeEquipmentBuffs(int nExceptBuff)
{
	if (m_vecBuff.empty())
		return;

	for each (GBuff* each in m_vecBuff)
	{
		if (each->IsChangeEquipment() &&
			nExceptBuff != each->GetBuffID())
		{
			DispelSingleBuffImpl(each->GetBuffID());
		}
	}
}


void GModuleBuff::DispelSingleBuffImpl(int nBuffID)
{
	GBuff* pBuff = FindBuffByID(nBuffID);
	if (!pBuff)
		return; // 가지고 있지 않은 버프

	pBuff->Dispel();
}

float GModuleBuff::GetBuffRemainTime(int nBuffID)
{
	GBuff* pBuff = FindBuffByID(nBuffID);
	if (NULL == pBuff) return 0.0f;

	return pBuff->GetRemainTime();
}

void GModuleBuff::CheckDuplicatedRemainBuff(GEntity* pOwner, vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, const int nBuffID, const float fRemainTime)
{
	GEntityPlayer* pPlayer = ToEntityPlayer(pOwner);
	VALID(pPlayer);

	for(vector<REMAIN_BUFF_TIME>::iterator it = outvecBuffRemainTime.begin(); it != outvecBuffRemainTime.end(); it++)
	{
		if (it->nID == nBuffID)
		{
			dlog(L"Duplicated remain BuffID(%d, CID:%d).\n"
			, nBuffID
			, pPlayer->GetPlayerInfo()->nCID);			

			_ASSERT(0 && L"버프중복");
		}
	}
}

void GModuleBuff::GetBuffRemainTimes(vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, bool bExceptFocusBuff, bool bExceptNoSaveDB)
{
	for each (GBuff* pBuff in m_vecBuff)
	{
		GBuffInfo* pBuffInfo = pBuff->GetInfo();
		if (!pBuffInfo)
			continue;

		// 포커스 제외
		if (true == bExceptFocusBuff)
		{
			if (true == pBuffInfo->IsFocusBuff()) continue;
		}

		// DB에 저장하지 않는 것 제외
		if (true == bExceptNoSaveDB)
		{
			if (false == pBuffInfo->m_bSaveToDB) continue;
		}

		if (1.0f >= pBuff->GetRemainTime())
			continue;

		if (IsRunForTest())
		{
			CheckDuplicatedRemainBuff(GetOwner(), outvecBuffRemainTime, pBuff->GetBuffID(), pBuff->GetRemainTime());
		}

		REMAIN_BUFF_TIME remainBuffTime;
		
		remainBuffTime.nID = pBuff->GetBuffID();
		remainBuffTime.fRemainDurationSeconds = pBuff->GetRemainTime();
		remainBuffTime.fRemainNextPeriodSeconds = pBuff->GetRemainNextPeriodTime();

		outvecBuffRemainTime.push_back(remainBuffTime);
	}	
}

void GModuleBuff::InsertBuffRemainTimes(const vector<REMAIN_BUFF_TIME>& vecBuffRemainTime)
{
	for(vector<REMAIN_BUFF_TIME>::const_iterator it = vecBuffRemainTime.begin(); it != vecBuffRemainTime.end(); it++)
	{
		int nBuffID = it->nID;
		float fRemainTime = it->fRemainDurationSeconds;

		GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
		if (pBuffInfo == NULL)	continue;

		GainBuffForced(pBuffInfo, fRemainTime, pBuffInfo->m_fPeriod);	}
}

bool GModuleBuff::GainInvincibleBuff_Rebirth()
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(GConst::INVINCIBLE_BUFF_ID);
	if (NULL == pBuffInfo) return false;

	GainBuffForced(pBuffInfo, GConst::INVINCIBLE_TIME_REVIVE, 0.0f);
	return true;
}

bool GModuleBuff::GainInvincibleBuff_EnterField()
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(GConst::INVINCIBLE_BUFF_ID);
	if (NULL == pBuffInfo) return false;

	GainBuffForced(pBuffInfo, GConst::INVINCIBLE_TIME_ENTER_FIELD, 0.0f);
	return true;
}

bool GModuleBuff::GainInvincibleBuff_Infinity()
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(GConst::INVINCIBLE_BUFF_ID);
	if (NULL == pBuffInfo) return false;

	GainBuffForced(pBuffInfo, BUFF_DURATION_INFINITY, 0.0f);
	return true;
}

bool GModuleBuff::GainInvisibleToNPCBuff_Infinity()
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(GConst::INVISIBLETONPC_BUFF_ID);
	if (NULL == pBuffInfo) return false;

	GainBuffForced(pBuffInfo, BUFF_DURATION_INFINITY, 0.0f);
	return true;
}

bool GModuleBuff::LostInvincibleBuff()
{
	CancelBuff(GConst::INVINCIBLE_BUFF_ID);

	return true;
}

bool GModuleBuff::LostInvisibleToNPCBuff()
{
	CancelBuff(GConst::INVISIBLETONPC_BUFF_ID);

	return true;
}

void GModuleBuff::BuffUpdate( vector<GBuff*>& vecBuff, float fDelta )
{
	if (vecBuff.empty())
		return;

	vector<GBuff*>::iterator it = vecBuff.begin();
	int i=0;
	for (it; it != vecBuff.end();)
	{
		GBuff* pBuff = *it;
		pBuff->PreUpdate(fDelta);

		if (pBuff->Update(fDelta))
			return; // 소유자가 죽었음
		
		if (pBuff->IsGone())
		{
			// OnDestry()에서 버프제거 통보 시, vecBuff를 참조하지 못하도록 먼저 제거한다.
			it = vecBuff.erase(it);
			
			pBuff->OnDestry();
			DestroyBuff(pBuff, true);
		}
		else
		{
			++it;
		}
	}
}

bool GModuleBuff::HasBuff() const
{
	return (!m_vecBuff.empty());
}

int GModuleBuff::GetDebuffQty()
{
	int nCount = 0;
	for (vector<GBuff*>::iterator iter = m_vecBuff.begin();		iter != m_vecBuff.end();		++iter)
	{
		GBuff* pBuff = (*iter);
		VALID_RET(pBuff->GetInfo(), -1);
		if (pBuff->GetInfo()->IsDebuff())
		{
			++nCount;
		}
	}

	return nCount; 
}

void GModuleBuff::DispelLatestBuff(DISPEL_TYPE dispelType, int nLimitPower )
{
	// 면역
	if (m_pOwner->IsActor() && ToEntityActor(m_pOwner)->GetImmuneHandler().IsImmune(immune::IMMUNE_DISPEL))
	{
		return;
	}


	// 최근 버프부터 해제 시도
	for(vector<GBuff*>::reverse_iterator it = m_vecBuff.rbegin(); it != m_vecBuff.rend(); it++)
	{
		GBuff* pBuff = *it;
		GBuffInfo* pBuffInfo = pBuff->GetInfo();

		// 해제불가
		if (pBuffInfo == NULL)	continue;
		if (!pBuffInfo->IsCancelable())	continue;


		// 디스펠 타입
		int nStackSlot = pBuff->GetBuffStackSlot();		
		if (!IsDispelTypeBuff(dispelType, nStackSlot))	continue;
		

		// 파워
		if (pBuff->GetBuffStackPower() <= nLimitPower)
		{
			int nBuffID = pBuff->GetBuffID();
			DispelSingleBuffImpl(nBuffID);
			return;
		}
	}
}

bool GModuleBuff::IsDispelTypeBuff(DISPEL_TYPE dispelType, int nStackSlot)
{
	switch(dispelType)
	{
	case DISPEL_MAGIC:		
		if (100 <= nStackSlot && nStackSlot < 200)	return true;
		break;
	case DISPEL_POISON:
		if (200 <= nStackSlot && nStackSlot < 300)	return true;
		break;
	case DISPEL_DISEASE:
		if (300 <= nStackSlot && nStackSlot < 400)	return true;
		break;
	case DISPEL_CURSE:
		if (400 <= nStackSlot && nStackSlot < 500)	return true;
		break;
	case DISPEL_MESMERIZE:
		if (500 <= nStackSlot && nStackSlot < 600)	return true;
		break;	
	case DISPEL_DEATH_PENALTY:
		if (600 <= nStackSlot && nStackSlot < 700)	return true;
		break;	
	case DISPEL_BUFF:
		if (1000 <= nStackSlot)	return true;
		break;
	case DISPEL_BPART:
		if (GConst::BPART_BUFFSTACK_ID == nStackSlot)	return true;
		break;
	default:
		// do nothing
		break;
	}

	return false;
}

bool GModuleBuff::ProcessGainResist( const CSBuffEnchantInfo &BuffInvokeInfo, GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	VALID_RET(BuffInvokeInfo.Resist.m_nMethod == TRM_BINARY, false);

	float fResistRatePercent = 100.0f;
	if (m_pOwner->IsActor() &&
		pUser && 
		gsys.pCombatCalculator->CheckResist(fResistRatePercent, pUser, ToEntityActor(m_pOwner), BuffInvokeInfo.Resist))
	{
		// 저항 성공 패킷 전송
		uint32 nCombatResultFlags=0;
		SetBitSet(nCombatResultFlags, CTR_RESIST_PERFECT);

		GTalentHitRouter talentHitRouter;
		talentHitRouter.Route_Resist(pUser, ToEntityActor(m_pOwner), pTalentInfo, 0, nCombatResultFlags);

		return true;
	}

	return false;
}

