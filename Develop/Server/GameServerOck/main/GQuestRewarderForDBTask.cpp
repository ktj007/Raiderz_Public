#include "stdafx.h"
#include "GQuestRewarderForDBTask.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBTaskQuestData.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GEntityPlayer.h"
#include "GItemAdder.h"
#include "GPartySystem.h"
#include "GItemRemover.h"
#include "GFactionSystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GPlayerInteraction.h"
#include "GScriptManager.h"
#include "GScriptGroup_Quest.h"
#include "GScriptUtil.h"
#include "GNullGlueNPC.h"
#include "GNullGlueQuest.h"
#include "GEntityNPC.h"
#include "GMoneySystem.h"
#include "GExpSystem.h"
#include "GPlayerLevelUpdater.h"

bool GQuestRewarderForDBTask::Reward(GDBT_QEUST_DONE& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_QuestComm.m_uidPlayer);
	VALID_RET(pPlayer, false);
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(data.m_QuestComm.m_nQuestID);
	VALID_RET(pQuestInfo, false);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(data.m_QuestComm.m_nQuestID);
	VALID_RET(pPlayerQuest, false);

	UpdatePlayerUsableSensor(pPlayer);
	SynchParty(pPlayer, pPlayerQuest);
	DoneQuest(pPlayer, pQuestInfo->nID);
	Route(pPlayer, data.m_nNPCUID, pQuestInfo->nID);
	RefreshNPCIcon(pPlayer);

	AddItem(pPlayer, data);
	RemoveItem(pPlayer, data);
	RewardFaction(pPlayer, pQuestInfo->vecReward, data.m_SelectedReward);
	RewardMoney(pPlayer, data.m_nIncMoney);
	RewardXP(pPlayer, data.m_QuestComm.m_nXP, data.m_nIncXP, data.m_QuestComm.m_nLevel, data.m_nDeltaLevel);
	EndInteraction(pPlayer);	

	// Script 콜백에서 GPlayerQuest를 사용하고 있어서, 콜백이 끝난후 지운다.
	Script(pPlayer, pPlayerQuest, data.m_nNPCUID);
	DeleteQuest(pPlayerQuest);

	return true;
}

void GQuestRewarderForDBTask::UpdatePlayerUsableSensor(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	pPlayer->UpdateUsableSensor();
}

void GQuestRewarderForDBTask::SynchParty(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest)
{
	VALID(pPlayer);
	VALID(pPlayerQuest);

	if (false == pPlayerQuest->IsActive()) return;

	if (gsys.pPartySystem->IsPartyMember(pPlayer))
	{			
		MUID uidParty = pPlayer->GetPartyUID();
		MUID uidMember = pPlayer->GetUID();
		int nQuestID = pPlayerQuest->GetInfo()->nID;

		gsys.pPartySystem->RemoveQuest(uidParty, uidMember, nQuestID);
	}

	pPlayer->OnSynchQuest();
}

void GQuestRewarderForDBTask::DoneQuest(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID(pPlayer);

	pPlayer->GetQuests().Done(nQuestID);
}

void GQuestRewarderForDBTask::AddItem(GEntityPlayer* pPlayer, GDBT_QEUST_DONE& data)
{
	VALID(pPlayer);	
	vector<ADD_ITEM> vecAddItem;
	for each (const GDBT_QUEST_ADDITEM& addItem in data.m_vecAddItem)
	{
		int64 nIUID = 0;
		map<int,int64>::iterator itor = data.m_mapIUID.find(addItem.m_nSlotID);
		if (data.m_mapIUID.end() != itor)
		{
			nIUID = itor->second;
		}		

		vecAddItem.push_back(ADD_ITEM(SLOTTYPE_INVENTORY, addItem.m_nSlotID, addItem.m_nStackAmt, addItem.m_nModStackAmt, nIUID, addItem.m_nItemID));
	}
	gsys.pItemSystem->GetAdder().AddForDBTask(pPlayer->GetUID(), vecAddItem);	
}

void GQuestRewarderForDBTask::RemoveItem( GEntityPlayer* pPlayer, GDBT_QEUST_DONE &data )
{
	vector<REMOVE_ITEM> vecRemoveItem;
	for each (const GITEM_STACK_AMT& itemStackAmt in data.m_vecRemoveItem)
	{
		vecRemoveItem.push_back(REMOVE_ITEM(itemStackAmt));
	}

	gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), vecRemoveItem);
}


void GQuestRewarderForDBTask::RewardFaction(GEntityPlayer* pPlayer, const vector<QUEST_REWARD>& vecQuestReward, QUEST_REWARD selectedReward)
{
	VALID(pPlayer);

	for each (const QUEST_REWARD& questReward in vecQuestReward)
	{
		if (QRT_FACTION != questReward.nType) continue;

		if (0 < questReward.nParam2)
		{
			gsys.pFactionSystem->Increase(pPlayer, questReward.nParam1, questReward.nParam2);
		}
		else if (0 > questReward.nParam2)
		{
			gsys.pFactionSystem->Decrease(pPlayer, questReward.nParam1, -questReward.nParam2);
		}			
	}

	if (QRT_FACTION == selectedReward.nType)
	{
		if (0 < selectedReward.nParam2)
		{
			gsys.pFactionSystem->Increase(pPlayer, selectedReward.nParam1, selectedReward.nParam2);
		}
		else if (0 > selectedReward.nParam2)
		{
			gsys.pFactionSystem->Decrease(pPlayer, selectedReward.nParam1, -selectedReward.nParam2);
		}			
	}	
}

void GQuestRewarderForDBTask::RewardMoney( GEntityPlayer* pPlayer, int nMoney )
{
	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, nMoney);
}

void GQuestRewarderForDBTask::RewardXP( GEntityPlayer* pPlayer, int nXP, int nDeltaXP, int nLevel, int nDeltaLevel )
{
	if (0 < nDeltaLevel)
	{
		GPlayerLevelUpdater levelUpater = GPlayerLevelUpdater(pPlayer);
		levelUpater.LevelUpForDBTask(nLevel, nXP, nDeltaXP);
	}
	else
	{
		gsys.pExpSystem->AddExpForDB(pPlayer, nDeltaXP);	
	}	
}


void GQuestRewarderForDBTask::Route(GEntityPlayer* pPlayer, MUID uidNPC, int nQuestID)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_REWARD,
		2,
		NEW_UID(uidNPC),
		NEW_INT(nQuestID));

	pPlayer->RouteToMe(pNewCmd);
}

void GQuestRewarderForDBTask::RefreshNPCIcon(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);
}

void GQuestRewarderForDBTask::EndInteraction( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
}

void GQuestRewarderForDBTask::Script(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, MUID uidNPC)
{
	VALID(pPlayer);
	VALID(pPlayerQuest);

	GEntityNPC* pNPC = pPlayer->FindNPC(uidNPC);
	GetScriptQuest()->OnEnd(TO_SAFE_LUAPTR(pPlayerQuest)
		, pPlayer->GetDelegator()
		, TO_SAFE_LUAPTR(pNPC));
}

void GQuestRewarderForDBTask::DeleteQuest(GPlayerQuest* pPlayerQuest)
{
	SAFE_DELETE(pPlayerQuest);
}