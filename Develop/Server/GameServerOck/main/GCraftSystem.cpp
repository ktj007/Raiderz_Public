#include "StdAfx.h"
#include "GCraftSystem.h"
#include "GCraftInfoMgr.h"
#include "GCraftInfo.h"
#include "GGlobal.h"
#include "GRecipeInfoMgr.h"
#include "GRecipeInfo.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GNPCInteractionInfo.h"
#include "GEntityPlayer.h"
#include "GPlayerFactions.h"
#include "CSFactionCalculator.h"
#include "GCommand.h"
#include "GItemManager.h"
#include "GDBManager.h"
#include "GQuestSystem.h"
#include "GItemAdder.h"
#include "GItemRemover.h"
#include "GSTLUtil.h"
#include "GConditionsSystem.h"
#include "GPlayerSystem.h"
#include "GPlayerRecipe.h"
#include "GItemHolder.h"
#include "CCommandTable.h"
#include "GPlayerInteraction.h"
#include "CCommandResultTable.h"
#include "GItemActableChecker.h"
#include "GConst.h"
#include "GDBTaskCraftData.h"
#include "GDBTaskDataRecipe.h"

GCraftSystem::GCraftSystem()
{
}

GCraftSystem::~GCraftSystem()
{
}

bool GCraftSystem::Show(GEntityPlayer* pPlayer, int nCraftID)
{
	if (NULL == pPlayer) return false;

	GCraftInfo* pCraftInfo = gmgr.pCraftInfoMgr->Get(nCraftID);
	if (NULL == pCraftInfo) return false;

	Show_Route(pPlayer, pCraftInfo);

	return true;
}

void GCraftSystem::Show_Route(GEntityPlayer* pPlayer, const GCraftInfo* pCraftInfo)
{
	float fMakeMod = CalcMakeMod(pPlayer, pCraftInfo);

	vector<TD_CRAFT_RECIPE> vecCraftRecipe;
	for each (int nRecipeID in pCraftInfo->m_setRecipeID)
	{
		GRecipeInfo* pRecipeInfo = gmgr.pRecipeInfoMgr->Get(nRecipeID);
		if (NULL == pRecipeInfo) continue;

		bool bMakable = gsys.pConditionsSystem->Check(pPlayer, pRecipeInfo->m_nConditionID);
		vecCraftRecipe.push_back(TD_CRAFT_RECIPE(nRecipeID, bMakable));
	}

	MCommand* pNewCmd = MakeNewCommand(MC_CRAFT_SHOW,
		2,		
		NEW_BLOB(vecCraftRecipe),
		NEW_FLOAT(fMakeMod));

	pPlayer->RouteToMe(pNewCmd);
}

bool GCraftSystem::Make(GEntityPlayer* pPlayer, int nCraftID, int nRecipeID)
{
	VALID_RET(pPlayer, false);
	
	GCraftInfo* pCraftInfo = gmgr.pCraftInfoMgr->Get(nCraftID);
	VALID_RET(pCraftInfo, false);	
	GRecipeInfo* pRecipeInfo = gmgr.pRecipeInfoMgr->Get(nRecipeID);
	VALID_RET(pRecipeInfo, false);

	int nPrice = 0;
	GITEM_STACK_AMT_VEC vecRecpItemList;
	if (false == Make_Check(pPlayer, pCraftInfo, pRecipeInfo, nPrice, vecRecpItemList)) return false;	
	if (false == Make_Apply(pPlayer, pRecipeInfo->m_nProductItemID, pRecipeInfo->m_nAmount, nPrice, vecRecpItemList)) return false;

	Make_Route(pPlayer, nRecipeID);	

	return true;
}

bool GCraftSystem::MakeRecipeItemList( GEntityPlayer* pPlayer, GRecipeInfo* pRecpInfo, GITEM_STACK_AMT_VEC& vecRecp )
{
	for each(const RecipeMaterial& material in pRecpInfo->m_vecMaterial)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		int nCollectRemainAmount = material.m_nAmount;

		nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_INVENTORY, material.m_nItemID, nCollectRemainAmount, vecRecp);
		if (0 != nCollectRemainAmount)	
		{
			nCollectRemainAmount = pItemHolder->CollectItemUntilAmount(SLOTTYPE_EQUIP, material.m_nItemID, nCollectRemainAmount, vecRecp);
			if (0 != nCollectRemainAmount)

			return false;
		}
	}

	return true;
}

bool GCraftSystem::Make_Check(GEntityPlayer* pPlayer, GCraftInfo* pCraftInfo, GRecipeInfo* pRecipeInfo, int& outnPrice, GITEM_STACK_AMT_VEC& outvecRecpItemList)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pCraftInfo, false);
	VALID_RET(pRecipeInfo, false);
	
	if (false == gsys.pConditionsSystem->Check(pPlayer, pRecipeInfo->m_nConditionID)) return false;

	if (false == MakeRecipeItemList(pPlayer, pRecipeInfo, outvecRecpItemList))	return false;

	for each (const GITEM_STACK_AMT& recipeItem in  outvecRecpItemList)
	{
		GItem* pRecipeItem = pPlayer->GetItemHolder()->GetItem(static_cast<SH_ITEM_SLOT_TYPE>(recipeItem.nSlotType), recipeItem.nSlotID);
		if (NULL == pRecipeItem) return false;

		if (false == gsys.pItemSystem->GetActableChecker().IsCraftable(pPlayer, pRecipeItem)) return false;
	}	

	if (false == gmgr.pItemManager->IsExist(pRecipeInfo->m_nProductItemID)) return false;

	float fMakeMod = CalcMakeMod(pPlayer, pCraftInfo);
	outnPrice = static_cast<int>(pRecipeInfo->m_nPrice * fMakeMod);

	if (outnPrice > pPlayer->GetPlayerInfo()->GetMoney()) return false;

	if (false == gsys.pItemSystem->GetAdder().CheckItemAddable(pPlayer, CHECK_ADDABLE_ITEM(pRecipeInfo->m_nProductItemID, pRecipeInfo->m_nAmount))) return false;	

	return true;
}

bool GCraftSystem::Make_Apply(GEntityPlayer* pPlayer, int nProductItemID, int nAmount, int nPrice, GITEM_STACK_AMT_VEC& vecRecpItemList)
{
	VALID_RET(pPlayer, false);

	GItemData* pItemData = gmgr.pItemManager->GetItemData(nProductItemID);
	if (NULL == pItemData)
		return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();

	// 생성되는 아이템의 속성 정보가 딱히 없어서 기본값을 넣도록 함.
	const uint8		nSlotType		= SLOTTYPE_INVENTORY;	
	const uint8		nDura			= pItemData->m_nMaxDurability;
	const uint8		nMaxDura		= pItemData->m_nMaxDurability;

	const int		nColor			= 0;
	const bool		bClaimed		= false;
	const bool		bPeriodItem		= false;
	const int		nUsagePeriod	= 0;
	const int		nAddExpiMin		= -1;
	const wstring	strExpiDt		= L"NULL"; // 아직 기간제 아이템에 대한 기획이 없다. NULL이 아닐경우 "'yyyy-mm-dd hh:mm:ss'"형식으로 넣어줘야 한다. 앞자리 0은 제거.
	GItem*			pMergeItem		= pItemHolder->GetMergeableItem(SLOTTYPE_INVENTORY, nProductItemID, nAmount);
	int				nInvenSlotID	= INVALID_ID;
	int64			nMergeIUID		= 0;	
	uint8			nStackAmt		= nAmount;
	
	if (NULL != pMergeItem)
	{		
		nInvenSlotID	= pMergeItem->m_nSlotID;
		nMergeIUID		= pMergeItem->m_nIUID;
		nStackAmt		+= pMergeItem->GetAmount();		
	}
	else
	{
		nInvenSlotID = pItemHolder->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);
	}

	if (INVALID_ID == nInvenSlotID )
		return false;

	if (NULL == pMergeItem)
	{
		GDBT_CRAFT_DATA data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, pPlayer->GetLevel()
			, pPlayer->GetMoney() - nPrice
			, nPrice
			, pPlayer->GetPlayerInfo()->nXP
			, nStackAmt
			, nAddExpiMin
			, pPlayer->GetInteraction().GetInteractingNPCID()
			, vecRecpItemList);

		data.m_Item.Set(pPlayer->GetCID()
			, nSlotType
			, nInvenSlotID
			, nProductItemID
			, nStackAmt
			, pPlayer->GetPlayerInfo()->nPlayTimeSec
			, 0
			, 0
			, nDura
			, nMaxDura
			, 0
			, nColor
			, bClaimed
			, bPeriodItem
			, nUsagePeriod, strExpiDt);

		return gsys.pDBManager->Craft_Insert(data);
	}
	else
	{
		GDBT_CRAFT_DATA data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, pPlayer->GetLevel()
			, pPlayer->GetMoney() - nPrice
			, nPrice
			, pPlayer->GetPlayerInfo()->nXP
			, nAmount
			, -1
			, pPlayer->GetInteraction().GetInteractingNPCID()
			, vecRecpItemList);

		data.m_Item.Build(pPlayer->GetCID(), pMergeItem);
		data.m_Item.m_nStackAmt = nStackAmt;
		data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;

		return gsys.pDBManager->Craft_Update(data);
	}
	
	return false;
}

void GCraftSystem::Make_Route(GEntityPlayer* pPlayer, int nRecipeID)
{
	MCommand* pNewCmd = MakeNewCommand(MC_CRAFT_MAKE, 
		1, 
		NEW_INT(nRecipeID));

	pPlayer->RouteToMe(pNewCmd);
}

float GCraftSystem::CalcMakeMod(GEntityPlayer* pPlayer, const GCraftInfo* pCraftInfo)
{
	if (NULL == pPlayer) return 1.0f;
	if (NULL == pCraftInfo) return 1.0f;

	uint16 nFactionQuantity = pPlayer->GetPlayerFactions().GetQuantity(pCraftInfo->m_nFactionID);
	FACTION_RELATION_TYPE nFRT = CSFactionCalculator::CalculRelation(nFactionQuantity);
	float fMakeMod = pCraftInfo->m_fBaseMakeMod * CSFactionCalculator::GetCraftMakeMod(nFRT);

	return fMakeMod;
}

bool GCraftSystem::InsertRecipe(GEntityPlayer* pPlayer, int nRecipeID)
{
	if (NULL == pPlayer) return false;
	if (false == InsertRecipe_Check(pPlayer, nRecipeID)) return false;
	if (false == InsertRecipe_Apply(pPlayer, nRecipeID)) return false;

	InsertRecipe_Route(pPlayer, nRecipeID);

	return true;
}

bool GCraftSystem::InsertRecipe_Check(GEntityPlayer* pPlayer, int nRecipeID)
{
	if (true == pPlayer->GetPlayerRecipe().IsExist(nRecipeID)) return false;
	return true;
}

bool GCraftSystem::InsertRecipe_Apply( GEntityPlayer* pPlayer, int nRecipeID )
{
	GDBT_RECIPE data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), nRecipeID);

	if (false == gsys.pDBManager->Craft_RecpInsert(data))
	{
		return false;
	}

	pPlayer->GetPlayerRecipe().Insert(nRecipeID);

	return true;
}

void GCraftSystem::InsertRecipe_Route(GEntityPlayer* pPlayer, int nRecipeID)
{
	MCommand* pNewCmd = MakeNewCommand(MC_CRAFT_RECIPE_INSERT, 
		1, 
		NEW_INT(nRecipeID));

	pPlayer->RouteToMe(pNewCmd);
}

bool GCraftSystem::DeleteRecipe(GEntityPlayer* pPlayer, int nRecipeID)
{
	if (NULL == pPlayer) return false;
	if (false == DeleteRecipe_Check(pPlayer, nRecipeID)) return false;
	if (false == DeleteRecipe_Apply(pPlayer, nRecipeID)) return false;

	DeleteRecipe_Route(pPlayer, nRecipeID);

	return true;
}

bool GCraftSystem::DeleteRecipe_Check( GEntityPlayer* pPlayer, int nRecipeID )
{
	if (false == pPlayer->GetPlayerRecipe().IsExist(nRecipeID)) return false;
	return true;
}

bool GCraftSystem::DeleteRecipe_Apply( GEntityPlayer* pPlayer, int nRecipeID )
{
	GDBT_RECIPE data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), nRecipeID);

	if (false == gsys.pDBManager->Craft_RecpDelete(data))
	{
		return false;
	}

	pPlayer->GetPlayerRecipe().Delete(nRecipeID);

	return true;
}

void GCraftSystem::DeleteRecipe_Route( GEntityPlayer* pPlayer, int nRecipeID )
{
	MCommand* pNewCmd = MakeNewCommand(MC_CRAFT_RECIPE_DELETE, 
		1, 
		NEW_INT(nRecipeID));

	pPlayer->RouteToMe(pNewCmd);
}