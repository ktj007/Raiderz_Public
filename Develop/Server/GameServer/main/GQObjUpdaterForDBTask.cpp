#include "stdafx.h"
#include "GQObjUpdaterForDBTask.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBTaskQuestData.h"
#include "GEntityPlayer.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GPlayerQObjective.h"
#include "GScriptManager.h"
#include "GScriptUtil.h"
#include "GScriptGroup_Quest.h"
#include "GNullGlueQuest.h"
#include "GQObjectiveInfo.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GQuestSystem.h"
#include "GQuestCompleter.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GCommand.h"

void GQObjUpdaterForDBTask::Update(const GDBT_QUEST_OBJECT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_QuestComm.m_uidPlayer);
	VALID(pPlayer);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(data.m_QuestComm.m_nQuestID);
	VALID(pPlayerQuest);
	GPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(data.m_nObjID);
	VALID(pPlayerQObjective);	

	bool bChangeToComplete = false;
	bool bChangeToIncomplete = false;
	UpdateQObjective(pPlayerQuest, data.m_nObjID, data.m_nProgress, bChangeToComplete, bChangeToIncomplete);
	Route(pPlayer, pPlayerQObjective);

	UpdateQuestComplete(pPlayer, pPlayerQuest, pPlayerQObjective->GetInfo(), data.m_bTrigger, bChangeToComplete, bChangeToIncomplete);
	UpdateQObjectiveScript(pPlayer, pPlayerQuest, pPlayerQObjective, data.m_bTrigger, data.m_uidNPC);
	UpdateUsableSensor(pPlayer);	
}

void GQObjUpdaterForDBTask::UpdateQObjective( GPlayerQuest* pPlayerQuest, int nQObejctiveID, int nProgress, bool& bChangeToComplete , bool& bChangeToIncomplete )
{
	VALID(pPlayerQuest);

	pPlayerQuest->UpdateQObjective(nQObejctiveID, nProgress, bChangeToComplete, bChangeToIncomplete);
}

void GQObjUpdaterForDBTask::UpdateQuestComplete(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GQObjectiveInfo* pQObjectiveInfo, bool bTrigger, bool bChangeToComplete, bool bChangeToIncomplete)
{
	VALID(pPlayer);
	VALID(pPlayerQuest);
	VALID(pQObjectiveInfo);

	if (bChangeToComplete)
		ChangeToQuestComplete(pPlayer, pPlayerQuest, pQObjectiveInfo, bTrigger);
	else if (bChangeToIncomplete)
		ChangeToQuestIncomplete(pPlayer, pQObjectiveInfo);
}

void GQObjUpdaterForDBTask::ChangeToQuestComplete(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GQObjectiveInfo* pQObjectiveInfo, bool bTrigger)
{
	VALID(pPlayer);
	VALID(pPlayerQuest);
	VALID(pQObjectiveInfo);

	GetScriptQuest()->OnObjectiveComplete(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), pQObjectiveInfo->nID, bTrigger);
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST_OBJECTIVE_COMPLETE);

	if (pPlayerQuest->IsAllQObjectiveComplete())
		gsys.pQuestSystem->GetCompleter().Complete(pPlayer, pQObjectiveInfo->nQuestID);	
}


void GQObjUpdaterForDBTask::ChangeToQuestIncomplete(GEntityPlayer* pPlayer, GQObjectiveInfo* pQObjectiveInfo)
{
	VALID(pPlayer);	
	VALID(pQObjectiveInfo);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST_OBJECTIVE_INCOMPLETE);

	gsys.pQuestSystem->GetCompleter().Incomplete(pPlayer, pQObjectiveInfo->nQuestID);	
}

void GQObjUpdaterForDBTask::UpdateQObjectiveScript(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GPlayerQObjective* pPlayerQObjective, bool bTrigger, MUID uidNPC)
{
	VALID(pPlayer);	
	VALID(pPlayerQuest);
	VALID(pPlayerQObjective);

	if (!IsScriptUpdatableObjectiveType(pPlayerQObjective->GetInfo()->nType))
		return;

	GetScriptQuest()->OnObjectiveUpdate(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), pPlayerQObjective->GetInfo()->nID, bTrigger, pPlayerQObjective->GetProgress());


	if (QOT_INTERACT == pPlayerQObjective->GetInfo()->nType)
	{
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
		if (NULL != pNPC)
		{
			GetScriptQuest()->OnObjectiveInteract(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), pPlayerQObjective->GetInfo()->nID, bTrigger, pPlayerQObjective->GetProgress(), pNPC->GetDelegator());
		}		
	}

	if (pPlayerQObjective->IsComplete())
	{
		GetScriptQuest()->OnObjectiveComplete(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), pPlayerQObjective->GetInfo()->nID, bTrigger);
	}
}

bool GQObjUpdaterForDBTask::IsScriptUpdatableObjectiveType(QOBJECTIVE_TYPE nType)
{
	if (QOT_DESTROY != nType 
		&& QOT_INTERACT != nType 
		&& QOT_COLLECT != nType)
		return false;

	return true;
}

void GQObjUpdaterForDBTask::UpdateUsableSensor( GEntityPlayer* pPlayer )
{
	pPlayer->UpdateUsableSensor();
}

void GQObjUpdaterForDBTask::Route(GEntityPlayer* pPlayer, GPlayerQObjective* pPlayerQObjective)
{
	TD_UPDATE_PLAYERQOBJECTIVE tdUpdatePlayerQObjective(pPlayerQObjective->GetInfo()->nQuestID
		, pPlayerQObjective->GetInfo()->nID
		, pPlayerQObjective->GetProgress()
		, pPlayerQObjective->IsComplete());

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_UPDATE_OBJECTIVE,
		1,
		NEW_SINGLE_BLOB(tdUpdatePlayerQObjective));

	pPlayer->RouteToMe(pNewCmd);
}
