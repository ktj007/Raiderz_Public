#include "stdafx.h"
#include "GQObjUpdater.h"
#include "GQuestInfo.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GEntityPlayer.h"
#include "GPlayerQObjective.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GScriptManager.h"
#include "GScriptGroup_Quest.h"
#include "GScriptUtil.h"
#include "GNullGlueQuest.h"
#include "GInteractionSystem.h"
#include "GConditionInfo.h"
#include "GQuestSystem.h"
#include "GPartySystem.h"
#include "CCommandResultTable.h"
#include "GConst.h"
#include "GPlayerNPCIconSender.h"
#include "GPlayerObjectManager.h"
#include "GEntityNPC.h"
#include "GQObjectiveInfo.h"
#include "GDBTaskQuestData.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GQuestCompleter.h"
#include "GQuestInfoMgr.h"
#include "GQObjUpdaterForDBTask.h"
#include "GHateTable.h"

GQObjUpdater::GQObjUpdater()
{
	// do nothig 
}

GQObjUpdater::~GQObjUpdater()
{
	// do nothig 
}

bool GQObjUpdater::IncCollectQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar)
{
	if (NULL == pPlayer) return false;

	return UpdateQObjectiveByParam1(pPlayer, QOT_COLLECT, nObjectiveParam1, nVar);
}

bool GQObjUpdater::DecCollectQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar)
{
	if (NULL == pPlayer) return false;

	return UpdateQObjectiveByParam1(pPlayer, QOT_COLLECT, nObjectiveParam1, -nVar);
}

bool GQObjUpdater::UpdDestroyQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar, GEntityNPC* pNPC)
{
	if (NULL == pPlayer) return false;
	if (NULL == pNPC) return false;

	return UpdateQObjectiveByParam1(pPlayer, QOT_DESTROY, nObjectiveParam1, nVar, pNPC->GetUID());
}

bool GQObjUpdater::UpdScoutQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar)
{
	if (NULL == pPlayer) return false;

	return UpdateQObjectiveByParam1(pPlayer, QOT_SCOUT, nObjectiveParam1, nVar);
}

bool GQObjUpdater::UpdEscortQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar)
{
	if (NULL == pPlayer) return false;

	return UpdateQObjectiveByParam1(pPlayer, QOT_ESCORT, nObjectiveParam1, nVar);
}


bool GQObjUpdater::UpdInteractQObj(GEntityPlayer* pPlayer, int nQuestID, const vector<int>& vecQObjeciveID, MUID uidNPC)
{
	VALID_RET(pPlayer, false);

	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return false;

	vector<GQObjectiveInfo*> vecQObjectiveInfo;
	for each (int nQObjectiveID in vecQObjeciveID)
	{
		GQObjectiveInfo* pQObjectiveInfo = pQuestInfo->Get(nQObjectiveID);
		if (NULL == pQObjectiveInfo) continue;
		if (QOT_INTERACT != pQObjectiveInfo->nType) continue;

		vecQObjectiveInfo.push_back(pQObjectiveInfo);
	}
	
	UpdateQObjective(pPlayer, 1, vecQObjectiveInfo, uidNPC);

	return true;
}

bool GQObjUpdater::UpdateQObjectiveByParam1(GEntityPlayer* pPlayer, QOBJECTIVE_TYPE nObjectiveType, int nObjectiveParam1, int nVar, MUID uidNPC)
{
	if (NULL == pPlayer) return true;

	vector<GQObjectiveInfo*> vecQObjectiveInfo = gmgr.pQuestInfoMgr->GetQObjectiveByParam1(nObjectiveType, nObjectiveParam1);
	if (true == vecQObjectiveInfo.empty()) return true;

	UpdateQObjective(pPlayer, nVar, vecQObjectiveInfo, uidNPC);

	return true;
}

bool GQObjUpdater::UpdActQObj(GEntityPlayer* pPlayer, int nQuestID, int nVar)
{
	VALID_RET(pPlayer, false);
	
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return false;

	vector<GQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetActQObjectiveInfo();

	UpdateQObjective(pPlayer, nVar, vecQObjectiveInfo);

	return true;
}

void GQObjUpdater::UpdateQObjective(GEntityPlayer* pPlayer, int nVar, const vector<GQObjectiveInfo*>& vecQObjectiveInfo, MUID uidNPC)
{
	if (NULL == pPlayer) return;

	int nMemberCount = gsys.pPartySystem->CalculateOnlineMemberCount(pPlayer->GetUID());

	vector<GQObjectiveInfo*> vecSharableQObjectiveInfo;
	vector<GQObjectiveInfo*> vecUnsharableQObjectiveInfo;

	for each (GQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
	{
		GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pQObjectiveInfo->nQuestID);
		if (NULL == pQuestInfo) continue;
		if (nMemberCount > pQuestInfo->nLimitPersonnel) continue;

		vector<GEntityPlayer*> vecUpdatePlayer = CollectQOUpdatePlayer(pPlayer, pQObjectiveInfo, uidNPC);
		for each (GEntityPlayer* pUpdatePlayer in vecUpdatePlayer)
		{
			bool bTrigger = (pPlayer == pUpdatePlayer);
			UpdateSingleQObjective(pUpdatePlayer, pQObjectiveInfo, nVar, bTrigger, uidNPC);
		}
	}
}

vector<GEntityPlayer*> GQObjUpdater::CollectQOUpdatePlayer(GEntityPlayer* pPlayer, GQObjectiveInfo* pQObjectiveInfo, MUID uidNPC)
{
	if (NULL == pPlayer) return vector<GEntityPlayer*>();
	if (NULL == pQObjectiveInfo) return vector<GEntityPlayer*>();

	vector<GEntityPlayer*> vecUpdatePlayer;
	if (QOT_COLLECT == pQObjectiveInfo->nType)
	{
		vecUpdatePlayer.push_back(pPlayer);
		return vecUpdatePlayer;
	}

	switch (pQObjectiveInfo->nPublicProgress)
	{
	case QPP_SELF:
		{
			vecUpdatePlayer.push_back(pPlayer);
		}
		break;
	case QPP_PARTY:
		{
			vecUpdatePlayer = gsys.pPartySystem->CollectNeighborMember(pPlayer, GConst::QUEST_OBJECTIVE_UPDATE_SHARE_DISTANCE);
		}
		break;
	case QPP_SECTOR:	
		{
			vector<MUID> vecNeighborPlayerUID;
			pPlayer->GetNeighborSectorPlayer(vecNeighborPlayerUID);

			if (QOT_DESTROY == pQObjectiveInfo->nType)
			{
				GEntityNPC* pNPC = pPlayer->FindNPC(uidNPC);
				if (NULL == pNPC) return vector<GEntityPlayer*>();			
				
				for each (const MUID& nNeighborPlayerUID in vecNeighborPlayerUID)
				{
					if (false == pNPC->GetHateTable().IsExist(nNeighborPlayerUID)) continue;

					GEntityPlayer* pNeighborPlayer = pPlayer->FindPlayer(nNeighborPlayerUID);
					vecUpdatePlayer.push_back(pNeighborPlayer);
				}
			}
			else
			{
				for each (const MUID& nNeighborPlayerUID in vecNeighborPlayerUID)
				{
					GEntityPlayer* pNeighborPlayer = pPlayer->FindPlayer(nNeighborPlayerUID);
					vecUpdatePlayer.push_back(pNeighborPlayer);
				}
			}			
		}
		break;
	}

	return vecUpdatePlayer;
}

void GQObjUpdater::UpdateSingleQObjective(GEntityPlayer* pPlayer, GQObjectiveInfo* pQObjectiveInfo, const int nVar, bool bTrigger, MUID uidNPC)
{
	VALID(pPlayer);
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(pQObjectiveInfo->nQuestID);
	if (!pPlayerQuest) return;

	if (true == IsScriptUpdatableObjectiveType(pQObjectiveInfo->nType))
	{
		GetScriptQuest()->OnObjectiveProgress(TO_SAFE_LUAPTR(pPlayerQuest), pPlayer->GetDelegator(), pQObjectiveInfo->nID, bTrigger);
	}	

	int nUpdateProgress = 0;
	if (false == pPlayerQuest->IsQObjectiveUpdatableVar(pQObjectiveInfo->nID, nVar, nUpdateProgress))
		return;
		
	GDBT_QUEST_OBJECT data;
	MakeDBTaskData(pPlayer, pPlayerQuest, pQObjectiveInfo, nUpdateProgress, bTrigger, uidNPC, data);
	
	gsys.pDBManager->QuestUpdateObject(data);
	gsys.pQuestSystem->GetQObjUpdaterForDBTask().Update(data);	
	 
	if (pPlayerQuest->IsComplete())
		gsys.pDBManager->QuestCompleteLog(data.m_QuestComm);
}

bool GQObjUpdater::IsScriptUpdatableObjectiveType(QOBJECTIVE_TYPE nType)
{
	if (QOT_DESTROY != nType 
		&& QOT_INTERACT != nType 
		&& QOT_COLLECT != nType)
		return false;

	return true;
}


void GQObjUpdater::MakeDBTaskData( GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GQObjectiveInfo* pQObjectiveInfo, int nProgress, bool bTrigger, MUID uidNPC, GDBT_QUEST_OBJECT& data )
{
	VALID(pPlayer);
	VALID(pPlayerQuest);
	VALID(pQObjectiveInfo);

	data = GDBT_QUEST_OBJECT(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pPlayerQuest->GetSlotID()
		, pQObjectiveInfo->nQuestID		
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, pQObjectiveInfo->nID
		, nProgress
		, pPlayerQuest->IsComplete()
		, bTrigger
		, uidNPC
		, pPlayerQuest->GetAcceptDt()
		, pPlayer->GetFieldID());
}

bool GQObjUpdater::CompleteAllQObj( GEntityPlayer* pPlayer, int nQuestID, QOBJECTIVE_TYPE nExceptType /*= QOT_NONE*/ )
{
	VALID_RET(pPlayer, false);
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo, false);	

	vector<GQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetAllQObjectiveInfo();
	for each (GQObjectiveInfo* pInfo in vecQObjectiveInfo)
	{
		if (pInfo->nType == nExceptType) continue;

		int nVar = (QOT_ACT == pInfo->nType) ? pInfo->vecParam1[0] : pInfo->nParam2;
		UpdateSingleQObjective(pPlayer, pInfo, nVar, true);
	}

	return true;
}
