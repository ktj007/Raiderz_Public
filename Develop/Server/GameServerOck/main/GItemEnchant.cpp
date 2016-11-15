#include "StdAfx.h"
#include "GItemEnchant.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GGlobal.h"
#include "GBuffInfo.h"
#include "GCommandTable.h"
#include "GCommand.h"
#include "GDBTaskItemData.h"
#include "GDBManager.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"
#include "GConst.h"
#include "GItemActableChecker.h"
#include "GPlayerDoing.h"


static const int FAILED = -1;

GItemEnchant::GItemEnchant(void)
{
}

GItemEnchant::~GItemEnchant(void)
{
}

int GItemEnchant::EnchantCheck( GEntityPlayer* pPlayer, GItem* pEStone, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, GItem* pEAgent )
{
	VALID_RET(pPlayer, CR_FAIL);
	VALID_RET(pEStone, CR_FAIL);
	VALID_RET(pEStone->m_pItemData, CR_FAIL);
	VALID_RET(pEAgent, CR_FAIL);

	if (!pEStone->m_pItemData->IsEnchantStone())
		return false; // 강화석이 아님

	// 목표 슬롯의 아이템 얻기
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pTargetItem = pItemHolder->GetItem(nTargetSlotType, nTargetSlotID);
	VALID_RET(pTargetItem, CR_FAIL);
	if (!CheckLimitLevel(pPlayer, pEStone))
	{
		return CR_FAIL_ENCHANT_REQ_NOTENOUGH_LEVEL; // 강화실패: 레벨부족
	}

	if (!CheckLimitEquipments(pTargetItem, pEStone))
	{
		return CR_FAIL_ENCHANT_REQ_UNABLE_EQUIP_PART; // 강화실패: 강화할 수 없는 장비
	}

	if (!CheckLimitWeapon(pTargetItem, pEStone))
	{
		return CR_FAIL_ENCHANT_REQ_UNABLE_WEAPON; // 강화실패: 강화할 수 없는 무기
	}

	if (!CheckAgent(pEStone, pEAgent))
	{
		return CR_FAIL_ENCHANT_REQ_WRONG_AGENT; // 강화실패: 알맞지 않은 강화제
	}

	// 현재 다른 기능에서 처리중인 아이템은 첨부 못한다.
	if (!gsys.pItemSystem->GetActableChecker().IsEnchantable(pPlayer, pTargetItem))
	{
		return CR_FAIL_ENCHANT_REQ_ITEM_BUSY; // 강화실패: 현재 다른 기능에서 사용 중인 아이템은 첨부할 수 없습니다.
	}

	int nBuffID = pEStone->m_pItemData->m_EnchantStone.nLinkedBuffID;
	if (nBuffID == INVALID_BUFF_ID)
		return CR_FAIL; // 강화버프 아님

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	VALID_RET(pEAgent, CR_FAIL);
	VALID_RET(pBuffInfo, CR_FAIL);

	if (!pBuffInfo->IsEnchant())
		return CR_FAIL; // 강화버프 아님

	if (!pTargetItem->IsEnchantable(pBuffInfo))
		return CR_FAIL_ENCHANT_REQ_NOTENOUGH_SLOT; // 강화할 수 없음

	// 목표 슬롯의 아이템 얻기
	ENCHANT_COLOR nEnchantColor = pEStone->m_pItemData->m_EnchantStone.nColor;
	if (-1 == pTargetItem->GetEmptyEnchantSlotIdx(pBuffInfo->HasProcEffect(), nEnchantColor))
		return CR_FAIL_ENCHANT_REQ_NOTENOUGH_SLOT; // 강화할 수 없음

	return CR_SUCCESS;
}

int GItemEnchant::GetEmptyEnchantSlotIdx(GItem* pEStone, GItem* pTargetItem, GItem* pEAgent)
{
	int nBuffID = pEStone->m_pItemData->m_EnchantStone.nLinkedBuffID;
	if (nBuffID == INVALID_BUFF_ID)
		return CR_FAIL; // 강화버프 아님

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	VALID_RET(pEAgent, CR_FAIL);
	VALID_RET(pBuffInfo, CR_FAIL);

	if (!pBuffInfo->IsEnchant())
		return CR_FAIL; // 강화버프 아님

	if (!pTargetItem->IsEnchantable(pBuffInfo))
		return CR_FAIL_ENCHANT_REQ_NOTENOUGH_SLOT; // 강화할 수 없음

	// 목표 슬롯의 아이템 얻기
	ENCHANT_COLOR nEnchantColor = pEStone->m_pItemData->m_EnchantStone.nColor;
	return pTargetItem->GetEmptyEnchantSlotIdx(pBuffInfo->HasProcEffect(), nEnchantColor);
}

bool GItemEnchant::Enchant( GEntityPlayer* pPlayer, int nEnchantSlotID, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nAgentSlotID )
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pEStone = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nEnchantSlotID);
	VALID_RET(pEStone, false);
	GItem* pEAgent = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nAgentSlotID);
	VALID_RET(pEAgent, false);
	return Enchant(pPlayer, pEStone, nTargetSlotType, nTargetSlotID, pEAgent);
}

bool GItemEnchant::Enchant( GEntityPlayer* pPlayer, GItem* pEStone, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, GItem* pEAgent )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pEStone, false);
	VALID_RET(pEStone->m_pItemData, false);
	VALID_RET(pEAgent, false);

	// 목표 슬롯의 아이템 얻기
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pTargetItem = pItemHolder->GetItem(nTargetSlotType, nTargetSlotID);
	VALID_RET(pTargetItem, false);

	int nEchantCheckResult = 
		EnchantCheck(pPlayer, pEStone, nTargetSlotType, nTargetSlotID, pEAgent);
	if (nEchantCheckResult != CR_SUCCESS)
	{
		pPlayer->FailAndRouteSystemMsg((CCommandResultTable)nEchantCheckResult);
		return false;
	}

	if (!CheckChance(pEStone, pTargetItem))	
	{
		EnchantBroken(pPlayer, pEStone, pEAgent, pTargetItem);
		return false; // 강화 확률실패
	}

	return EnchantToDB(pPlayer, pEStone, pEAgent, pTargetItem, false);
}

int GItemEnchant::EnchantBuff( GEntityPlayer* pPlayer, GItem* pEStone, GItem* pTargetItem, ENCHANT_COLOR nEnchantColor )
{
	VALID_RET(pTargetItem, FAILED);
	VALID_RET(pTargetItem->m_pItemData,FAILED);
	VALID_RET(pEStone, FAILED);
	VALID_RET(pEStone->m_pItemData,FAILED);
	
	// 버프 유효성 검사
	int nItemID = pEStone->m_pItemData->m_nID;
	int nBuffID = pEStone->m_pItemData->m_EnchantStone.nLinkedBuffID;
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	VALID_RET(pBuffInfo, FAILED);
	if (!pBuffInfo->IsEnchant())
		return FAILED; // 강화버프 아님

	if (!pTargetItem->IsEnchantable(pBuffInfo))
		return FAILED; // 강화할 수 없음

	return pTargetItem->Enchant(pBuffInfo, nItemID, nEnchantColor);
}

bool GItemEnchant::CheckChance( GItem* pEStone, GItem* pTargetItem ) const
{
	VALID_RET(pEStone, false);
	VALID_RET(pTargetItem, false);

	float fSuccessRate = 
		CalcEnchantChanceRate(pEStone, pTargetItem);

	GPercentDice dice;
	if (!dice.Dice(fSuccessRate))	
		return false; // 실패	

	return true;
}

bool GItemEnchant::CheckLimitLevel(GEntityPlayer* pPlayer, GItem* pEStone)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pEStone, false);
	VALID_RET(pEStone->m_pItemData, false);

	if (pEStone->m_pItemData->m_EnchantStone.nLimitLevel > pPlayer->GetLevel())
		return false; // 레벨 부족

	return true;
}

bool GItemEnchant::CheckLimitEquipments(GItem* pTargetItem, GItem* pEStone)
{
	VALID_RET(pTargetItem, false);
	VALID_RET(pTargetItem->m_pItemData, false);
	VALID_RET(pEStone, false);
	VALID_RET(pEStone->m_pItemData, false);

	if (pEStone->m_pItemData->m_EnchantStone.bAllowAllEquipments)
		return true; // 장착 가능

	for each (SH_ITEM_SLOT nLimitSlot in pEStone->m_pItemData->m_EnchantStone.vecLimitEquipments)
	{
		if (nLimitSlot == pTargetItem->m_pItemData->m_nItemSlot)
			return false;

		if (nLimitSlot == pTargetItem->m_pItemData->m_nSubSlot)
			return false;
	}

	return true;
}

bool GItemEnchant::CheckLimitWeapon(GItem* pTargetItem, GItem* pEStone)
{
	VALID_RET(pTargetItem, false);
	VALID_RET(pTargetItem->m_pItemData, false);
	VALID_RET(pEStone, false);
	VALID_RET(pEStone->m_pItemData, false);

	if (pEStone->m_pItemData->m_EnchantStone.bAllowAllWeapons)
		return true; // 장착 가능

	for each (WEAPON_TYPE nLimitSlot in pEStone->m_pItemData->m_EnchantStone.vecLimitWeapons)
	{
		if (nLimitSlot == pTargetItem->m_pItemData->m_nWeaponType)
			return false;
	}

	return true;
}

bool GItemEnchant::CheckAgent(GItem* pEStone, GItem* pEAgent)
{
	VALID_RET(pEStone, false);
	VALID_RET(pEStone->m_pItemData, false);
	VALID_RET(pEAgent, false);
	VALID_RET(pEAgent->m_pItemData, false);

	int nTier = pEStone->m_pItemData->m_EnchantStone.nTier;
	if (nTier <= 0)
		return true; // 티어가 지정되어 있지 않음


	const int kMaxTier = MAX_LEVEL/10;
	for (int nTargetTier=nTier; nTargetTier<=kMaxTier; ++nTargetTier)
	{
		int nAgentItemID = GetAgentItemID(nTargetTier);
		if (pEAgent->m_pItemData->m_nID == nAgentItemID)
			return true; // 적절한 강화제
	}
	

	return false;
}

float GItemEnchant::CalcEnchantChanceRate( GItem* pEStone, GItem* pTargetItem ) const
{
	VALID_RET(pEStone, 0.0f);
	VALID_RET(pTargetItem, 0.0f);
	VALID_RET(pEStone->m_pItemData, 0.0f);

	// 강화석 고유확률
	float fSuccessRate = 
		pEStone->m_pItemData->m_EnchantStone.fSuccessRate;

	// 강화횟수 보정
	fSuccessRate -= (max((pTargetItem->GetEnchantCount()-1), 0) * 5.00f);
	return min(fSuccessRate, 100.0f);
}

void GItemEnchant::EnchantBroken(GEntityPlayer* pPlayer, GItem* pEStone, GItem* pEAgent, GItem* pTargetItem)
{
	VALID(pPlayer);
	VALID(pEStone);
	VALID(pEStone->m_pItemData);
	VALID(pTargetItem);

	int nBuffID = pEStone->m_pItemData->m_EnchantStone.nLinkedBuffID;
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	VALID(pBuffInfo);
	if (!pBuffInfo->IsEnchant())
		return; // 강화버프 아님

	if (!pTargetItem->IsEnchantable(pBuffInfo))
		return; // 강화할 수 없음

	EnchantToDB(pPlayer, pEStone, pEAgent, pTargetItem, true);
}

bool GItemEnchant::EnchantToDB( GEntityPlayer* pPlayer, GItem* pEStone, GItem* pEAgent, GItem* pTargetItem, bool bBroken )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pEStone, false);
	VALID_RET(pEStone->m_pItemData, false);
	VALID_RET(pEAgent, false);
	VALID_RET(pTargetItem, false);

	int8 nEnchantSlotID = 
		GetEmptyEnchantSlotIdx(pEStone, pTargetItem, pEAgent);

	GDBT_ITEM_ENCH data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetMoney()
		, pEStone->m_nSlotID
		, pEStone->m_nIUID
		, pEStone->m_pItemData->m_nID
		, pEAgent->m_nSlotID
		, pEAgent->m_nIUID
		, pEAgent->m_pItemData->m_nID
		, pEAgent->GetAmount() -1
		, -1
		, pTargetItem->m_nSlotType
		, pTargetItem->m_nSlotID
		, pTargetItem->m_nIUID		
		, pTargetItem->m_pItemData->m_nID
		, pTargetItem->m_nDurability
		, pTargetItem->m_nDurability
		, nEnchantSlotID
		, bBroken?ENCHANT_BROKEN_SLOT:pEStone->m_pItemData->m_nID
		, true);

	return gsys.pDBManager->ItemEnchant(data);
}

int GItemEnchant::GetAgentItemID( int nTier )
{
	if (nTier <= 0)
		return INVALID_ID;

	if (MAX_LEVEL/10 <= nTier-1)
		return INVALID_ID;
	
	return ENCHANT_AGENT_ITEMID[nTier-1];
}


void GItemEnchant::RouteEnchantCheckResult(GEntityPlayer* pPlayer, int nErrorCode)
{
	VALID(pPlayer);

	MCommand* pNewCommand = MakeNewCommand(MC_ENCHANT_CHECK_RESULT,
		1, NEW_INT(nErrorCode));
	pPlayer->RouteToMe(pNewCommand);
}

void GItemEnchant::EnchantPrepare( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	// 플레이어 강화 상태로 변경
	pPlayer->GetDoing().ChangeDoing(CD_ENCHANTING);	
}

void GItemEnchant::EnchantCancel( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	// 플레이어 강화 상태로 변경
	pPlayer->GetDoing().ChangeDoing(CD_NORMAL);	
}
