#include "StdAfx.h"
#include "GItemUser.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GConditionsSystem.h"
#include "GPlayerTalent.h"
#include "GCommand.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GPlayerQuests.h"
#include "GPlayerInteraction.h"
#include "GDialogSystem.h"
#include "GDialogInfo.h"
#include "GInteractionSystem.h"
#include "GTalentInfoMgr.h"
#include "GModuleCombat.h"
#include "GTrainingSystem.h"
#include "GNPCInteractor.h"
#include "GItemEquiper.h"
#include "GActorCooltimeChecker.h"
#include "GItemActableChecker.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GDBManager.h"
#include "GItemRemover.h"
#include "AStlUtil.h"
#include "GDBTaskDataTalent.h"
#include "CSConditionInfo.h"
#include "GConditionInfo.h"


bool GItemUser::Use( GEntityPlayer* pPlayer, int nInvenSlotID, const MUID uidNPC /*= MUID(0, 0)*/ )
{
	if (NULL == pPlayer) return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nInvenSlotID);
	if (pItem == NULL) return false;

	GCondition_Leaf* pFailConditionLeaf = NULL;
	if (false == Check(pPlayer, pItem, pFailConditionLeaf))
	{
		RouteFail(pPlayer, nInvenSlotID, pFailConditionLeaf);
		return false;
	}
	
	if (false == Apply(pPlayer, pItem, uidNPC))
	{
		RouteFail(pPlayer, nInvenSlotID);
		return false;
	}

	return true;
}

bool GItemUser::Check(GEntityPlayer* pPlayer, GItem* pItem, GCondition_Leaf*& pFailConditionLeaf)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;
	
	vector<GCondition_Leaf*> vecFailConditionLeaf;
	if (false == gsys.pConditionsSystem->CheckForItem(pPlayer, pItem->m_pItemData->m_nConditionsInfoID, vecFailConditionLeaf))
	{
		if (!vecFailConditionLeaf.empty())
		{
			pFailConditionLeaf = vecFailConditionLeaf.front();
		}

		return false;
	}

	if (false == gsys.pItemSystem->GetActableChecker().IsUsable(pPlayer, pItem))
	{
		return false;
	}

	return true;
}

bool GItemUser::Apply(GEntityPlayer* pPlayer, GItem* pItem, const MUID uidNPC)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;
	GItemData* pItemData = pItem->m_pItemData;
	if (pItemData == NULL) return false;	

	switch (pItemData->m_ItemType)
	{
	case ITEMTYPE_WEAPON:
	case ITEMTYPE_ARMOR:	
	case ITEMTYPE_HOUSING:	return Apply_Equip(pPlayer, pItem);			
	case ITEMTYPE_USABLE:	return Apply_Usable(pPlayer, pItem, uidNPC);	
	}	

	return false;
}

bool GItemUser::Apply_Equip(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return gsys.pItemSystem->GetEquiper().Equip(pPlayer, pItem, pItem->m_pItemData->m_nItemSlot);
}

bool GItemUser::Apply_Usable(GEntityPlayer* pPlayer, GItem* pItem, const MUID uidNPC)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	switch (pItem->m_pItemData->m_nUsableType)
	{
	case USABLE_TALENT_USE:			return Apply_TalentUse(pPlayer, pItem);
	case USABLE_TALENT_TRAIN:		return Apply_TalentTrain(pPlayer, pItem);
	case USABLE_TALENT_UNTRAIN:		return Apply_TalentUntrain(pPlayer, pItem);
	case USABLE_INTERACTION:
	case USABLE_INTERACTION_DEAD:	return Apply_Interaction(pPlayer, pItem, uidNPC);
	case USABLE_QUEST_ADD:			return Apply_QuestAdd(pPlayer, pItem);
	}

	return false;
}

bool GItemUser::Apply_Interaction(GEntityPlayer* pPlayer, GItem* pItem, const MUID uidNPC)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	GItemData* pItemData = pItem->m_pItemData;
	if (NULL == pItemData) return false;

	bool bLive = (USABLE_INTERACTION == pItemData->m_nUsableType) ? true : false;
	GEntityNPC* pNPC = GetInteractionNPC(pPlayer, pItem, uidNPC, bLive);
	if (NULL == pNPC) return false;

	if (!gsys.pInteractionSystem->GetNPCInteractor().InteractionByItem(pPlayer, pNPC, pItem->GetUID()))
		return false;

	return true;
}

GEntityNPC* GItemUser::GetInteractionNPC(GEntityPlayer* pPlayer, GItem* pItem, const MUID& uidNPC, const bool IsLive)
{
	if (!pItem->IsInteractableType())
		return NULL;

	GItemData* pItemData = pItem->m_pItemData;
	if (NULL == pItemData)
		return NULL;

	GEntityNPC* pNPC = pPlayer->FindNPC(uidNPC);
	if (NULL == pNPC)	
		return NULL;

	if (false == IsExist(pItemData->m_vecUsableParam, pNPC->GetNPCInfo()->nID))
		return NULL;

	if (IsLive)
	{
		if (pNPC->IsDead())
			return NULL;
	}
	else
	{
		if (!pNPC->IsDead())
			return NULL;
	}

	return pNPC;
}

bool GItemUser::Apply_TalentUse(GEntityPlayer* pPlayer, GItem* pItem)
{
	pPlayer->GetTalent().EnableUseItemTalent(pItem->GetUID());
	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_TALENT, 2, NEW_INT(pItem->GetAmount()), NEW_INT(pItem->m_nSlotID));
	pPlayer->RouteToMe(pNewCommand);

	return true;
}

bool GItemUser::Apply_QuestAdd( GEntityPlayer* pPlayer, GItem* pItem )
{
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(pItem->m_pItemData->m_vecUsableParam.front());
	if (NULL == pQuestInfo) return false;			

	if (false == pPlayer->GetQuests().IsDoingQuest(pItem->m_pItemData->m_vecUsableParam.front()))
	{
		pPlayer->GetInteraction().BeginInteraction(ITRT_ITEM, ITAT_ITEM, pItem->GetUID(), vector<GNPCInteractionElementInfo*>());

		GNPCInteractionElementInfo IElementInfo;
		IElementInfo.m_nType = IT_QUEST_BEGIN;
		IElementInfo.m_vecAct.push_back(pItem->m_pItemData->m_vecUsableParam.front());
		pPlayer->GetInteraction().SelectIElement(IElementInfo);
		gsys.pDialogSystem->Start(pPlayer, pQuestInfo->nGiveDialogID);
	}

	return true;
}

bool GItemUser::Apply_TalentTrain(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return gsys.pTrainingSystem->TrainByItem(pPlayer, pItem);	
}

bool GItemUser::Apply_TalentUntrain(GEntityPlayer* pPlayer, GItem* pItem)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pItem, false);

	return gsys.pTrainingSystem->UntrainAll(pPlayer, pItem);
}

void GItemUser::ConsumeUsableItem(GEntityPlayer* pPlayer, MUID nItemUID)
{
	if (NULL == pPlayer) return;	

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();	
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nItemUID);

	ConsumeUsableItem(pPlayer, pItem);	
}

void GItemUser::ConsumeUsableItem(GEntityPlayer* pPlayer, GItem* pItem)
{

	if (NULL == pPlayer) return;
	if (NULL == pItem) return;

	if (ITEMTYPE_USABLE != pItem->m_pItemData->m_ItemType) return;
	if (false == pItem->m_pItemData->m_bUsableConsume) return;
	if (false == gsys.pItemSystem->GetRemover().Remove(pPlayer, pItem->m_nSlotType, pItem->m_nSlotID, 1)) return;

	return;
}

void GItemUser::RouteFail(GEntityPlayer* pPlayer, int nInvenSlotID, GCondition_Leaf* pFailConditionLeaf)
{
	VALID(pPlayer);

	CONDITION_TYPE nConditionType = CT_NONE;
	vector<int> vecParam1;
	vector<int> vecParam2;
	if (NULL != pFailConditionLeaf)
	{
		nConditionType = pFailConditionLeaf->m_nCT;
		vecParam1 = pFailConditionLeaf->m_vecParam1;
		vecParam2 = pFailConditionLeaf->m_vecParam2;
	}

	MCommand* pNewCommand = MakeNewCommand(MC_ITEM_USE_FAIL, 
		4,
		NEW_INT(nInvenSlotID), 
		NEW_INT(nConditionType), 
		NEW_BLOB(vecParam1), 
		NEW_BLOB(vecParam2));

	pPlayer->RouteToMe(pNewCommand);
}