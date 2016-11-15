#include "StdAfx.h"
#include "GQuestReseter.h"
#include "GEntityPlayer.h"
#include "GPlayerQuests.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"

bool GQuestReseter::ResetDoneQuestForGM( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	pPlayer->GetQuests().ClearDoneCount();

	gsys.pDBManager->GM_QUEST_HISTORY_RESET_ALL(pPlayer->GetAID(), (int64)pPlayer->GetCID());

	MCommand* pNewCmd = MakeNewCommand(MC_GM_QUEST_RESET, 0, NULL);

	pPlayer->RouteToMe(pNewCmd);
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);

	return true;
}

bool GQuestReseter::ResetAllQuestForGM( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	pPlayer->GetQuests().Clear();
	pPlayer->GetQuests().ClearDoneCount();	

	gsys.pDBManager->GM_QUEST_RESET_ALL(pPlayer->GetAID(), (int64)pPlayer->GetCID());

	gsys.pDBManager->GM_QUEST_HISTORY_RESET_ALL(pPlayer->GetAID(), (int64)pPlayer->GetCID());

	MCommand* pNewCmd = MakeNewCommand(MC_GM_QUEST_RESET, 0, NULL);

	pPlayer->RouteToMe(pNewCmd);
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);

	return true;
}