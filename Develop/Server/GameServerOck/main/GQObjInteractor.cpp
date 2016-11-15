#include "StdAfx.h"
#include "GQObjInteractor.h"
#include "GEntityPlayer.h"
#include "GPlayerInteraction.h"
#include "GPlayerQuests.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"
#include "GTestSystem.h"

bool GQObjInteractor::Prepare(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nQuestID)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return false;	

	pPlayer->GetInteraction().BeginInteractQObjectvie(GConst::INTERACT_QUEST_OBJECTIVE_ANI_TIME);

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_INTERACT_QOBJECTIVE_BEGIN, 
		2, 
		NEW_UID(pPlayer->GetUID()),
		NEW_UID(pNPC->GetUID()));
	pPlayer->RouteToThisCell(pNewCmd);

	return true;
}

bool GQObjInteractor::Cancel( GEntityPlayer* pPlayer, int nQuestID )
{
	VALID_RET(pPlayer, false);

	if (false == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return false;

	pPlayer->GetInteraction().CancelInteractQObjectvie();

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_INTERACT_QOBJECTIVE_END, 
		1, 
		NEW_UID(pPlayer->GetUID()));
	pPlayer->RouteToThisCell(pNewCmd);

	return true;
}

bool GQObjInteractor::Act(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nQuestID, const vector<int>& vecQObjeciveID)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == pPlayer->GetQuests().IsDoingQuest(nQuestID)) return false;	

	pPlayer->GetInteraction().ConsumeIUseItem();
	gsys.pQuestSystem->GetQObjUpdater().UpdInteractQObj(pPlayer, nQuestID, vecQObjeciveID, pNPC->GetUID());

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_INTERACT_QOBJECTIVE_END, 
		1, 
		NEW_UID(pPlayer->GetUID()));
	pPlayer->RouteToThisCell(pNewCmd);
	
	if (IT_QUEST_INTERACT_MORTAL == pPlayer->GetInteraction().GetSelectedIElementInfo().m_nType)
	{
		pNPC->doDespawn(true);
		gsys.pTestSystem->DespawnLog(pNPC->GetUID(), __FUNCTIONW__);
	}	

	return true;
}