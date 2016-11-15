#include "StdAfx.h"
#include "GQuestUIShower.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"

bool GQuestUIShower::ShowAcceptUI(GEntityPlayer* pPlayer, int nQuestID)
{	
	if (NULL == pPlayer) return false;	

	if (false == gmgr.pQuestInfoMgr->IsExist(nQuestID)) return false;

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_SHOW_ACCPET_UI,
		1,
		NEW_INT(nQuestID));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GQuestUIShower::ShowRewardUI(GEntityPlayer* pPlayer, int nQuestID)
{
	if (NULL == pPlayer) return false;

	if (false == gmgr.pQuestInfoMgr->IsExist(nQuestID)) return false;

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return false;
	if (false == pPlayerQuest->IsComplete()) return false;

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_SHOW_REWARD_UI,
		1,
		NEW_INT(nQuestID));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}