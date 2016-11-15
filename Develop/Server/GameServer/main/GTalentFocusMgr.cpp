#include "StdAfx.h"
#include "GTalentFocusMgr.h"
#include "GTalentFocus_Counter.h"
#include "GTalentFocus_Berserk.h"
#include "GTalentFocus_Precision.h"
#include "GTalentFocus_Enlighten.h"
#include "GTalentFocus_Advent.h"
#include "GTalentFocus_Sneak.h"
#include "GEntityPlayer.h"
#include "GCommandTable.h"
#include "GConst.h"
#include "GCommand.h"

GTalentFocusMgr::GTalentFocusMgr(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nLastBerserkBubble(0)
{
	memset(m_pFocusTable, NULL, sizeof(m_pFocusTable));

	m_pOwner->AttachObserver(this);
}

GTalentFocusMgr::~GTalentFocusMgr(void)
{
	m_pOwner->DetachObserver(this);
}

void GTalentFocusMgr::Init()
{
	m_pFocusTable[TFT_COUNTER]		= new GTalentFocus_Counter(m_pOwner);
	m_pFocusTable[TFT_BERSERK]		= new GTalentFocus_Berserk(m_pOwner);
	m_pFocusTable[TFT_PRECISION]	= new GTalentFocus_Precision(m_pOwner);
	m_pFocusTable[TFT_ENLIGHTEN]	= new GTalentFocus_Enlighten(m_pOwner);
	m_pFocusTable[TFT_ADVENT]		= new GTalentFocus_Advent(m_pOwner);
	m_pFocusTable[TFT_SNEAK]		= new GTalentFocus_Sneak(m_pOwner);
}

void GTalentFocusMgr::Fini()
{
	for (int i=0; i<TFT_SIZE; ++i)
	{
		SAFE_DELETE(m_pFocusTable[i]);
	}
}

void GTalentFocusMgr::Update( float fDelta )
{
	PFI_BLOCK_THISFUNC(9056);

	for (int i=0; i<TFT_SIZE; ++i)
	{
		GTalentFocus* pEachFocus = m_pFocusTable[i];
		if (!pEachFocus)	continue;

		bool bExpired = pEachFocus->Update(fDelta);
		if (bExpired)
		{
			LostFocus(pEachFocus->GetType());
		}
	}

	UpdateBubbleQty();
}

void GTalentFocusMgr::UpdateBubbleQty()
{
	PFI_BLOCK_THISFUNC(9057);

	int m_nBerserkBubble = GetBerserkBubble();

	// 버블 갯수가 바뀌었으면 클라이언트에게 알려준다.
	if (m_nBerserkBubble != m_nLastBerserkBubble)
		Route_UpdateBubble(TFT_BERSERK, m_nBerserkBubble, GetBerserkMaxBubble());

	m_nLastBerserkBubble = m_nBerserkBubble;
}


int GTalentFocusMgr::GetBerserkBubble()
{
	GTalentFocus_Berserk* pBerserkFocus = GetBerserkFocus();
	VALID_RET(pBerserkFocus, 0);
	return pBerserkFocus->GetHitCount();
}

int GTalentFocusMgr::GetBerserkMaxBubble()
{
	GTalentFocus_Berserk* pBerserkFocus = GetBerserkFocus();
	VALID_RET(pBerserkFocus, 0);
	return pBerserkFocus->GetMaxHitCount();
}

void GTalentFocusMgr::CheckGainFocus(TALENT_FOCUS_TYPE nFocus, int nBuffType)
{
	// 해당 포커스로 바꿀 수 있는 조건이 안된다면 무시
	GTalentFocus* pFocus = GetFocus(nFocus);
	VALID(pFocus);
	if (!pFocus->CheckActivate())
		return;

	if (!pFocus->CheckEquipItem())
		return;

	GainFocus(nFocus, nBuffType);
}

void GTalentFocusMgr::GainFocus( TALENT_FOCUS_TYPE nFocus, int nBuffType )
{
	VALID(nFocus > TFT_NONE);
	VALID(nFocus < TFT_SIZE);

	GTalentFocus* pCurFocus = GetFocus(nFocus);
	VALID(pCurFocus);

	if (!pCurFocus->CheckLicense())
		return; // 라이센스를 배우지 않음

	if (HasFocus(nFocus) == false)
	{
		m_setActiveFocus.insert(nFocus);
	}

	pCurFocus->SetBuffType(nBuffType);
	pCurFocus->Active();

	Route_GainFocus(nFocus, pCurFocus->GetDurationTime());
}

void GTalentFocusMgr::LostFocus(TALENT_FOCUS_TYPE nFocus)
{
	VALID(nFocus > TFT_NONE);
	VALID(nFocus < TFT_SIZE);

	if (!HasFocus(nFocus))
		return; // 없애야할 포커스가 없을 경우 무시

	GTalentFocus* pCurFocus = GetFocus(nFocus);
	VALID(pCurFocus);
	pCurFocus->Deactive();

	m_setActiveFocus.erase(nFocus);

	Route_LostFocus(nFocus);
}

void GTalentFocusMgr::LostAllFocus()
{
	// set 컨테이너의 원소가 순환중에 삭제되므로, 복사한 후 처리
	vector<TALENT_FOCUS_TYPE> vecFocus(m_setActiveFocus.begin(), m_setActiveFocus.end());

	for each (TALENT_FOCUS_TYPE nFocus in vecFocus)
	{
		LostFocus(nFocus);
	}
}

bool GTalentFocusMgr::HasFocus()
{
	return !m_setActiveFocus.empty();
}

bool GTalentFocusMgr::HasFocus( TALENT_FOCUS_TYPE nFocus )
{
	return (m_setActiveFocus.find(nFocus) != m_setActiveFocus.end());
}

GTalentFocus* GTalentFocusMgr::GetFocus( TALENT_FOCUS_TYPE nFocus )
{
	VALID_RET(nFocus > TFT_NONE, NULL);
	VALID_RET(nFocus < TFT_SIZE, NULL);
	GTalentFocus* pFocus = m_pFocusTable[nFocus];
	VALID_RET(pFocus, NULL);
	return pFocus;
}

GTalentFocus_Counter* GTalentFocusMgr::GetCounterFocus()
{
	return static_cast<GTalentFocus_Counter*>(GetFocus(TFT_COUNTER));
}

GTalentFocus_Berserk* GTalentFocusMgr::GetBerserkFocus()
{
	return static_cast<GTalentFocus_Berserk*>(GetFocus(TFT_BERSERK));
}

GTalentFocus_Precision* GTalentFocusMgr::GetPrecisionFocus()
{
	return static_cast<GTalentFocus_Precision*>(GetFocus(TFT_PRECISION));
}

GTalentFocus_Enlighten* GTalentFocusMgr::GetEnlightenFocus()
{
	return static_cast<GTalentFocus_Enlighten*>(GetFocus(TFT_ENLIGHTEN));
}	

GTalentFocus_Sneak* GTalentFocusMgr::GetSneakFocus()
{
	return static_cast<GTalentFocus_Sneak*>(GetFocus(TFT_SNEAK));
}

GTalentFocus_Advent* GTalentFocusMgr::GetAdventFocus()
{
	return static_cast<GTalentFocus_Advent*>(GetFocus(TFT_ADVENT));
}

void GTalentFocusMgr::Route_GainFocus(TALENT_FOCUS_TYPE nFocus, float fDurationTime)
{
	VALID(m_pOwner);

	MCommand* pNewCommand = MakeNewCommand(MC_FOCUS_GAIN, 3, 
		NEW_UID(m_pOwner->GetUID()), 
		NEW_INT(nFocus),
		NEW_FLOAT(fDurationTime));
	m_pOwner->RouteToThisCell(pNewCommand);
}

void GTalentFocusMgr::Route_LostFocus(TALENT_FOCUS_TYPE nFocus)
{
	VALID(m_pOwner);

	MCommand* pNewCommand = MakeNewCommand(MC_FOCUS_LOST, 2, 
		NEW_UID(m_pOwner->GetUID()), NEW_INT(nFocus));
	m_pOwner->RouteToThisCell(pNewCommand);
}

void GTalentFocusMgr::Route_UpdateBubble( TALENT_FOCUS_TYPE nFocus, uint8 nBubbleQty, uint8 nMaxBubbleQty )
{
	VALID(m_pOwner);

	uint8 nBubblePercentage = (uint8)(nBubbleQty*100/nMaxBubbleQty);

	MCommand* pNewCommand = MakeNewCommand(MC_FOCUS_UPDATE_BUBBLE, 2, 
		NEW_INT(nFocus), NEW_UCHAR(nBubblePercentage));
	m_pOwner->RouteToMe(pNewCommand);
}

void GTalentFocusMgr::ToggleFocus(TALENT_FOCUS_TYPE nFocus, int nBuffType)
{
	if (HasFocus(nFocus))
	{
		LostFocus(nFocus);
	}
	else
	{
		CheckGainFocus(nFocus, nBuffType);
	}
}

void GTalentFocusMgr::OnGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)
{
	GTalentFocus_Counter* pCounterFocus = GetCounterFocus();
	pCounterFocus->OnGuard(pAttacker);
	CheckGainFocus(pCounterFocus->GetType());
}

void GTalentFocusMgr::OnAbsoluteGuard(GEntityActor* pOwner,  GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	GTalentFocus_Counter* pCounterFocus = GetCounterFocus();
	pCounterFocus->OnAbsoluteGuard(pAttacker);
	CheckGainFocus(pCounterFocus->GetType());
}

void GTalentFocusMgr::OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	// 피격 이벤트시 포커스 얻기 처리
	if (pTalentInfo->m_nTiming == TC_HIT_ALL_TALENT &&
		pTalentInfo->HasNextFocus())
	{
		GainFocus(pTalentInfo->m_nNextFocus);
	}
}

void GTalentFocusMgr::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	// 피격 이벤트시 포커스 얻기 처리
	if (pTalentInfo->m_nTiming == TC_HIT_TALENT &&
		pTalentInfo->HasNextFocus())
	{
		GainFocus(pTalentInfo->m_nNextFocus);
	}

	switch(pTalentInfo->m_nCategory)
	{
	case TC_MELEE:	OnHitMeleeTalent(pTalentInfo);		break;
	}
}

void GTalentFocusMgr::OnHitMeleeTalent(GTalentInfo* pHitTalentInfo)
{
	GTalentFocus_Berserk* pBerserkFocus = GetBerserkFocus();
	pBerserkFocus->OnHitMeleeTalent(pHitTalentInfo);
	CheckGainFocus(pBerserkFocus->GetType());
}

void GTalentFocusMgr::OnDie()
{
	LostAllFocus();
}

void GTalentFocusMgr::OnTogglePrecision()
{
	ToggleFocus(TFT_PRECISION);
}

void GTalentFocusMgr::OnToggleSneak(int nSneakBuffType)
{
	ToggleFocus(TFT_SNEAK, nSneakBuffType);
}

void GTalentFocusMgr::OnApplyAdvent()
{
	CheckGainFocus(TFT_ADVENT);
}

void GTalentFocusMgr::OnGainBuff( int nBuffID )
{
	// 각성 버프 처리
	if (!HasFocus(TFT_ENLIGHTEN))
	{
		GTalentFocus_Enlighten* pFocus = GetEnlightenFocus();
		VALID(pFocus);
		if (nBuffID != pFocus->GetFocusBuffID())
			return;

		if (!pFocus->CheckEquipItem())
			return;

		GainFocus(pFocus->GetType());
	}
}

void GTalentFocusMgr::OnLostBuff( int nBuffID )
{
	// 포커스 버프 해제 체크
	for (int i=0; i<TFT_SIZE; i++)
	{
		if (nBuffID == m_pFocusTable[i]->GetFocusBuffID())
		{
			LostFocus((TALENT_FOCUS_TYPE)i);
			return;
		}
	}
}

void GTalentFocusMgr::OnUseTalent(GEntityActor* pUser, GTalentInfo* pTalentInfo)
{
	VALID(pTalentInfo);

	PFI_B(3700, "GTalentFocusMgr::OnUseTalent");

	// 탤런트 사용이 시작되는 이벤트시 포커스 얻기 처리
	if (pTalentInfo->m_nTiming == TC_USE_TALENT &&
		pTalentInfo->HasNextFocus())
	{
		GainFocus(pTalentInfo->m_nNextFocus);
	}

	PFI_E(3700);
}

void GTalentFocusMgr::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	// 탤런트사용 이벤트시 포커스 얻기 처리
	if (pTalentInfo->m_nTiming == TC_FINISH_TALENT &&
		pTalentInfo->HasNextFocus())
	{
		GTalentFocus* pFocus = GetFocus(pTalentInfo->m_nNextFocus);
		VALID(pFocus);
		if (!pFocus->CheckEquipItem())
			return;

		GainFocus(pFocus->GetType());
	}
}

void GTalentFocusMgr::OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	// 포커스가 일회성이면 연계포커스가 없는 탤런트일경우, 사용후 제거한다.
	if (pTalentInfo->HasRequireFocus() &&
		!pTalentInfo->HasNextFocus())
	{
		GTalentFocus* pFocus = GetFocus(pTalentInfo->m_nRequireFocus);
		VALID(pFocus);
		if (pFocus->IsOnce())
		{
			LostFocus(pFocus->GetType());
		}
	}
}

void GTalentFocusMgr::OnItemEquipped( GItem* pItem )
{
	CheckEquipItemForAllFocus();
}

void GTalentFocusMgr::OnItemUnequipped( GItem* pItem )
{
	CheckEquipItemForAllFocus();
}

void GTalentFocusMgr::OnSwitchingWeaponSet( SH_ITEM_SWITCH_WEAPON val )
{
	CheckEquipItemForAllFocus();
}

void GTalentFocusMgr::OnInteraction()
{
	if (true == HasFocus(TFT_SNEAK))
	{
		LostFocus(TFT_SNEAK);
	}
}

void GTalentFocusMgr::CheckEquipItemForAllFocus()
{
	if (m_setActiveFocus.empty())
		return;

	vector<TALENT_FOCUS_TYPE> vecLostFocuses;
	for each (TALENT_FOCUS_TYPE nFocus in m_setActiveFocus)
	{
		GTalentFocus* pFocus = GetFocus(nFocus);
		DCHECK(pFocus);
		if (!pFocus->CheckEquipItem())
		{
			vecLostFocuses.push_back(nFocus);
		}
	}

	for each (TALENT_FOCUS_TYPE nFocus in vecLostFocuses)
	{
		LostFocus(nFocus);
	}
}

