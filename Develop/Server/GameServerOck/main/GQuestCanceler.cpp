#include "StdAfx.h"
#include "GQuestCanceler.h"
#include "GEntityPlayer.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GDBTaskQuestData.h"
#include "GGlobal.h"
#include "GQuestSystem.h"
#include "GQuestRemoveItemCollector.h"
#include "GDBManager.h"
#include "GQuestInfoMgr.h"
#include "CCommandResultTable.h"
#include "GQuestInfo.h"
#include "GPartySystem.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionsInfo.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GItemRemover.h"
#include "GPlayerObjectManager.h"


bool GQuestCanceler::Cancel(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID_RET(pPlayer, false);

	if (false == Check(pPlayer, nQuestID)) return false;

	GPlayerQuest* pQuest = pPlayer->GetQuests().Get(nQuestID);
	VALID_RET(pQuest, false);

	GDBT_QUEST_GIVEUP data;
	MakeDBTaskData(pPlayer, pQuest, nQuestID, data);

	return gsys.pDBManager->QuestGiveup(data);
}

bool GQuestCanceler::Check(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID_RET(pPlayer, false);

	GQuestInfo* pInfoQuest = gmgr.pQuestInfoMgr->Get(nQuestID);
	VRSM_FAIL(pInfoQuest, CR_FAIL_SYSTEM_INVALID_QUEST_ID);

	VRSM_FAIL(pPlayer->GetQuests().IsDoingQuest(nQuestID), CR_FAIL_QUEST_NOT_HAVE_QUEST);
	VRSM_FAIL(pInfoQuest->bCancelable, CR_FAIL_QUEST_NOT_CANCELABLE_QUEST);

	return true;
}

void GQuestCanceler::MakeDBTaskData( GEntityPlayer* pPlayer, GPlayerQuest* pQuest, int nQuestID, GDBT_QUEST_GIVEUP& data )
{
	data = GDBT_QUEST_GIVEUP(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayer->GetLevel()
		, pQuest->GetSlotID()
		, nQuestID
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pQuest->GetAcceptDt()
		, pPlayer->GetFieldID());

	gsys.pQuestSystem->GetRemoveItemCollector().CollectForCancel(pPlayer, pQuest->GetInfo(), data.m_vecQItems);
}

void GQuestCanceler::CancelForDBTask(const GDBT_QUEST_GIVEUP& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	VALID(pPlayer);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(data.m_nQuestID);
	VALID(pPlayerQuest);
	GQuestInfo* pQuestInfo = pPlayerQuest->GetInfo();
	VALID(pQuestInfo);

	pPlayer->GetQuests().Cancel(data.m_nQuestID);
	pPlayer->OnQuestDrop(data.m_nQuestID);

	if (pPlayerQuest->IsActive())
	{
		if (gsys.pPartySystem->IsPartyMember(pPlayer))
		{
			MUID uidMember = pPlayer->GetUID();
			MUID uidParty = pPlayer->GetPartyUID();
			gsys.pPartySystem->RemoveQuest(uidParty, uidMember, data.m_nQuestID);
		}

		pPlayer->OnSynchQuest();
	}

	vector<REMOVE_ITEM> vecRemoveItem;
	for each (const GITEM_STACK_AMT& removeItem in data.m_vecQItems)
	{
		vecRemoveItem.push_back(removeItem);
	}

	gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), vecRemoveItem);	
	pPlayer->UpdateUsableSensor();	

	Route(pPlayer, data.m_nQuestID);	

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);
}

void GQuestCanceler::Route(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_CANCEL,
		1,
		NEW_INT(nQuestID));

	pPlayer->RouteToMe(pNewCmd);
}