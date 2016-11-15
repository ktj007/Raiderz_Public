#include "StdAfx.h"
#include "GQuestVarUpdater.h"
#include "GQuestInfo.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"
#include "GEntityPlayer.h"
#include "GPlayerQuests.h"
#include "GDBTaskQuestData.h"
#include "GPlayerQuest.h"
#include "GDBManager.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GPartySystem.h"
#include "GConst.h"


bool GQuestVarUpdater::Update(GEntityPlayer* pPlayer, int nQuestID, int nVar)
{
	VALID_RET(pPlayer, false);
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo, false);

	vector<GEntityPlayer*> vecQuestVarShrablePlayer = GetQuestVarSharablePlayer(pPlayer, pQuestInfo->bPublicVar);
	for each (GEntityPlayer* pQuestVarSharablePlayer in vecQuestVarShrablePlayer)
	{
		UpdateSingle(pQuestVarSharablePlayer, nQuestID, nVar);
	}

	gsys.pQuestSystem->GetQObjUpdater().UpdActQObj(pPlayer, nQuestID, nVar);

	return true;
}

void GQuestVarUpdater::UpdateSingle(GEntityPlayer* pPlayer, int nQuestID, int nVar)
{
	_ASSERT(pPlayer != NULL);

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return;

	GDBT_QUEST_VAR data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayerQuest->GetSlotID()
		, pPlayerQuest->GetInfo()->nID
		, nVar);

	gsys.pDBManager->QuestUpdateVar(data);

	pPlayerQuest->UpdateQuestVar(nVar);
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_QUEST_VAR);
}

vector<GEntityPlayer*> GQuestVarUpdater::GetQuestVarSharablePlayer( GEntityPlayer* pPlayer, bool bPublic )
{
	VALID_RET(pPlayer, vector<GEntityPlayer*>());

	vector<GEntityPlayer*> vecQuestVarSharablePlayer;

	if (true == bPublic)
	{
		vecQuestVarSharablePlayer = gsys.pPartySystem->CollectNeighborMember(pPlayer, GConst::QUEST_OBJECTIVE_UPDATE_SHARE_DISTANCE);
	}
	else
	{
		vecQuestVarSharablePlayer.push_back(pPlayer);
	}

	return vecQuestVarSharablePlayer;
}
