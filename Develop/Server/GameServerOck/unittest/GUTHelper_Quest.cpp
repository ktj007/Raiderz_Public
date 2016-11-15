#include "StdAfx.h"
#include "GUTHelper_Quest.h"
#include "SUnitTestUtil.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GItemManager.h"
#include "GItemData.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GTestForward.h"
#include "GQObjectiveInfo.h"
#include "GDialogInfo.h"

GQuestInfo* GUTHelper_Quest::NewQuestInfo(int nID)
{
	if (0 == nID) nID = SUnitTestUtil::NewID();

	GQuestInfo* pNewQuestInfo = new GQuestInfo();
	pNewQuestInfo->nID = nID;

	GDialogInfo* pNewDialogInfo = test.dialog.NewDialogInfo();

	pNewQuestInfo->nGiveDialogID = pNewDialogInfo->m_nID;
	if (gmgr.pQuestInfoMgr->Get(pNewQuestInfo->nID))
	{
		SAFE_DELETE(pNewQuestInfo);
	}
	gmgr.pQuestInfoMgr->Insert(pNewQuestInfo);

	return pNewQuestInfo;
}

GQObjectiveInfo* GUTHelper_Quest::NewQObjectiveInfo(GQuestInfo* pQuestInfo, int nID)
{
	if (NULL == pQuestInfo) return NULL;
	if (0 == nID) nID = SUnitTestUtil::NewID();

	GQObjectiveInfo* pNewQObjectiveInfo = new GQObjectiveInfo();
	pNewQObjectiveInfo->nID = nID;
	pNewQObjectiveInfo->nQuestID = pQuestInfo->nID;

	pQuestInfo->Insert(pNewQObjectiveInfo);

	return pNewQObjectiveInfo;
}

GQObjectiveInfo* GUTHelper_Quest::NewCollectQObjectiveInfo(GQuestInfo* pQuestInfo, int nItemQuantity, int nItemID, int nID)
{
	if (NULL == pQuestInfo) return NULL;
	if (0 == nID) nID = SUnitTestUtil::NewID();

	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData)
	{
		pItemData = test.item.NewItemData();
	}

	GQObjectiveInfo* pNewQObjectiveInfo = NewQObjectiveInfo(pQuestInfo, nID);
	if (NULL == pNewQObjectiveInfo) return NULL;

	pNewQObjectiveInfo->nType = QOT_COLLECT;
	pNewQObjectiveInfo->vecParam1.push_back(pItemData->m_nID);
	pNewQObjectiveInfo->nParam2 = nItemQuantity;

	gmgr.pQuestInfoMgr->MakeQOInfoCache_Collect(pNewQObjectiveInfo->vecParam1[0], pNewQObjectiveInfo);

	return pNewQObjectiveInfo;
}

GQObjectiveInfo* GUTHelper_Quest::NewDestroyQObjectiveInfo( GQuestInfo* pQuestInfo, int nDestroyQuantity, int nNPCID, int nID )
{
	if (NULL == pQuestInfo) return NULL;
	if (0 == nID) nID = SUnitTestUtil::NewID();

	GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(nNPCID);
	if (NULL == pNPCInfo)
	{
		pNPCInfo = test.npc.NewNPCInfo();
	}

	GQObjectiveInfo* pNewQObjectiveInfo = NewQObjectiveInfo(pQuestInfo, nID);
	if (NULL == pNewQObjectiveInfo) return NULL;

	pNewQObjectiveInfo->nType = QOT_DESTROY;
	pNewQObjectiveInfo->vecParam1.push_back(pNPCInfo->nID);
	pNewQObjectiveInfo->nParam2 = nDestroyQuantity;

	gmgr.pQuestInfoMgr->MakeQOInfoCache_Destroy(pNewQObjectiveInfo->vecParam1[0], pNewQObjectiveInfo);

	return pNewQObjectiveInfo;
}

GQObjectiveInfo* GUTHelper_Quest::NewScoutQObjectiveInfo( GQuestInfo* pQuestInfo, int nSensorID, int nFieldID, int nID)
{
	if (NULL == pQuestInfo) return NULL;
	if (0 == nID) nID = SUnitTestUtil::NewID();

	GFieldInfo* pFieldInfo = test.field.NewFieldInfo(nFieldID);

	GQObjectiveInfo* pNewQObjectiveInfo = NewQObjectiveInfo(pQuestInfo, nID);
	if (NULL == pNewQObjectiveInfo) return NULL;

	pNewQObjectiveInfo->nType = QOT_DESTROY;
	pNewQObjectiveInfo->vecParam1.push_back(pFieldInfo->m_nFieldID);
	pNewQObjectiveInfo->nParam2 = nSensorID;

	gmgr.pQuestInfoMgr->MakeQOInfoCache_Scout(pNewQObjectiveInfo->vecParam1[0], pNewQObjectiveInfo);

	return pNewQObjectiveInfo;
}

GQObjectiveInfo* GUTHelper_Quest::NewInteractQObjectiveInfo( GQuestInfo* pQuestInfo, int nNPCID, int nInteractCount )
{
	if (NULL == pQuestInfo) return NULL;

	GQObjectiveInfo* pNewQObjectiveInfo = NewQObjectiveInfo(pQuestInfo);
	if (NULL == pNewQObjectiveInfo) return NULL;

	pNewQObjectiveInfo->nType = QOT_INTERACT;
	pNewQObjectiveInfo->vecParam1.push_back(nNPCID);
	pNewQObjectiveInfo->nParam2 = nInteractCount;

	return pNewQObjectiveInfo;
}

GPlayerQuest* GUTHelper_Quest::GiveNewPlayerQuest(GEntityPlayer* pPlayer, int nID, int nTimeLimit, bool bHasObjective)
{
	static uint8 nSlotID = 0;

	if (NULL == pPlayer) return NULL;

	GQuestInfo * pQuestInfo = gmgr.pQuestInfoMgr->Get(nID);
	if (NULL == pQuestInfo)
	{
		pQuestInfo = NewQuestInfo(nID);

		if (bHasObjective)
		{
			CSQObjectiveInfo* pObjective = new CSQObjectiveInfo();
			pObjective->nID = 1;
			pObjective->nQuestID = pQuestInfo->nID;
			pQuestInfo->Insert(pObjective);
		}
	}

	pQuestInfo->nTimeLimit = nTimeLimit;

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Insert(nSlotID++, pQuestInfo->nID, L"", L"");	

	if (MAX_QUEST_INSTACNE_COUNT <= nSlotID)
		nSlotID = 0;
	
	return pPlayerQuest;
}

void GUTHelper_Quest::GiveNewPlayerDoneQuest( GEntityPlayer* pPlayer, int nID /*= 0*/, int nTimeLimit /*= 0*/, bool bHasObjective/*=false*/ )
{
	GPlayerQuest* pPlayerQuest = GiveNewPlayerQuest(pPlayer, nID, nTimeLimit, bHasObjective);
	if (NULL == pPlayerQuest) return;

	DoneQuest(pPlayer, pPlayerQuest->GetInfo()->nID);
}

void GUTHelper_Quest::DropQuest(GEntityPlayer* pPlayer, int nID)
{
	if (NULL == pPlayer) return;

	pPlayer->GetQuests().Cancel(nID);
}

void GUTHelper_Quest::FailQuest(GEntityPlayer* pPlayer, int nID)
{
	if (NULL == pPlayer) return;

	GPlayerQuest* pQuest = pPlayer->GetQuests().Get(nID);
	if (pQuest)
	{
		pQuest->Fail();
	}
}

void GUTHelper_Quest::CompleteQuest(GEntityPlayer* pPlayer, int nID)
{
	if (NULL == pPlayer) return;

	GPlayerQuest* pQuest = pPlayer->GetQuests().Get(nID);
	if (pQuest)
	{
		pQuest->Complete();
	}
}

void GUTHelper_Quest::CompleteQuestAllObjective(GEntityPlayer* pPlayer, int nID)
{
	if (NULL == pPlayer) return;

	GPlayerQuest* pQuest = pPlayer->GetQuests().Get(nID);
	if (pQuest)
	{
		pQuest->AllQObjectiveComplete();
	}
}

void GUTHelper_Quest::DoneQuest( GEntityPlayer* pPlayer, int nID )
{
	if (NULL == pPlayer) return;
	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nID);
	if (NULL == pPlayerQuest) return;

	pPlayer->GetQuests().Done(nID);
	SAFE_DELETE(pPlayerQuest);
}

void GUTHelper_Quest::AddSelectableReward( GQuestInfo* pQuestInfo, const QUEST_REWARD& reward )
{
	pQuestInfo->vecSelectableReward.push_back(reward);
}

void GUTHelper_Quest::AddReward( GQuestInfo* pQuestInfo, const QUEST_REWARD& reward )
{
	pQuestInfo->vecReward.push_back(reward);
}