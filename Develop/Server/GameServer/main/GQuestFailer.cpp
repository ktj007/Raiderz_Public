#include "StdAfx.h"
#include "GQuestFailer.h"
#include "GEntityPlayer.h"
#include "GDBTaskQuestData.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GPlayerObject.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"
#include "GPartySystem.h"
#include "GScriptManager.h"
#include "GScriptGroup_Quest.h"
#include "GScriptUtil.h"
#include "GNullGlueQuest.h"
#include "GPlayerNPCIconSender.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "CSConditionsInfo.h"


void GQuestFailer::Fail(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID(pPlayer);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	VALID(pPlayerQuest);
	
	const GDBT_QUEST_COMMON data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayerQuest->GetSlotID()
		, nQuestID
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetPlayerInfo()->nLevel
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayerQuest->GetAcceptDt()
		, pPlayer->GetFieldID());
	
	gsys.pDBManager->QuestFail(data);
}

void GQuestFailer::FailForDBTask(const GDBT_QUEST_COMMON& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	VALID(pPlayer);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(data.m_nQuestID);
	VALID(pPlayerQuest);

	pPlayerQuest->Fail();

	if (gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		MUID uidParty = pPlayer->GetPartyUID();
		MUID uidMember = pPlayer->GetUID();

		gsys.pPartySystem->RemoveQuest(uidParty, uidMember, data.m_nQuestID);
	}

	pPlayer->OnSynchQuest();

	GetScriptQuest()->OnFail(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator());
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);

	Route(pPlayer, data.m_nQuestID);
}

void GQuestFailer::Route(GEntityPlayer* pPlayer, int nQuestID)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_FAIL, 
		1, 
		NEW_INT(nQuestID)
		);

	pPlayer->RouteToMe(pNewCmd);
}