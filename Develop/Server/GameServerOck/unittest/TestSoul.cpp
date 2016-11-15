#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GLootInfo.h"
#include "GSoulSystem.h"
#include "CCommandTable.h"
#include "MockLink.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "GItemHolder.h"
#include "GTestForward.h"

SUITE(SoulSystem)
{
	struct FSoulSystem : public FBaseMockLink
	{
		FSoulSystem()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC = m_Helper.NewEntityNPC(m_pField);

			m_pLootInfo = test.loot.NewLootInfo();
			m_pLootSoulInfo = test.loot.NewLootSoulInfo(m_pLootInfo, SOULT_A);
			m_pNPC->GetNPCInfo()->m_pLootInfo = m_pLootInfo;

			m_pAmuletItem = m_Helper.GiveNewItem(m_pPlayer);
			m_pAmuletItem->m_pItemData->m_nSoulType = SOULT_A;
			m_pAmuletItem->m_pItemData->m_nSoulMaxQuantity = UCHAR_MAX;
			m_pAmuletItem->m_pItemData->m_nItemSlot = ITEMSLOT_CHARM;
			m_pAmuletItem->m_pItemData->m_nArmorType = ARMOR_AMULET;

			test.item.EquipItem(m_pPlayer, m_pAmuletItem, ITEMSLOT_CHARM);
		}

		virtual ~FSoulSystem()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;		
		GEntityNPC*		m_pNPC;
		GItem*			m_pAmuletItem;

		GLootInfo*		m_pLootInfo;
		LOOT_SOUL_INFO* m_pLootSoulInfo;
	};

	namespace DrainSoul
	{
		// charm½½·Ô¿¡ Âø¿ëÇÑ ¾ÆÀÌÅÛÀÌ ¾Ö¹Ä·¿ÀÌ ¾Æ´Ò¶§
		TEST_FIXTURE(FSoulSystem, NotAmulet)
		{
			m_pAmuletItem->m_pItemData->m_nArmorType = ARMOR_ACCESSORY;
			m_pNPC->GetNPCLoot().GetDropList().DropByKill(m_pPlayer->GetCID());

			gsys.pSoulSystem->DrainSoul(m_pPlayer, m_pNPC);

			CHECK_EQUAL(0, m_pAmuletItem->m_nSoulCount);
			CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsEmpty());
		}

		// Èí¼öÀÚ°¡ Âø¿ëÇÑ ¾Ö¹Ä·¿°ú µå¶øµÈ ¿µÈ¥ÀÇ Á¾·ù°¡ Æ²¸±¶§
		TEST_FIXTURE(FSoulSystem, DifferentSoulType)
		{
			m_pLootSoulInfo->m_nType = SOULT_A;
			m_pAmuletItem->m_pItemData->m_nSoulType = SOULT_B;
			m_pNPC->GetNPCLoot().GetDropList().DropByKill(m_pPlayer->GetCID());

			gsys.pSoulSystem->DrainSoul(m_pPlayer, m_pNPC);

			CHECK_EQUAL(0, m_pAmuletItem->m_nSoulCount);
			CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsEmpty());
		}

		// ¾Ö¹Ä·¿ ¾ÆÀÌÅÛÀÇ ÃÖ´ë ¿µÈ¥ Èí¼ö·®º¸´Ù ´õ ¸¹ÀÌ Èí¼ö ÇÒ¼ö ¾ø´Ù.
		TEST_FIXTURE(FSoulSystem, MaxSoulQuantity)
		{
			m_pAmuletItem->m_pItemData->m_nSoulMaxQuantity = 10;
			m_pAmuletItem->m_nSoulCount = 8;
			int nDropSoulQuantity = 3;
			m_pLootSoulInfo->m_nMin = nDropSoulQuantity;
			m_pLootSoulInfo->m_nMax = nDropSoulQuantity;

			m_pNPC->GetNPCLoot().GetDropList().DropByKill(m_pPlayer->GetCID());

			gsys.pSoulSystem->DrainSoul(m_pPlayer, m_pNPC);

			CHECK(m_pAmuletItem->m_pItemData->m_nSoulMaxQuantity <= m_pAmuletItem->m_nSoulCount);
			CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsEmpty());
		}

		// µå¶øµÈ ¿µÈ¥ÀÌ ¾øÀ»¶§
		TEST_FIXTURE(FSoulSystem, EmptyDropSoul)
		{
			m_pLootSoulInfo->m_fRate = 0.0f;
			m_pNPC->GetNPCLoot().GetDropList().DropByKill(m_pPlayer->GetCID());

			gsys.pSoulSystem->DrainSoul(m_pPlayer, m_pNPC);

			CHECK_EQUAL(0, m_pAmuletItem->m_nSoulCount);
			CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsEmpty());
		}		
		
		TEST_FIXTURE(FSoulSystem, Normal)
		{
			GEntityPlayer* pNeighborPlayer = GUTHelper::NewEntityPlayer(m_pField);

			int nDropSoulQuantity = 5;
			m_pLootSoulInfo->m_nMin = nDropSoulQuantity;
			m_pLootSoulInfo->m_nMax = nDropSoulQuantity;
			m_pNPC->GetNPCLoot().GetDropList().DropByKill(m_pPlayer->GetCID());

			int nBeforeSoulQuantity = m_pAmuletItem->m_nSoulCount;

			MockLink* pLink = NewLink(m_pPlayer);
			MockLink* pNeighborLink = NewLink(pNeighborPlayer);
			gsys.pSoulSystem->DrainSoul(m_pPlayer, m_pNPC);

			CHECK_EQUAL(nBeforeSoulQuantity + nDropSoulQuantity, m_pAmuletItem->m_nSoulCount);
			CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsEmpty());

			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_SOUL_DRAIN_ME, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
			CHECK_EQUAL(m_pAmuletItem->m_nSlotID, pLink->GetParam<char>(0, 1));
			CHECK_EQUAL(nDropSoulQuantity, pLink->GetParam<unsigned char>(0, 2));

			CHECK_EQUAL(1, pNeighborLink->GetCommandCount());
			CHECK_EQUAL(MC_SOUL_DRAIN_OTHER, pNeighborLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pNPC->GetUID(), pNeighborLink->GetParam<MUID>(0, 0));
			CHECK_EQUAL(m_pPlayer->GetUID(), pNeighborLink->GetParam<MUID>(0, 1));
			CHECK(m_pAmuletItem->m_pItemData->m_nSoulType == pNeighborLink->GetParam<unsigned char>(0, 2));
			CHECK_EQUAL(nDropSoulQuantity, pNeighborLink->GetParam<unsigned char>(0, 3));
		}
	}	
}