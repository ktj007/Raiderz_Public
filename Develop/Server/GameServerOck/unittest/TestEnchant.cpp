#include "StdAfx.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "GTestForward.h"
#include "FBasePlayer.h"
#include "GItemEnchant.h"
#include "GItemHolder.h"
#include "GItemUnequiper.h"
#include "GItemEquiper.h"
#include "GWorld.h"
#include "GDBCacheSystem.h"
#include "GItemManager.h"
#include "GPlayerDoing.h"
#include "GEquipmentSlot.h"

SUITE(Enchant)
{
	struct FEnchant : public FBasePlayer
	{
		FEnchant()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			pPlayer = NewEntityPlayer(m_pField);
			pItemHolder = pPlayer->GetItemHolder();
		}

		~FEnchant() 
		{
			m_pField->Destroy();
		}

		GBuffInfo* NewEnchantBuffInfo(int nBuffID=INVALID_BUFF_ID)
		{
			GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
			pBuffInfo->m_nType = BUFT_ENCHANT;
			pBuffInfo->Cooking();
			return pBuffInfo;
		}

		bool Enchant(int nBuffID=INVALID_BUFF_ID, GItem* pItem=NULL, GItem** poutEStone=NULL)
		{
			NewEnchantBuffInfo(nBuffID);
			GItem* pEStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
			pEStone->m_pItemData->m_EnchantStone.nTier = 0;

			if (poutEStone)
			{
				*poutEStone = pEStone;
			}

			return Enchant(pEStone, pItem);
		}

		bool EnchantSucess(int nBuffID=INVALID_BUFF_ID, GItem* pItem=NULL, GItem** poutEStone=NULL)
		{
			NewEnchantBuffInfo(nBuffID);
			GItem* pEStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
			pEStone->m_pItemData->m_EnchantStone.nTier = 0;
			pEStone->m_pItemData->m_EnchantStone.fSuccessRate = 1000.0f; // 강화확률 저하 무시

			if (poutEStone)
			{
				*poutEStone = pEStone;
			}

			return Enchant(pEStone, pItem);
		}

		bool Enchant(GItem* pEStone, GItem* pItem=NULL)
		{
			if (pItem == NULL)
			{
				GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
				gsys.pDBCacheSystem->DeleteItem(pPlayer->GetUID(), pItem->m_nIUID);
			}
						
			GItemEnchant enchant;
			enchant.EnchantPrepare(pPlayer);
			return enchant.Enchant(pPlayer, pEStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		}

		GItemHolder* pItemHolder;
		GEntityPlayer* pPlayer;
		MockField*		m_pField;
		GUTHelper		m_Helper;
		DECLWRAPPER_BuffMgr;
		DECLWRAPPER_ItemMgr;
	};

	TEST_FIXTURE(FEnchant, EnchatBuff_Cooking)
	{
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo();
		pBuffInfo->m_nType = BUFT_ENCHANT;
		pBuffInfo->Cooking();

		CHECK_EQUAL(BUFF_DURATION_INFINITY, pBuffInfo->m_fDuration);
		CHECK_EQUAL(false, pBuffInfo->m_bCancelable);
		CHECK_EQUAL(false, pBuffInfo->m_bSaveToDB);
		CHECK_EQUAL(false, pBuffInfo->m_bUpdateRoute);
	}

	TEST_FIXTURE(FEnchant, EnchatBuff_HasProcEffect)
	{
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo();
		pBuffInfo->m_nType = BUFT_ENCHANT;
		pBuffInfo->Cooking();

		CHECK_EQUAL(false, pBuffInfo->HasProcEffect());
		pBuffInfo->m_TriggeredBuff.nBuffID = 777;
		CHECK_EQUAL(true, pBuffInfo->HasProcEffect());
	}

	TEST_FIXTURE(FEnchant, EnchantBuff)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->Cooking();
		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItemEnchant enchant;

		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[0]);
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[1]);

		GItem* pEStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pEStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[0]);
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[1]);

		pBuffInfo->m_nType = BUFT_ENCHANT;
		pBuffInfo->Cooking();
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pEStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[0]);
		CHECK_EQUAL(pEStone->m_pItemData->m_nID, pItem->m_nEnchants[1]);
		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));

		// 액티브 강화가 된 것은 아님
		CHECK_EQUAL(false, pItem->HasActiveEnchantBuff());
	}

	TEST_FIXTURE(FEnchant, ActiveEnchantBuff)
	{
		int nBuffID = 123;
		int nBuffID2 = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		GBuffInfo* pBuffInfo2 = test.buff.NewBuffInfo(nBuffID2);
		pBuffInfo->m_nType = BUFT_ENCHANT;
		pBuffInfo->m_TriggeredBuff.nBuffID = 777;
		pBuffInfo2->m_nType = BUFT_ENCHANT;
		pBuffInfo2->m_TriggeredBuff.nBuffID = 888;
		pBuffInfo->Cooking();
		pBuffInfo2->Cooking();
		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItemEnchant enchant;

		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[0]);
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[1]);

		GItem* pStone = NULL;
		Enchant(nBuffID, pItem, &pStone);
		int nItemID1 = pStone->m_pItemData->m_nID;
		Enchant(nBuffID2, pItem, &pStone);
		int nItemID2 = pStone->m_pItemData->m_nID;
		CHECK_EQUAL(nItemID1, pItem->m_nEnchants[0]);  // overwrite 하지 않음
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[1]);

		CHECK_EQUAL(nItemID1, pItem->GetActiveEnchantBuffID());
	}

	TEST_FIXTURE(FEnchant, EnchantToInvenEquipitem)
	{
		int nBuffID = 123;
		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItemUnequiper unequipper;
		unequipper.Unequip(pPlayer, pItem);
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));

		GItemEnchant enchant;
		NewEnchantBuffInfo(nBuffID);
		GItem* pEStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pEStone->m_pItemData->m_EnchantStone.nTier = 0;
		enchant.Enchant(pPlayer, pEStone, SLOTTYPE_INVENTORY, 0, test.item.NewItem());

		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
	}

	TEST_FIXTURE(FEnchant, EnchantEffect_EquipUnequip)
	{
		int nBuffID = 123;
		Enchant(nBuffID);
		pPlayer->GetDoing().EndDoing();

		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
		GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON);
		GItemUnequiper unequipper;
		unequipper.Unequip(pPlayer, pItem);
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));

		GItemEquiper Equipper;
		Equipper.Equip(pPlayer, pItem, ITEMSLOT_RWEAPON);
		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
	}

	TEST_FIXTURE(FEnchant, EnchantEffect_SwitchingWeapon)
	{
		int nBuffID = 123;
		Enchant(nBuffID);

		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
		pPlayer->GetItemHolder()->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_SECONDARY);
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
		pPlayer->GetItemHolder()->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_PRIMARY);
		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
	}

	TEST_FIXTURE(FEnchant, Packet_MyInfo)
	{
		int nBuffID1 = 123;
		int nBuffID2 = 321;
		int nBuffID3 = 777;
		NewEnchantBuffInfo(nBuffID1);
		NewEnchantBuffInfo(nBuffID2);
		NewEnchantBuffInfo(nBuffID3);

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItemEnchant enchant;
		GItem* pStone = NULL;
		EnchantSucess(nBuffID1, pItem, &pStone);
		int nItemID1 = pStone->m_pItemData->m_nID;
		EnchantSucess(nBuffID2, pItem, &pStone);
		int nItemID2 = pStone->m_pItemData->m_nID;
		EnchantSucess(nBuffID3, pItem, &pStone);
		int nItemID3 = pStone->m_pItemData->m_nID;

		GWorld world;
		MockLink* pLink = GUTHelper::NewLink(pPlayer);				
		MUID uidPlayer = pPlayer->GetUID();
		world.SendMyInfo(uidPlayer);
		
		MCommand& cmd = pLink->GetCommand(0);
		TVALID_EQ(MC_CHAR_MYINFO, pLink->GetCommandID(0));
		vector<TD_ITEM> vecTDItems;
		pLink->GetCommand(0).GetBlob(vecTDItems, 2);
		TVALID_EQ(1, vecTDItems.size());
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, vecTDItems[0].m_nEnchants[0]);
		CHECK_EQUAL(nItemID1, vecTDItems[0].m_nEnchants[1]);
		CHECK_EQUAL(nItemID2, vecTDItems[0].m_nEnchants[2]);
		CHECK_EQUAL(nItemID3, vecTDItems[0].m_nEnchants[3]);
	}

	TEST_FIXTURE(FEnchant, Packet_LookUpdate)
	{
		GEntityPlayer* pOhterPlayer = GUTHelper::NewEntityPlayer(m_pField);		 
		MockLink* pLink = GUTHelper::NewLink(pOhterPlayer);				
				
		int nBuffID = 123;
		Enchant(nBuffID);

		TVALID_EQ(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM, pLink->GetCommandID(0));
	}

	TEST_FIXTURE(FEnchant, Packet_SectorInfo)
	{
		GEntityPlayer* pOhterPlayer = GUTHelper::NewEntityPlayer(m_pField);		 
		MockLink* pLink = GUTHelper::NewLink(pOhterPlayer);				

		int nBuffID = 123;
		Enchant(nBuffID);

		TVALID_EQ(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM, pLink->GetCommandID(0));
	}

	TEST_FIXTURE(FEnchant, Packet_Success)
	{
		MockLink* pLink = GUTHelper::NewLink(pPlayer);		
		pLink->AddIgnoreID(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM);
		pLink->AddIgnoreID(MC_TRAINING_TRAIN);
		pLink->AddIgnoreID(MC_ITEM_EQUIP);
		pLink->AddIgnoreID(MC_ITEM_REMOVE);
						
		int nBuffID = 123;
		GItem* pStone=NULL;
		Enchant(nBuffID, NULL, &pStone);
		int nItemID = pStone->m_pItemData->m_nID;

		TVALID_EQ(MC_ENCHANT_SUCCESS, pLink->GetCommandID(0));
		CHECK_EQUAL(true, pLink->GetParam<bool>(0,0)); // EquipSlot
		CHECK_EQUAL(1, pLink->GetParam<int8>(0,2)); // EnchantSlotID
		CHECK_EQUAL(nItemID, pLink->GetParam<int>(0,3)); // BuffID
	}

	TEST_FIXTURE(FEnchant, Packet_Broken)
	{
		MockLink* pLink = GUTHelper::NewLink(pPlayer);		
		pLink->AddIgnoreID(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM);
		pLink->AddIgnoreID(MC_TRAINING_TRAIN);
		pLink->AddIgnoreID(MC_ITEM_EQUIP);
		pLink->AddIgnoreID(MC_ITEM_REMOVE);
		pLink->AddIgnoreID(MC_MSG_RES);
		
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 0.0f;
		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());

		TVALID_EQ(MC_ENCHANT_BROKEN, pLink->GetCommandID(0));
		CHECK_EQUAL(true, pLink->GetParam<bool>(0,0)); // EquipSlot
		CHECK_EQUAL(1, pLink->GetParam<int8>(0,2)); // EnchantSlotID
	}

	TEST_FIXTURE(FEnchant, Packet_TryEnchant)
	{
		int nBuffID = 123;
		// Enchant
		GBuffInfo* pBuffInfo = NewEnchantBuffInfo(nBuffID);
		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		gsys.pDBCacheSystem->DeleteItem(pPlayer->GetUID(), pItem->m_nIUID);

		GItem* pTargetItem = pItemHolder->GetEquipment().GetRightWeapon();
		GItem* pEStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		GItem* pEAgent = test.item.GiveNewItem(pPlayer, nBuffID);

		MockLink* pLink = GUTHelper::NewLink(pPlayer);		
		pLink->AddIgnoreID(MC_ITEM_REMOVE);

		pLink->OnRecv(MC_ENCHANT_PREPARE,		0, NULL);
		pLink->ResetCommands();

		int nEnchantStoneSlotID = 0;
		int nEnchantAgentSlotID = 1;
		SH_ITEM_SLOT_TYPE nTargetSlotType = SLOTTYPE_EQUIP;
		int nTargetSlotID = ITEMSLOT_RWEAPON;

		pLink->OnRecv(MC_ENCHANT_REQ,		4,	
			NEW_INT(nTargetSlotType), 
			NEW_INT(nTargetSlotID),
			NEW_INT(nEnchantStoneSlotID),
			NEW_INT(nEnchantAgentSlotID)
			);
		
		TVALID_EQ(MC_ENCHANT_SUCCESS, pLink->GetCommandID(0));
	}

	TEST_FIXTURE(FEnchant, GetEnchantCount)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;
		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItemEnchant enchant;

		CHECK_EQUAL(0, pItem->GetEnchantCount());
		Enchant(nBuffID, pItem);
		CHECK_EQUAL(1, pItem->GetEnchantCount());
		Enchant(nBuffID, pItem);
		CHECK_EQUAL(2, pItem->GetEnchantCount());
	}

	TEST_FIXTURE(FEnchant, Enchant_ActiveBuff)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		CHECK_EQUAL(false, pItem->HasActiveEnchantBuff());
		pBuffInfo->m_TriggeredBuff.nBuffID = 777;
		CHECK_EQUAL(false, pItem->HasActiveEnchantBuff());

		GItemEnchant enchant;
		Enchant(nBuffID, pItem);

		CHECK_EQUAL(true, pItem->HasActiveEnchantBuff());
	}
	
	TEST_FIXTURE(FEnchant, EnchantStoneType)
	{
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, INVALID_BUFF_ID);

		CHECK_EQUAL(true, pStone->m_pItemData->IsEnchantStone());
	}

	TEST_FIXTURE(FEnchant, Enchant)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		 
		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[0]);
		CHECK_EQUAL(pStone->m_pItemData->m_nID, pItem->m_nEnchants[1]);
		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
	}

	TEST_FIXTURE(FEnchant, EnchantChance)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 0.0f;

		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
	}

	TEST_FIXTURE(FEnchant, EnchantFailed)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 0.0f;

		Enchant(pStone, pItem);
		CHECK_EQUAL(ENCHANT_UNUSED_SLOT, pItem->m_nEnchants[0]);
		CHECK_EQUAL(ENCHANT_BROKEN_SLOT, pItem->m_nEnchants[1]);
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
	}

	TEST_FIXTURE(FEnchant, EnchantFailed_IncreaseEnchantCount)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 0.0f;

		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(0, pItem->GetEnchantCount());
		enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		CHECK_EQUAL(1, pItem->GetEnchantCount());
	}

	TEST_FIXTURE(FEnchant, CalcEnchantChanceRate)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 100.0f;
		
		GItemEnchant enchant;
		CHECK_EQUAL(100.0f, enchant.CalcEnchantChanceRate(pStone, pItem)); // 인챈트 횟수: 0
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 100.0f;
		
		CHECK_EQUAL(100.0f, enchant.CalcEnchantChanceRate(pStone, pItem)); // 인챈트 횟수: 1
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem());
		pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.fSuccessRate = 100.0f;

		CHECK_EQUAL(95.0f, enchant.CalcEnchantChanceRate(pStone, pItem));  // 인챈트 횟수: 2
	}

	TEST_FIXTURE(FEnchant, Limit_Level)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.nLimitLevel = 10;

		GItemEnchant enchant;
		pPlayer->GetPlayerInfo()->nLevel = 1;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		pPlayer->GetPlayerInfo()->nLevel = 10;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));

		pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.nLimitLevel = 10;

		pPlayer->GetPlayerInfo()->nLevel = 15;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
	}

	TEST_FIXTURE(FEnchant, Limit_Equipments)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.bAllowAllEquipments = false;
		pStone->m_pItemData->m_EnchantStone.vecLimitEquipments.push_back(ITEMSLOT_HEAD);

		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		
		pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.bAllowAllEquipments = false;
		pStone->m_pItemData->m_EnchantStone.vecLimitEquipments.push_back(ITEMSLOT_HEAD);
		pStone->m_pItemData->m_EnchantStone.vecLimitEquipments.push_back(ITEMSLOT_RWEAPON);

		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		pStone->m_pItemData->m_EnchantStone.bAllowAllEquipments = true;		
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
	}

	TEST_FIXTURE(FEnchant, Limit_Weapons)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.bAllowAllWeapons = false;
		pStone->m_pItemData->m_EnchantStone.vecLimitWeapons.push_back(WEAPON_1H_BLUNT);

		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));

		pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.bAllowAllWeapons = false;
		pStone->m_pItemData->m_EnchantStone.vecLimitWeapons.push_back(WEAPON_1H_BLUNT);
		pStone->m_pItemData->m_EnchantStone.vecLimitWeapons.push_back(WEAPON_1H_SLASH);
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		pStone->m_pItemData->m_EnchantStone.bAllowAllWeapons = true;		
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
	}

	TEST_FIXTURE(FEnchant, EnchantWeaponSet)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH, ITEMSLOT_RWEAPON2);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.bAllowAllWeapons = false;
		pStone->m_pItemData->m_EnchantStone.vecLimitWeapons.push_back(WEAPON_1H_BLUNT);

		GItemHolder* pHolder = pPlayer->GetItemHolder();
		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON2, test.item.NewItem());
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
		pHolder->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_SECONDARY);
		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
		pHolder->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_PRIMARY);
		CHECK_EQUAL(false, pPlayer->HasBuff_ForTest(nBuffID));
		pHolder->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_SECONDARY);
		CHECK_EQUAL(true, pPlayer->HasBuff_ForTest(nBuffID));
	}

	TEST_FIXTURE(FEnchant, EquipEnchantSlot)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH, ITEMSLOT_RWEAPON);
		pItem->m_pItemData->m_vecEquipEnchantColor.clear();
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_NONE);
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_WHITE);
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_BLUE);
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_WHITE);
		GItem* pStoneYellow = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStoneYellow->m_pItemData->m_EnchantStone.nColor = ENCHANT_YELLOW;
		GItem* pStoneWhite = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStoneWhite->m_pItemData->m_EnchantStone.nColor = ENCHANT_WHITE;

		GItemHolder* pHolder = pPlayer->GetItemHolder();
		GItemEnchant enchant;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStoneYellow, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStoneWhite, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));

		pStoneWhite = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStoneWhite->m_pItemData->m_EnchantStone.nColor = ENCHANT_WHITE;

		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStoneWhite, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStoneWhite, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
	}

	TEST_FIXTURE(FEnchant, GetEmptyEnchantSlotIdx)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH, ITEMSLOT_RWEAPON2);
		pItem->m_pItemData->m_vecEquipEnchantColor.clear();
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_NONE);
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_WHITE);
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_BLUE);
		pItem->m_pItemData->m_vecEquipEnchantColor.push_back(ENCHANT_BLUE);

		CHECK_EQUAL(2, pItem->GetEmptyEnchantSlotIdx(false, ENCHANT_BLUE));
		int nItemID = 777;
		pItem->Enchant(pBuffInfo, nItemID, ENCHANT_BLUE);
		CHECK_EQUAL(3, pItem->GetEmptyEnchantSlotIdx(false, ENCHANT_BLUE));
	}

	TEST_FIXTURE(FEnchant, GetAgentItemID)
	{
		//static int ENCHANT_AGENT_ITEMID[5] =
		//{
		//	39999000 //저급강화제
		//	, 39999001 //일반강화제
		//	, 39999002 //상급강화제
		//	, 39999003 //최상급강화제
		//	, 39999004 //특급강화제
		//};

		GItemEnchant enchant;
		CHECK_EQUAL(INVALID_ID, enchant.GetAgentItemID(0));
		CHECK_EQUAL(39999000, enchant.GetAgentItemID(1));
		CHECK_EQUAL(39999002, enchant.GetAgentItemID(3));
		CHECK_EQUAL(39999004, enchant.GetAgentItemID(5));
		CHECK_EQUAL(INVALID_ID, enchant.GetAgentItemID(7));
	}

	TEST_FIXTURE(FEnchant, EnchantAgent)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH, ITEMSLOT_RWEAPON);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);

		GItemEnchant enchant;
		pStone->m_pItemData->m_EnchantStone.nTier = 0;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
		pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.nTier = 1;
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, test.item.NewItem()));
	}

	TEST_FIXTURE(FEnchant, EnchantAgent_UsageByHigherTier)
	{
		int nBuffID = 123;
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo(nBuffID);
		pBuffInfo->m_nType = BUFT_ENCHANT;

		GItem* pItem = test.item.EquipNewWeapon(pPlayer, WEAPON_1H_SLASH, ITEMSLOT_RWEAPON);
		GItem* pStone = test.item.NewEnchantStoneItem(pPlayer, nBuffID);
		pStone->m_pItemData->m_EnchantStone.nTier = 2;
		GItem* pEAgent = test.item.GiveNewItem(pPlayer, nBuffID);
						
		GItemEnchant enchant;
		pEAgent->m_pItemData->m_nID = ENCHANT_AGENT_ITEMID[0]; // 1티어 강화제
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(false, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, pEAgent));
		pEAgent->m_pItemData->m_nID = ENCHANT_AGENT_ITEMID[2]; // 3티어 강화제
		enchant.EnchantPrepare(pPlayer);
		CHECK_EQUAL(true, enchant.Enchant(pPlayer, pStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, pEAgent));
	}

	TEST_FIXTURE(FEnchant, Cooking_EStoneTier)
	{
		GItem* pItem = test.item.NewEnchantStoneItem(pPlayer);
		
		pItem->m_pItemData->m_EnchantStone.nLimitLevel = 0;
		pItem->m_pItemData->Cooking();
		CHECK_EQUAL(0, pItem->m_pItemData->m_EnchantStone.nTier);

		pItem->m_pItemData->m_EnchantStone.nLimitLevel = 1;
		pItem->m_pItemData->Cooking();
		CHECK_EQUAL(1, pItem->m_pItemData->m_EnchantStone.nTier);

		pItem->m_pItemData->m_EnchantStone.nLimitLevel = 10;
		pItem->m_pItemData->Cooking();
		CHECK_EQUAL(1, pItem->m_pItemData->m_EnchantStone.nTier);

		pItem->m_pItemData->m_EnchantStone.nLimitLevel = 11;
		pItem->m_pItemData->Cooking();
		CHECK_EQUAL(2, pItem->m_pItemData->m_EnchantStone.nTier);

		pItem->m_pItemData->m_EnchantStone.nLimitLevel = 25;
		pItem->m_pItemData->Cooking();
		CHECK_EQUAL(3, pItem->m_pItemData->m_EnchantStone.nTier);

		pItem->m_pItemData->m_EnchantStone.nLimitLevel = 57;
		pItem->m_pItemData->Cooking();
		CHECK_EQUAL(6, pItem->m_pItemData->m_EnchantStone.nTier);
	}
}