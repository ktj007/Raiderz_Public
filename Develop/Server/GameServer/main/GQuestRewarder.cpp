#include "StdAfx.h"
#include "GQuestRewarder.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GQuestInfo.h"
#include "CCommandResultTable.h"
#include "GDBTaskQuestData.h"
#include "GPlayerInteraction.h"
#include "GDBManager.h"
#include "GItemAdder.h"
#include "GQuestSystem.h"
#include "GQuestRemoveItemCollector.h"
#include "GQuestAddItemCollector.h"
#include "GExpCalculator.h"


bool GQuestRewarder::Reward(GEntityPlayer* pPlayer, int nQuestID, int nSelectedReward)
{
	VALID_RET(pPlayer, false);	
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo, false);

	QUEST_REWARD selectedReward;
	if (false == Check(pPlayer, pQuestInfo, nSelectedReward, selectedReward)) return false;	

	GDBT_QEUST_DONE data;
	if (false == MakeDBTaskData(pPlayer, pQuestInfo, selectedReward, data)) return false;

	return gsys.pDBManager->QuestDone(data);
}

bool GQuestRewarder::RewardForGM(GEntityPlayer* pPlayer, int nQuestID )
{
	VALID_RET(pPlayer, false);

	return Reward(pPlayer, nQuestID, 0);
}

bool GQuestRewarder::Check(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, int nSelectedReward, QUEST_REWARD& selectedReward)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pQuestInfo, false);

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
	if (NULL == pPlayerQuest) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_HAVE_QUEST);

	if (pPlayerQuest->IsFail()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_IS_FAILED);

	if (false == pPlayerQuest->IsComplete()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_COMPLETE_QUEST);

	if (false == pQuestInfo->vecSelectableReward.empty())
	{
		if (0 > nSelectedReward) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_SELECTABLE_REWARD);
		if (static_cast<int>(pQuestInfo->vecSelectableReward.size()) <= nSelectedReward) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_SELECTABLE_REWARD);

		selectedReward = pQuestInfo->vecSelectableReward[nSelectedReward];
	}

	if (false == CheckItemAddable(pPlayer, pQuestInfo, selectedReward)) return false;

	return true;
}

bool GQuestRewarder::CheckItemAddable( GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, const QUEST_REWARD& selectedReward)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pQuestInfo, false);

	vector<CHECK_ADDABLE_ITEM> vecCheckAddbaleItem;
	for each (const QUEST_REWARD& questReward in pQuestInfo->vecReward)
	{
		if (QRT_ITEM == questReward.nType)
		{
			vecCheckAddbaleItem.push_back(CHECK_ADDABLE_ITEM(questReward.nParam1, questReward.nParam2));
		}		
	}

	if (QRT_ITEM == selectedReward.nType)
	{
		vecCheckAddbaleItem.push_back(CHECK_ADDABLE_ITEM(selectedReward.nParam1, selectedReward.nParam2));
	}	

	return gsys.pItemSystem->GetAdder().CheckItemAddable(pPlayer, vecCheckAddbaleItem);
}

bool GQuestRewarder::MakeDBTaskData( GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, const QUEST_REWARD& selectedReward, GDBT_QEUST_DONE& data )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pQuestInfo, false);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
	VALID_RET(pPlayerQuest, false);

	vector<GDBT_QUEST_ADDITEM> vecAddItem;
	gsys.pQuestSystem->GetAddItemCollector().CollectForReward(pPlayer, pQuestInfo, selectedReward, vecAddItem);

	vector<GITEM_STACK_AMT> vecRemoveItem;
	if (!gsys.pQuestSystem->GetRemoveItemCollector().CollectForReward(pPlayer, pQuestInfo, vecRemoveItem))
		return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_HAVE_REWARD_NEED_ITEM);

	gsys.pQuestSystem->GetAddItemCollector().RemoveDuplicateCollection(vecAddItem, vecRemoveItem);

	int nMoney = 0;
	int nXP		= 0;
	CalcRewardMoneyAndXP(pPlayer, pQuestInfo, selectedReward, nMoney, nXP);

	int nNewLevel = GExpCalculator::CalcNewLevel(pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP+nXP);
	int nNewExp = GExpCalculator::CalcNewExp(pPlayer->GetLevel(), pPlayer->GetPlayerInfo()->nXP+nXP);

	data = GDBT_QEUST_DONE (pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, nNewLevel
		, nNewLevel - pPlayer->GetLevel()
		, pPlayerQuest->GetSlotID()
		, pQuestInfo->nID
		, selectedReward
		, nNewExp
		, nXP
		, pPlayer->GetMoney() + nMoney
		, nMoney
		, pPlayerQuest->GetAcceptDt()
		, pPlayerQuest->GetExpiDt()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetInteraction().GetInteractingNPCUID()
		, pPlayer->GetFieldID()
		, pPlayer->GetPos());

	data.m_vecAddItem.swap(vecAddItem);
	data.m_vecRemoveItem.swap(vecRemoveItem);

	return true;
}

void GQuestRewarder::CalcRewardMoneyAndXP(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, const QUEST_REWARD& selectedReward, int& nMoney, int& nXP)
{
	VALID(pPlayer);
	VALID(pQuestInfo);

	nMoney = 0;
	nXP = 0;

	for each (const QUEST_REWARD& rw in pQuestInfo->vecReward)
	{
		if (QRT_MONEY == rw.nType)
		{
			nMoney += rw.nParam1;
		}
		else if (QRT_XP == rw.nType)
		{
			nXP += rw.nParam1;
		}	
	}

	if (selectedReward.nType == QRT_MONEY)
	{
		nMoney += selectedReward.nParam1;
	}

	if (selectedReward.nType == QRT_XP)
	{
		nXP += selectedReward.nParam1;
	}

	if (MAX_MONEY - nMoney < pPlayer->GetMoney())
	{
		nMoney = MAX_MONEY - pPlayer->GetMoney();
	}
}

