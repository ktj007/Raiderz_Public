#include "stdafx.h"
#include "GEntityPlayer.h"
#include "SUnitTestUtil.h"
#include "MockLink.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GCraftInfo.h"
#include "GInteractionSystem.h"
#include "CCommandTable.h"
#include "GRecipeInfo.h"
#include "GCraftSystem.h"
#include "GPlayerInteraction.h"
#include "GPlayerTalent.h"
#include "GTestWrapper.h"
#include "GItemManager.h"
#include "GTalentInfoMgr.h"
#include "GDBManager.h"
#include "GItemRemover.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "FBaseMockLink.h"
#include "GNPCInteractor.h"
#include "GTestForward.h"
#include "GPlayerFactions.h"
#include "CSFactionCalculator.h"
#include "GSTLUtil.h"
#include "GPlayerRecipe.h"
#include "GItemHolder.h"

SUITE(CraftSystem)
{	
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{			
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);		
			m_pNPC = m_Helper.NewEntityNPC(m_pField);
			m_pNPC->GetNPCStatus().nAttackable = NAT_NONE;

			m_pProductItemData = m_Helper.NewItemData();
			m_pRecipeInfo = m_Helper.NewRecipeInfo();
			m_pRecipeInfo->m_nProductItemID = m_pProductItemData->m_nID;
			m_pOptionToolItem = m_Helper.GiveNewItem(m_pPlayer, 0, 2);
			m_pMat1Item = m_Helper.GiveNewItem(m_pPlayer, 0, 5);
			m_pRecipeInfo->m_vecMaterial.push_back(RecipeMaterial(m_pMat1Item->m_pItemData->m_nID, m_pMat1Item->GetAmount() - 1));
			m_pMat2Item = m_Helper.GiveNewItem(m_pPlayer, 0, 5);
			m_pRecipeInfo->m_vecMaterial.push_back(RecipeMaterial(m_pMat2Item->m_pItemData->m_nID, m_pMat2Item->GetAmount() - 1));			
			m_pMat3Item = m_Helper.GiveNewItem(m_pPlayer, 0, 5);
			m_pRecipeInfo->m_vecMaterial.push_back(RecipeMaterial(m_pMat3Item->m_pItemData->m_nID, m_pMat3Item->GetAmount() - 1));
			m_pMat4Item = m_Helper.GiveNewItem(m_pPlayer, 0, 5);
			m_pRecipeInfo->m_vecMaterial.push_back(RecipeMaterial(m_pMat4Item->m_pItemData->m_nID, m_pMat4Item->GetAmount() - 1));
			m_pMat5Item = m_Helper.GiveNewItem(m_pPlayer, 0, 5);
			m_pRecipeInfo->m_vecMaterial.push_back(RecipeMaterial(m_pMat5Item->m_pItemData->m_nID, m_pMat5Item->GetAmount() - 1));

			m_pCraftInfo = m_Helper.NewCraftInfo();
			m_pCraftInfo->m_setRecipeID.insert(m_pRecipeInfo->m_nID);
			m_pCraftInfo->m_fBaseMakeMod = 1.0f;

			m_pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo());
			m_pIElementInfo->m_nType = IT_CRAFT;
			m_pIElementInfo->m_vecAct.push_back(m_pCraftInfo->m_nID);

			m_pRecipeInfo->m_nPrice = 10;
			m_pPlayer->GetPlayerInfo()->SetMoney(100);

			gsys.pInteractionSystem->GetNPCInteractor().InteractionByClick(m_pPlayer, m_pNPC);

			CHECK_EQUAL(true, gsys.pInteractionSystem->GetNPCInteractor().InteractionElement(m_pPlayer, m_pNPC, m_pIElementInfo->m_nID));
		}

		virtual ~Fixture()
		{
			m_pField->Destroy();
		}

		GUTHelper m_Helper;
		MockField* m_pField;
		GEntityPlayer* m_pPlayer;
		GEntityNPC* m_pNPC;
		GCraftInfo* m_pCraftInfo;
		GItemData*	m_pProductItemData;
		GNPCInteractionElementInfo* m_pIElementInfo;

		GRecipeInfo*	m_pRecipeInfo;
		GItem*			m_pOptionToolItem;
		GItem*			m_pMat1Item;
		GItem*			m_pMat2Item;
		GItem*			m_pMat3Item;
		GItem*			m_pMat4Item;
		GItem*			m_pMat5Item;

		GTestMgrWrapper<GItemManager>		m_ItemMgr;
	};

	TEST_FIXTURE(Fixture, Show)
	{
		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(true, gsys.pCraftSystem->Show(m_pPlayer, m_pCraftInfo->m_nID));

		vector<TD_CRAFT_RECIPE> vecCraftRecipe;
		pLink->GetCommand(0).GetBlob(vecCraftRecipe, 0);
		CHECK_EQUAL(m_pRecipeInfo->m_nID, vecCraftRecipe[0].m_nRecipeID);
		CHECK_EQUAL(true, vecCraftRecipe[0].m_bMakable);

		uint16 nFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pCraftInfo->m_nFactionID);
		FACTION_RELATION_TYPE nFRT = CSFactionCalculator::CalculRelation(nFactionQuantity);
		float fMakeMod = m_pCraftInfo->m_fBaseMakeMod * CSFactionCalculator::GetCraftMakeMod(nFRT);
		CHECK_EQUAL(fMakeMod,  pLink->GetParam<float>(0, 1));
	}

	// 재료 아이템 1의 갯수가 부족한 경우
	TEST_FIXTURE(Fixture, Make_ShortMaterial1)
	{
		gsys.pItemSystem->GetRemover().Remove(m_pPlayer, m_pMat1Item->m_pItemData->m_nID, 
			m_pMat1Item->GetAmount() - m_pRecipeInfo->m_vecMaterial[0].m_nAmount + 1);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	// 재료 아이템 2의 갯수가 부족한 경우
	TEST_FIXTURE(Fixture, Make_ShortMaterial2)
	{
		gsys.pItemSystem->GetRemover().Remove(m_pPlayer, m_pMat2Item->m_pItemData->m_nID,
			m_pMat2Item->GetAmount() - m_pRecipeInfo->m_vecMaterial[1].m_nAmount + 1);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	// 재료 아이템 3의 갯수가 부족한 경우
	TEST_FIXTURE(Fixture, Make_ShortMaterial3)
	{
		gsys.pItemSystem->GetRemover().Remove(m_pPlayer, m_pMat3Item->m_pItemData->m_nID,
			m_pMat3Item->GetAmount() - m_pRecipeInfo->m_vecMaterial[2].m_nAmount + 1);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	// 재료 아이템 4의 갯수가 부족한 경우
	TEST_FIXTURE(Fixture, Make_ShortMaterial4)
	{
		gsys.pItemSystem->GetRemover().Remove(m_pPlayer, m_pMat4Item->m_pItemData->m_nID,
			m_pMat4Item->GetAmount() - m_pRecipeInfo->m_vecMaterial[3].m_nAmount + 1);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	// 재료 아이템 5의 갯수가 부족한 경우
	TEST_FIXTURE(Fixture, Make_ShortMaterial5)
	{
		gsys.pItemSystem->GetRemover().Remove(m_pPlayer, m_pMat5Item->m_pItemData->m_nID,
			m_pMat5Item->GetAmount() - m_pRecipeInfo->m_vecMaterial[4].m_nAmount + 1);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	// 생산에 필요한 돈이 부족한 경우
	TEST_FIXTURE(Fixture, Make_ShortPrice)
	{
		m_pRecipeInfo->m_nPrice = 1000;
		m_pPlayer->GetPlayerInfo()->SetMoney(100);		

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, Make_Success)
	{
		MockLink* pLink = NewLink(m_pPlayer);
		pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
		pLink->AddIgnoreID(MC_NPCINTERACTION_END);

		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();

		int nBeforeMat1Count = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[0].m_nItemID);
		int nBeforeMat2Count = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[1].m_nItemID);
		int nBeforeMat3Count = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[2].m_nItemID);
		int nBeforeMat4Count = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[3].m_nItemID);
		int nBeforeMat5Count = pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[4].m_nItemID);
		int nBeforeMoney = m_pPlayer->GetPlayerInfo()->GetMoney();

		CHECK_EQUAL(true, gsys.pCraftSystem->Make(m_pPlayer, m_pCraftInfo->m_nID, m_pRecipeInfo->m_nID));			

		CHECK_EQUAL(nBeforeMat1Count - m_pRecipeInfo->m_vecMaterial[0].m_nAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[0].m_nItemID));
		CHECK_EQUAL(nBeforeMat2Count - m_pRecipeInfo->m_vecMaterial[1].m_nAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[1].m_nItemID));
		CHECK_EQUAL(nBeforeMat3Count - m_pRecipeInfo->m_vecMaterial[2].m_nAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[2].m_nItemID));
		CHECK_EQUAL(nBeforeMat4Count - m_pRecipeInfo->m_vecMaterial[3].m_nAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[3].m_nItemID));
		CHECK_EQUAL(nBeforeMat5Count - m_pRecipeInfo->m_vecMaterial[4].m_nAmount, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY, m_pRecipeInfo->m_vecMaterial[4].m_nItemID));
		CHECK_EQUAL(nBeforeMoney - m_pRecipeInfo->m_nPrice, m_pPlayer->GetPlayerInfo()->GetMoney());

		vector<GItem*> vecProductItem;
		m_pPlayer->GetItemHolder()->CollectItemByID(SLOTTYPE_INVENTORY, m_pProductItemData->m_nID, vecProductItem);
		ASSERT_EQUAL(false, vecProductItem.empty());
		CHECK_EQUAL(m_pRecipeInfo->m_nAmount, vecProductItem.front()->GetAmount());

		CHECK_EQUAL(4, pLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_UPDATE_MONEY, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetPlayerInfo()->GetMoney(), pLink->GetParam<int>(0, 0));
		CHECK_EQUAL(MC_ITEM_REMOVE, pLink->GetCommand(1).GetID());
		CHECK_EQUAL(MC_ITEM_ADD, pLink->GetCommand(2).GetID());
		CHECK_EQUAL(MC_CRAFT_MAKE, pLink->GetCommand(3).GetID());
		CHECK_EQUAL(m_pRecipeInfo->m_nID, pLink->GetParam<int>(3, 0));
	}

	TEST_FIXTURE(Fixture, InsertRecipe_AlreadyExist)
	{
		m_pPlayer->GetPlayerRecipe().Insert(m_pRecipeInfo->m_nID);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->InsertRecipe(m_pPlayer, m_pRecipeInfo->m_nID));

		CHECK_EQUAL(true, m_pPlayer->GetPlayerRecipe().IsExist(m_pRecipeInfo->m_nID));
		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, InsertRecipe_Success)
	{
		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(true, gsys.pCraftSystem->InsertRecipe(m_pPlayer, m_pRecipeInfo->m_nID));

		CHECK_EQUAL(true, m_pPlayer->GetPlayerRecipe().IsExist(m_pRecipeInfo->m_nID));
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_CRAFT_RECIPE_INSERT, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pRecipeInfo->m_nID, pLink->GetParam<int>(0, 0));
	}

	
	TEST_FIXTURE(Fixture, DeleteRecipe_NotExist)
	{
		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(false, gsys.pCraftSystem->DeleteRecipe(m_pPlayer, m_pRecipeInfo->m_nID));

		CHECK_EQUAL(0, pLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, DeleteRecipe_Success)
	{
		m_pPlayer->GetPlayerRecipe().Insert(m_pRecipeInfo->m_nID);

		MockLink* pLink = NewLink(m_pPlayer);
		CHECK_EQUAL(true, gsys.pCraftSystem->DeleteRecipe(m_pPlayer, m_pRecipeInfo->m_nID));

		CHECK_EQUAL(false, m_pPlayer->GetPlayerRecipe().IsExist(m_pRecipeInfo->m_nID));
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_CRAFT_RECIPE_DELETE, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pRecipeInfo->m_nID, pLink->GetParam<int>(0, 0));
	}
}

