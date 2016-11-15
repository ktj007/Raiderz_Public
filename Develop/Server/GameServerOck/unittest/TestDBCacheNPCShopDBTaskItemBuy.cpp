#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemBuyInsert.h"
#include "GDBTaskItemBuyUpdate.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_NPCShopItemBuy)
	{
		class MockDBTaskItemBuyInsert : public GDBTaskItemBuyInsert 
		{
		public :
			MockDBTaskItemBuyInsert(const MUID& uidPlayer) : GDBTaskItemBuyInsert(uidPlayer) { SetIUID(); }
			void SetIUID()
			{
				m_Result.nIUID = 1;
			}
		};

		TEST_FIXTURE(FIXTURE, TestItemBuyInsertSyncSuccess)
		{
			MockDBManager	db;
			const CID		nCID		= 2;
			const int		nMoney		= 3;
			const int		nItemID		= GUTHelper_Item::NewItemData()->m_nID;	
			GEntityPlayer*	pPlayer		= CreateTestPlayer(nCID, 0, 0);
			const MUID		uidPlayer	= pPlayer->GetUID();

			GDBT_NPC_SHOP_TRADE_DATA data;
			data.m_uidPlayer		= uidPlayer;
			data.m_nMoney			= nMoney;
			data.m_Item.m_nItemID	= nItemID;
			data.m_Item.m_nStackAmt	= 1;
			data.m_Item.m_nSlotType	= SLOTTYPE_INVENTORY;
			data.m_Item.m_nSlotID	= 1;

			MockDBTaskItemBuyInsert* pTask = new MockDBTaskItemBuyInsert(uidPlayer);

			CHECK(pTask->Input(data));
			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(db.Post(pTask));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(0, gsys.pDBCacheSystem->FindCharCache(uidPlayer)->GetMoney().GetIncrease());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());		
			CHECK(NULL != pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, 1));
		}

		TEST_FIXTURE(FIXTURE, TestItemBuyInsertSyncFail)
		{
			const CID		nCID			= 2;
			const int		nMoney			= 3;
			const int		nItemID			= GUTHelper_Item::NewItemData()->m_nID;	
			GEntityPlayer*	pPlayer			= CreateTestPlayer(nCID, 0, 0);
			const MUID		uidPlayer		= pPlayer->GetUID();

			// 동기화 실패를 하기 위해 먼저 동기화 진행중으로 표시한다.
			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->BeginCacheSync();

			MockDBTaskItemBuyInsert t(uidPlayer);

			GDBT_NPC_SHOP_TRADE_DATA data;
			data.m_uidPlayer		= uidPlayer;
			data.m_nMoney			= nMoney;
			data.m_Item.m_nItemID	= nItemID;
			data.m_Item.m_nStackAmt	= 1;
			data.m_Item.m_nSlotType	= SLOTTYPE_INVENTORY;
			data.m_Item.m_nSlotID	= 1;

			CHECK(!t.Input(data));

			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(0, pPlayer->GetMoney());		
		}

		class FIXTURE_ITEM_BUY_UPDATE : public FIXTURE
		{
		public :
			GDBT_NPC_SHOP_TRADE_DATA CreateTaskData(GEntityPlayer* pPlayer, GItem* pItem,  const int nDecMoney, const int16 nIncStackAmt)
			{
				GDBT_NPC_SHOP_TRADE_DATA data;
				data.m_uidPlayer		= pPlayer->GetUID();
				data.m_Item.m_nIUID		= pItem->m_nIUID;
				data.m_Item.m_nSlotType	= pItem->m_nSlotType;
				data.m_Item.m_nSlotID	= pItem->m_nSlotID;
				data.m_nMoney			= pPlayer->GetMoney() - nDecMoney;
				data.m_nModMoney		= nDecMoney;
				data.m_Item.m_nStackAmt	= pItem->GetAmount() + nIncStackAmt;
				data.m_nModStackAmt		= nIncStackAmt;

				return data;
			}
		};

		TEST_FIXTURE(FIXTURE_ITEM_BUY_UPDATE, TestItemBuyUpdateSyncSuccess)
		{
			MockDBManager				db;
			const int					nDecMoney		= 2;
			const int16					nIncStackAmt	= 1;
			const int					nXP				= 30;
			const uint8					nDura			= 10;
			GEntityPlayer*				pPlayer			= CreateTestPlayer();
			GItem*						pItem			= GiveTestItem(pPlayer);
			GCharacterDBCache*			pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*				pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA			data	= CreateTaskData(pPlayer, pItem, nDecMoney, nIncStackAmt);

			// 캐시 초기화 되는지 확인하기 위해서.
			pch->IncXP(1, false);
			pi->IncDura(1, false);

			GDBTaskItemBuyUpdate* pTask = new GDBTaskItemBuyUpdate(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			CHECK(pch->IsSyncing());
			CHECK(pi->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK(!pi->IsSyncing());
			CHECK_EQUAL(0, pch->GetMoney().GetIncrease());
			CHECK_EQUAL(0, pi->GetDura().GetIncrease());			
			CHECK_EQUAL(data.m_nMoney, pPlayer->GetMoney());		
			CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());			
		}

		TEST_FIXTURE(FIXTURE_ITEM_BUY_UPDATE, TestItemBuyUpdateSyncFailCharacter)
		{
			const CID					nCID			= 2;
			const int					nMoney			= 5;
			const int					nDecMoney		= 2;
			const int					nItemID			= GUTHelper_Item::NewItemData()->m_nID;	
			const int16					nStackAmt		= 5;
			const int16					nIncStackAmt	= 1;
			const int					nXP				= 10;
			const uint8					nDura			= 19;
			GEntityPlayer*				pPlayer			= CreateTestPlayer(nCID, 0, nMoney);
			const MUID					uidPlayer		= pPlayer->GetUID();
			GItem*						pItem			= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GCharacterDBCache*			pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*				pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA			data			= CreateTaskData(pPlayer, pItem, nDecMoney, nIncStackAmt);
			
			// 동기화 실패를 하기 위해 먼저 동기화 진행중으로 표시한다.
			pch->IncXP(1, false);
			pi->IncDura(1, false);
			pch->BeginCacheSync();

			GDBTaskItemBuyUpdate t(uidPlayer);

			CHECK(!t.Input(data));
			
			CHECK(pch->IsSyncing());
			CHECK(!pi->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());		
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi->GetDura().GetIncrease());			
		}

		TEST_FIXTURE(FIXTURE_ITEM_BUY_UPDATE, TestItemBuyUpdateSyncFailItem)
		{
			const CID					nCID			= 2;
			const int					nMoney			= 5;
			const int					nDecMoney		= 2;
			const int					nItemID			= GUTHelper_Item::NewItemData()->m_nID;	
			const int16					nStackAmt		= 5;
			const int16					nIncStackAmt	= 1;
			const int					nXP				= 123;
			GEntityPlayer*				pPlayer			= CreateTestPlayer(nCID, 0, nMoney);
			const MUID					uidPlayer		= pPlayer->GetUID();
			GItem*						pItem			= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GCharacterDBCache*			pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*				pi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA			data			= CreateTaskData(pPlayer, pItem, nDecMoney, nIncStackAmt);

			// 동기화 실패를 하기 위해 먼저 동기화 진행중으로 표시한다.
			pch->IncXP(1, false);
			pi->IncDura(1, false);
			pi->BeginCacheSync();

			GDBTaskItemBuyUpdate t(uidPlayer);

			CHECK(!t.Input(data));

			CHECK(!pch->IsSyncing());
			CHECK(pi->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());		
			CHECK_EQUAL(nStackAmt, pItem->GetAmount());
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());			
			CHECK_EQUAL(1, pi->GetDura().GetIncrease());
		}
	}
}