#include "StdAfx.h"
#include "GQuestGiver.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GPlayerQuests.h"
#include "GQuestDateCalculator.h"
#include "GQuestInfo.h"
#include "GDBTaskQuestData.h"
#include "GDBManager.h"
#include "GPlayerQuest.h"
#include "GItemAddAndRemoverForInfo.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GQuestSystem.h"
#include "GPlayerQObjective.h"
#include "GQObjectiveInfo.h"
#include "GItemHolder.h"
#include "GPlayerInteraction.h"
#include "GScriptManager.h"
#include "GScriptGroup_Quest.h"
#include "GScriptUtil.h"
#include "GEntityNPC.h"
#include "GPlayerNPCIconSender.h"
#include "GNPCSensorChecker.h"
#include "GConditionsSystem.h"
#include "GItemAddRemoveInfo.h"
#include "GChallengerQuestMgr.h"
#include "CSConditionInfo.h"
#include "GNullGlueNPC.h"
#include "GNullGlueQuest.h"
#include "GQuestCompleter.h"
#include "GQuestSharer.h"
#include "GQObjUpdater.h"
#include "GQuestRemoveItemCollector.h"
#include "GQuestAddItemCollector.h"
#include "GItemRemover.h"
#include "GItemAdder.h"


bool GQuestGiver::Give(GEntityPlayer* pPlayer, int nQuestID, bool bForce)
{
	VALID_RET(pPlayer, false);
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo, false);	

	if (false == Check(pPlayer, nQuestID, pQuestInfo, bForce)) return false;

	GDBT_QUEST_ACCEPT data;
	if (false == MakeDBTaskData(pPlayer, pQuestInfo, data)) return false;	

	return gsys.pDBManager->QuestAccept(data);
}

void GQuestGiver::GiveForGM(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID(pPlayer);

	Give(pPlayer, nQuestID, true);
}


bool GQuestGiver::Check(GEntityPlayer* pPlayer, int nQuestID, const GQuestInfo* pQuestInfo, bool bForce)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pQuestInfo, false);

	if (true == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_TAKABLE_QUEST);
	if (NULL != pQuestInfo->pItemAddRemoveInfo)
	{
		if (false == gsys.pItemSystem->GetAddAndRemoverForInfo().Check(pPlayer, pQuestInfo->pItemAddRemoveInfo->GetItemAddInfo())) return false;
	}

	if (true == bForce) return true;

	if (pQuestInfo->bChallengerQuest)
	{ 
		if (!pQuestInfo->CheckRequireLevel_ForChallengerQuest(pPlayer->GetLevel())) return false;
		if (!gsys.pConditionsSystem->Check(pPlayer, pQuestInfo->nRequiredConditionIDForChallengerQuest)) return false;
	}	

	if (false == pQuestInfo->bRepeatable)
	{
		if (true == pPlayer->GetQuests().IsDoneQuest(nQuestID)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_QUEST_NOT_REPEATABLE_QUEST);
	}	

	if (pQuestInfo->bChallengerQuest)
	{
		if (!gmgr.pChallengerQuestMgr->IsTodayQuest(pQuestInfo))	
			return false;
	}

	return true;
}

bool GQuestGiver::CheckForChallengerQuest( GEntityPlayer* pPlayer, int nQuestID, const GQuestInfo* pQuestInfo )
{
	VALID_RET(pPlayer, false);	

	return Check(pPlayer, nQuestID, pQuestInfo);
}


bool GQuestGiver::MakeDBTaskData( GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, GDBT_QUEST_ACCEPT& data )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pQuestInfo, false);

	const uint8 nSlotID = pPlayer->GetQuests().FindAndMarkingEmptySlot();
	if (INVALID_QUSET_SLOT_ID == nSlotID) return false;

	vector<GDBT_QUEST_ADDITEM> vecAddItem;
	gsys.pQuestSystem->GetAddItemCollector().CollectForGive(pPlayer, pQuestInfo, vecAddItem);
	vector<GITEM_STACK_AMT> vecRemoveItem;
	gsys.pQuestSystem->GetRemoveItemCollector().CollectForGive(pPlayer, pQuestInfo, vecRemoveItem);	

	gsys.pQuestSystem->GetAddItemCollector().RemoveDuplicateCollection(vecAddItem, vecRemoveItem);

	wstring strAcceptDt;
	wstring strExpiDt;

	GQuestDateCalculator::GetAccpetDt(strAcceptDt);
	GQuestDateCalculator::GetExpiDt(pQuestInfo->bChallengerQuest, strExpiDt);

	data = GDBT_QUEST_ACCEPT(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, nSlotID
		, pQuestInfo->nID
		, pQuestInfo->bChallengerQuest
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetPlayerInfo()->nLevel
		, pPlayer->GetPlayerInfo()->GetMoney()
		, pQuestInfo->IsEmptyQObjective()
		, strAcceptDt
		, strExpiDt
		, pPlayer->GetFieldID()
		, pPlayer->GetInteraction().GetInteractingNPCUID());

	data.m_vecAddItem.swap(vecAddItem);
	data.m_vecRemoveItem.swap(vecRemoveItem);	

	return true;
}

void GQuestGiver::GiveForDBTask(GDBT_QUEST_ACCEPT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_QuestComm.m_uidPlayer);
	VALID(pPlayer);

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Insert(data.m_QuestComm.m_nSlotID, data.m_QuestComm.m_nQuestID, data.m_QuestComm.m_strAcceptDt, data.m_strExpiDt);
	VALID(pPlayerQuest);
	GQuestInfo* pQuestInfo = pPlayerQuest->GetInfo();
	VALID(pQuestInfo);	

	Route(pPlayer, data.m_QuestComm.m_nQuestID);
	RefreshNPCIcon(pPlayer);
	Script(pPlayer, pPlayerQuest, data.m_uidNPC);	
	UpdateQObject(pPlayer, pQuestInfo);
	AddRemoveItem(pPlayer, data);	
	ShareQuest(pPlayer, pQuestInfo);
	UpdateUsableSensor(pPlayer);
}

void GQuestGiver::Route(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID(pPlayer);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	VALID(pPlayerQuest);
	GQuestInfo* pQuestInfo = pPlayerQuest->GetInfo();
	VALID(pQuestInfo);

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_GIVE, 
		5, 		
		NEW_INT(nQuestID),
		NEW_UINT(pPlayerQuest->GetAcceptTime()),
		NEW_UINT(pPlayerQuest->GetLimitTime()), 
		NEW_VEC(pQuestInfo->vRewardPos), 
		NEW_BOOL(pPlayerQuest->GetInfo()->bChallengerQuest)
		);

	pPlayer->RouteToMe(pNewCmd);
}

void GQuestGiver::RefreshNPCIcon(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);
}

void GQuestGiver::Script( GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, MUID uidNPC )
{
	VALID(pPlayer);
	VALID(pPlayerQuest);	

	GEntityNPC* pNPC = pPlayer->FindNPC(uidNPC);
	GetScriptQuest()->OnBegin(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), TO_SAFE_LUAPTR(pNPC));
}

void GQuestGiver::UpdateQObject(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo)
{
	VALID(pPlayer);
	VALID(pQuestInfo);

	if (true == pQuestInfo->IsEmptyQObjective())
	{
		gsys.pQuestSystem->GetCompleter().Complete(pPlayer, pQuestInfo->nID);
	}
	else
	{
		GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQuestInfo->nID);
		if (NULL == pPlayerQuest) return;

		MAP_PLAYERQOBJECTIVE& mapPlayerQObjective = pPlayerQuest->GetQObjectiveContainer();
		for each (pair<int, GPlayerQObjective*> val in mapPlayerQObjective)
		{
			GPlayerQObjective* pPlayerQObjective = val.second;
			if (NULL == pPlayerQObjective) continue;
			GQObjectiveInfo* pQObjectiveInfo = pPlayerQObjective->GetInfo();
			if (NULL == pQObjectiveInfo) continue;

			if (QOT_COLLECT == pQObjectiveInfo->nType)
			{
				GItemHolder* pItemHolder = pPlayer->GetItemHolder();
				int nQuantity = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, pQObjectiveInfo->vecParam1[0]);
				if (nQuantity <= pPlayerQObjective->GetProgress()) continue;

				gsys.pQuestSystem->GetQObjUpdater().IncCollectQObj(pPlayer, pQObjectiveInfo->vecParam1[0], nQuantity - pPlayerQObjective->GetProgress());
			}
		}
	}	
}

void GQuestGiver::AddRemoveItem( GEntityPlayer* pPlayer, GDBT_QUEST_ACCEPT& data)
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

	vector<REMOVE_ITEM> vecRemoveItem;
	for each (const GITEM_STACK_AMT& removeItem in data.m_vecRemoveItem)
	{
		vecRemoveItem.push_back(removeItem);
	}

	gsys.pItemSystem->GetAdder().AddForDBTask(pPlayer->GetUID(), vecAddItem);
	gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), vecRemoveItem);
}

void GQuestGiver::ShareQuest( GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo)
{
	VALID(pPlayer);	
	VALID(pQuestInfo);	

	if (QSHAT_AUTO == pQuestInfo->nShareType)
	{
		gsys.pQuestSystem->GetSharer().Share(pPlayer, pQuestInfo->nID);
	}	
}

void GQuestGiver::UpdateUsableSensor( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	pPlayer->UpdateUsableSensor();
}