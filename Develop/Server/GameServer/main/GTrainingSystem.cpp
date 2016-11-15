#include "StdAfx.h"
#include "GTrainingSystem.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GCommandTable.h"
#include "GNPCMgr.h"
#include "GConditionsInfoMgr.h"
#include "GConditionsSystem.h"
#include "GInteractionSystem.h"
#include "GPlayerInteraction.h"
#include "GPalette.h"
#include "GPaletteSystem.h"
#include "GPlayerTalent.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "GConditionInfo.h"
#include "GPlayerSystem.h"
#include "GPlayerNPCIconSender.h"
#include "CCommandResultTable.h"
#include "GPlayerObjectManager.h"
#include "GItemManager.h"
#include "GDBTaskDataTalent.h"
#include "GItemRemover.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"


GTrainingSystem::GTrainingSystem()
{
}

GTrainingSystem::~GTrainingSystem()
{
}

bool GTrainingSystem::Train(GEntityPlayer *pReqPlayer, int nTalentID)
{
	GTalentInfo* pInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (pInfo == NULL)	return false;

	if (true == pInfo->m_bByLicense) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_NEED_USEITEM);
	if (CONSUME_TP_FOR_LEARN_TALENT > pReqPlayer->GetTalent().GetTP()) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_NOT_ENOUGH_TP);

	if (!Train_Check(pReqPlayer, nTalentID))	return false;
	if (!Train_DB(pReqPlayer, nTalentID))		return false;
	
	return true;
}

bool GTrainingSystem::Train_Check(GEntityPlayer* pReqPlayer, int nTalentID)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (pTalentInfo == NULL)	return false;
	// °°Àº ¶óÀÎÀÇ ³·Àº ·©Å© ÅÅ·±Æ®¸¦ ¹è¿ü´ÂÁö
	GTalentInfo* pLowRankTalentInfo = gmgr.pTalentInfoMgr->GetLowRankTalent(nTalentID);
	if (pLowRankTalentInfo != NULL)
	{
		if (false == pReqPlayer->GetTalent().IsTrainedTalent(pLowRankTalentInfo)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_CONDITION_CHECK_FAIL);
	}

	if (pTalentInfo->m_nTNeedLevel > pReqPlayer->GetPlayerInfo()->nLevel) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_NEED_LEVEL);
	if (0 != pTalentInfo->m_nTNeedTalent)
	{
		if (false == pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo->m_nTNeedTalent)) 
			return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_NEED_LINE);
	}
	if (pTalentInfo->m_nTNeedStyleTP > pReqPlayer->GetTalent().GetTrainedStyleTP(pTalentInfo->m_nStyle)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_NEED_STYPE_TP);
	if (true == pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_ALREADY_LEARNED);

	return true;
}

bool GTrainingSystem::Train_DB( GEntityPlayer* pReqPlayer, int nTalentID, bool bAsync/*=true*/ )
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (NULL == pTalentInfo)	return false;

	vector<pair<PALETTE_NUM, PALETTE_SLOT>> vecNumAndSlot;
	GTalentInfo* pLowRankTalentInfo = gmgr.pTalentInfoMgr->GetLowRankTalent(nTalentID);	
	if(NULL != pLowRankTalentInfo)
	{		
		pReqPlayer->GetPalette().GetNumAndSlotByTalentID(pLowRankTalentInfo->m_nID, vecNumAndSlot);		
	}

	bool bInPalette = false;
	int nLowRankTalentID = 0;
	if (false == vecNumAndSlot.empty())
	{
		bInPalette = true;
		nLowRankTalentID = pLowRankTalentInfo->m_nID;
	}

	GDBT_TALENT data(pReqPlayer->GetAID()
		, pReqPlayer->GetUID()
		, pReqPlayer->GetCID()
		, pReqPlayer->GetPlayerInfo()->nPlayTimeSec
		, pReqPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pReqPlayer->GetLevel()
		, pReqPlayer->GetMoney()
		, pTalentInfo->m_nID
		, nLowRankTalentID
		, pReqPlayer->GetTalent().GetTP() - CONSUME_TP_FOR_LEARN_TALENT
		, bInPalette);

	data.SetPalette(vecNumAndSlot);

	if (true == bAsync)
	{
		if (false == gsys.pDBManager->Training_Train(data)) 
		{
			return false;		
		}
	}
	else
	{
		if (false == gsys.pDBManager->Training_TrainSync(data)) 
		{
			return false;		
		}

	}

	return true;
}

bool GTrainingSystem::TrainForDBTask(GDBT_TALENT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return false;
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(data.m_nTalentID);
	if (NULL == pTalentInfo) return false;
	
	pPlayer->GetTalent().Insert(pTalentInfo);	
	
	for each (const pair<PALETTE_NUM, PALETTE_SLOT>& each in data.m_vNumAndSlot)
	{
		gsys.pPaletteSystem->PutUp(pPlayer, each.first, each.second, PIT_TALENT, pTalentInfo->m_nID, false);
	}

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_TALENT);

	MCommand* pNewCmd = MakeNewCommand(MC_TRAINING_TRAIN,
		2, 
		NEW_INT(data.m_nTalentID),
		NEW_BOOL(true));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GTrainingSystem::TrainByItem(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	vector<int>& vecTalentID = pItem->m_pItemData->m_vecUsableParam;
	if (vecTalentID.empty())	return false;

	for each(int nTalentID in vecTalentID)
	{
		if (!Train_Check(pPlayer, nTalentID))	return false;
	}

	GDBT_ITEM_LEARN_TALENT data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, pItem->m_nSlotType
		, pItem->m_nSlotID
		, pItem->m_pItemData->m_nID
		, pItem->m_nIUID
		, pPlayer->GetTalent().GetTP()
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetMoney());

	size_t nIdx = 0;
	for each(int nTalentID in vecTalentID)
	{
		if (NULL == gmgr.pTalentInfoMgr->Find(nTalentID))	return false;

		data.SetTalent(nIdx, nTalentID);
		++nIdx;
		_ASSERT(GDBT_ITEM_LEARN_TALENT::MAX_COUNT > nIdx);
		if (GDBT_ITEM_LEARN_TALENT::MAX_COUNT <= nIdx)
			return false;		
	}

	return gsys.pDBManager->ItemLearnTalent(data);
}


bool GTrainingSystem::TrainByItemForDBTask(GDBT_ITEM_LEARN_TALENT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return false;	
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, data.m_nSlotID);
	if (NULL == pItem) return false;
	GItemData* pItemData = pItem->m_pItemData;
	if (NULL == pItemData) return false;

	vector<int>& vecTID = pItemData->m_vecUsableParam;
	for (size_t i = 0; i < vecTID.size(); ++i)
	{
		GTalentInfo* pTInfo = gmgr.pTalentInfoMgr->Find(vecTID[i]);
		if (NULL == pTInfo)	return false;

		pPlayer->GetTalent().Insert(pTInfo, false);		

		MCommand* pNewCmd = MakeNewCommand(MC_TRAINING_TRAIN,
			2, 
			NEW_INT(pTInfo->m_nID),
			NEW_BOOL(false));

		pPlayer->RouteToMe(pNewCmd);
	}

	REMOVE_ITEM removeItem = REMOVE_ITEM(SLOTTYPE_INVENTORY, data.m_nSlotID, 0, pItem->GetAmount());
	gsys.pItemSystem->GetRemover().RemoveForDBTask(data.m_uidPlayer, removeItem);

	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_TALENT);

	return true;
}

bool GTrainingSystem::UntrainAll(GEntityPlayer* pReqPlayer, GItem* pItem)
{
	if (NULL == pReqPlayer)	return false;
	if (NULL == pItem) return false;
	if (true == pReqPlayer->GetTalent().IsEmpty()) return true;

	GDBT_TALENT data(pReqPlayer->GetAID()
		, pReqPlayer->GetUID()
		, pReqPlayer->GetCID()
		, pReqPlayer->GetPlayerInfo()->nPlayTimeSec
		, pReqPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pReqPlayer->GetLevel()
		, pReqPlayer->GetMoney()
		, 0
		, 0
		, pReqPlayer->GetPlayerInfo()->nLevel
		, false
		, pItem->m_nSlotID
		, pItem->m_nIUID
		, pItem->m_pItemData->m_nID);	

	data.SetRestAllTalentCountOnPalette(pReqPlayer->GetPalette().GetTalentCountOnPalette());

	return gsys.pDBManager->Training_UntrainAll(data);
}

void GTrainingSystem::UntrainAllForDBTask(GDBT_TALENT& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, data.m_nSlotID);
	if (NULL == pItem) return;	

	pPlayer->GetTalent().DeleteAll();	

	REMOVE_ITEM removeItem = REMOVE_ITEM(SLOTTYPE_INVENTORY, data.m_nSlotID, 0, pItem->GetAmount());
	gsys.pItemSystem->GetRemover().RemoveForDBTask(data.m_uidPlayer, removeItem);

	gsys.pPaletteSystem->PutDownAllTalent(pPlayer);
	pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_TALENT);	

	MCommand* pNewCmd = MakeNewCommand(MC_TRAINING_UNTRAIN_ALL, 
		1,
		NEW_INT(pPlayer->GetPlayerInfo()->nLevel));

	pPlayer->RouteToMe(pNewCmd);
}

bool GTrainingSystem::ForceTrain(GEntityPlayer* pReqPlayer, int nTalentID)
{
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (NULL == pTalentInfo) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_TALENT_ID);
	if (true == pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo)) return pReqPlayer->FailAndRouteSystemMsg(CR_FAIL_TRAINING_ALREADY_LEARNED);	

	if (1 > pReqPlayer->GetTalent().GetTP())
	{
		gsys.pPlayerSystem->AddTP(pReqPlayer, 1);
	}

	if (false == Train_DB(pReqPlayer, nTalentID, false)) return false;

	return true;
}