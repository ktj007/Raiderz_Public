#include "StdAfx.h"
#include "GQuestCompleter.h"
#include "GPlayerQuest.h"
#include "GEntityPlayer.h"
#include "GPlayerQuests.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GScriptManager.h"
#include "GScriptGroup_Quest.h"
#include "GScriptUtil.h"
#include "GNullGlueQuest.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GItemAdder.h"
#include "GQObjectiveInfo.h"
#include "GQuestInfo.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"
#include "GQuestVarUpdater.h"


bool GQuestCompleter::Complete(GEntityPlayer* pPlayer, int nQuestID)
{
	if (NULL == pPlayer) return false;

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return false;
	if (false == pPlayerQuest->IsAllQObjectiveComplete()) return false;

	pPlayerQuest->Complete();
	pPlayer->OnQuestComplete(nQuestID);

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_COMPLETE, 1, NEW_INT(nQuestID));
	pPlayer->RouteToMe(pNewCmd);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);

	GetScriptQuest()->OnComplete(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator());	

	return true;
}

void GQuestCompleter::CompleteForGM(GEntityPlayer* pPlayer, int nQuestID)
{
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return;

	vector<pair<int,int>> vecQuestNeedItem = pPlayerQuest->GetQuestNeedItem();

	for each (const vector<pair<int,int>>::value_type& val in vecQuestNeedItem)
	{
		int nItemID = val.first;
		int nItemAmount = val.second;

		gsys.pItemSystem->GetAdder().Add(pPlayer, nItemID, nItemAmount);
	}

	// 위에서 아이템 지급하면서 이미 완료되었기 때문에 QOT_COLLECT는 제외하고 완료시킨다.
	gsys.pQuestSystem->GetQObjUpdater().CompleteAllQObj(pPlayer, nQuestID, QOT_COLLECT);
}

bool GQuestCompleter::Incomplete(GEntityPlayer* pPlayer, int nQuestID)
{
	if (NULL == pPlayer) return false;

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return false;
	if (true == pPlayerQuest->IsAllQObjectiveComplete()) return false;

	pPlayerQuest->Incomplete();	

	MCommand* pNewCmd = MakeNewCommand(MC_QUEST_INCOMPLETE, 1, NEW_INT(nQuestID));
	pPlayer->RouteToMe(pNewCmd);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST);

	return true;
}