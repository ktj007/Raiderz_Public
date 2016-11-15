#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskTrade.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_DBTaskTrade)
	{
		class Trader
		{
		public :
			GEntityPlayer*		m_pPlayer;
			GItem*				m_pItem1;
			GItem*				m_pItem2;		

			GCharacterDBCache*	m_pch;
			GItemDBCache*		m_pi1;
			GItemDBCache*		m_pi2;
		};

		class FIXTURE_TRADE : public FIXTURE
		{
		public :
			Trader CreateTestTrader(const int nMoney, const int16 nStackAmt)
			{
				static CID	nCID = 1;

				Trader t;
				
				t.m_pPlayer = CreateTestPlayer(nCID++);
				t.m_pItem1	= GiveTestItem(t.m_pPlayer);
				t.m_pItem2	= GiveTestItem(t.m_pPlayer);

				t.m_pPlayer->GetPlayerInfo()->SetMoney(nMoney);
				t.m_pPlayer->GetItemHolder()->SetAmount(t.m_pItem1->m_nSlotType, t.m_pItem1->m_nSlotID, nStackAmt);
				t.m_pPlayer->GetItemHolder()->SetAmount(t.m_pItem2->m_nSlotType, t.m_pItem2->m_nSlotID, nStackAmt);

				t.m_pch		= gsys.pDBCacheSystem->FindCharCache(t.m_pPlayer->GetUID());
				t.m_pi1		= gsys.pDBCacheSystem->FindItemCache(t.m_pPlayer->GetUID(), t.m_pItem1->m_nSlotType, t.m_pItem1->m_nSlotID, t.m_pItem1->m_nIUID);
				t.m_pi2		= gsys.pDBCacheSystem->FindItemCache(t.m_pPlayer->GetUID(), t.m_pItem2->m_nSlotType, t.m_pItem2->m_nSlotID, t.m_pItem2->m_nIUID);

				return t;
			}

			GTRADE_CHAR_VEC CreateTradeData(Trader& t1, Trader& t2
				, const int nTradeMoney1, const int nTradeMoney2
				, const int16 nTradeAmt1, const int16 nTradeAmt2)
			{
				GTRADE_CHAR_VEC vec;

				GTRADE_CHAR tc1(0, t1.m_pPlayer->GetUID(), t1.m_pPlayer->GetCID(), 0
					, t1.m_pPlayer->GetMoney() - nTradeMoney1 + nTradeMoney2, nTradeMoney1
					, 0, 0, 0, 0, 0, 0, 0);

				GTRADE_CHAR tc2(0, t2.m_pPlayer->GetUID(), t2.m_pPlayer->GetCID(), 0
					, t2.m_pPlayer->GetMoney() - nTradeMoney2 + nTradeMoney1, nTradeMoney2
					, 0, 0, 0, 0, 0, 0, 0);

				GTRADE_ITEM ti11(t1.m_pItem1->m_nSlotID, 10, t1.m_pItem1->m_pItemData->m_nID, t1.m_pItem1->m_nIUID, 0, t1.m_pItem1->GetAmount()
					, 0, 0, 0, 0, 0, 0, 0);
				GTRADE_ITEM ti12(t1.m_pItem2->m_nSlotID, 11, t1.m_pItem2->m_pItemData->m_nID, t1.m_pItem2->m_nIUID, t1.m_pItem2->GetAmount() - nTradeAmt1, nTradeAmt1
					, 0, 0, 0, 0, 0, 0, 0);

				tc1.m_vItem[0] = ti11;
				tc1.m_vItem[1] = ti12;
				
				GTRADE_ITEM ti21(t2.m_pItem1->m_nSlotID, 10, t2.m_pItem1->m_pItemData->m_nID, t2.m_pItem1->m_nIUID, t2.m_pItem1->GetAmount() - nTradeAmt2, nTradeAmt2
					, 0, 0, 0, 0, 0, 0, 0);
				GTRADE_ITEM ti22(t2.m_pItem2->m_nSlotID, 11, t2.m_pItem2->m_pItemData->m_nID, t2.m_pItem2->m_nIUID, 0, t2.m_pItem2->GetAmount()
					, 0, 0, 0, 0, 0, 0, 0);

				tc2.m_vItem[0] = ti21;
				tc2.m_vItem[1] = ti22;

				vec.push_back(tc1);
				vec.push_back(tc2);

				return vec;
			}

			void CHECK_TRADER(Trader& t, const int nMoney, const bool bSyncing, const DWORD nDBTaskRefCount)
			{
				CHECK_EQUAL(nMoney, t.m_pPlayer->GetMoney());
				CHECK_EQUAL(bSyncing, t.m_pch->IsSyncing());				
				CHECK_EQUAL(nDBTaskRefCount, t.m_pPlayer->GetDBAsyncMonitor().GetRefCount());
			}
		};

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncSuccess)
		{
			MockDBManager	db;
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			CHECK(t1.m_pPlayer->GetUID() != t2.m_pPlayer->GetUID());
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			const IUID nPlayer1_MoveIUID = t1.m_pItem1->m_nIUID;
			const IUID nPlayer2_MoveIUID = t2.m_pItem2->m_nIUID;

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			GDBTaskTrade* pTask = new GDBTaskTrade;	

			CHECK(pTask->Input(vtc));

			CHECK_TRADER(t1, nMoney, true, 1);
			CHECK_TRADER(t2, nMoney, true, 1);		

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, true);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, true);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, true);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, true);

			CHECK(db.Post(pTask));

			CHECK_TRADER(t1, nMoney - nTradeMoney1 + nTradeMoney2, false, 0);
			CHECK_TRADER(t2, nMoney - nTradeMoney2 + nTradeMoney1, false, 0);	
			CHECK_CHAR_END_CACHE(t1.m_pch);
			CHECK_CHAR_END_CACHE(t2.m_pch);

			CHECK(NULL == gsys.pDBCacheSystem->FindPlayerCache(t1.m_pPlayer->GetUID())->GetItemCcMgr().Find(nPlayer1_MoveIUID));
			CHECK_ITEM_END_CACHE(t1.m_pi2);
			CHECK_ITEM_END_CACHE(t2.m_pi1);
			CHECK(NULL == gsys.pDBCacheSystem->FindPlayerCache(t2.m_pPlayer->GetUID())->GetItemCcMgr().Find(nPlayer2_MoveIUID));			
		}

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncFailFirstPlayer)
		{
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			t1.m_pch->BeginCacheSync();

			GDBTaskTrade t;	
			CHECK(!t.Input(vtc));

			CHECK_TRADER(t1, nMoney, true, 0);
			CHECK_TRADER(t2, nMoney, false, 0);

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, false);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, false);			
		}

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncFailSecondPlayer)
		{
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			t2.m_pch->BeginCacheSync();

			GDBTaskTrade t;	
			CHECK(!t.Input(vtc));

			CHECK_TRADER(t1, nMoney, false, 0);
			CHECK_TRADER(t2, nMoney, true, 0);

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, false);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, false);	
		}

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncFailFirstPlayerFirstItem)
		{
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			t1.m_pi1->BeginCacheSync();

			GDBTaskTrade t;	
			CHECK(!t.Input(vtc));

			CHECK_TRADER(t1, nMoney, false, 0);
			CHECK_TRADER(t2, nMoney, false, 0);

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, true);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, false);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, false);			
		}

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncFailFirstPlayerSecondItem)
		{
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			t1.m_pi2->BeginCacheSync();

			GDBTaskTrade t;	
			CHECK(!t.Input(vtc));

			CHECK_TRADER(t1, nMoney, false, 0);
			CHECK_TRADER(t2, nMoney, false, 0);

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, true);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, false);			
		}

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncFailSecondPlayerFirstItem)
		{
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			t2.m_pi1->BeginCacheSync();

			GDBTaskTrade t;	
			CHECK(!t.Input(vtc));

			CHECK_TRADER(t1, nMoney, false, 0);
			CHECK_TRADER(t2, nMoney, false, 0);

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, false);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, true);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, false);			
		}

		TEST_FIXTURE(FIXTURE_TRADE, TestTradeSyncFailSecondPlayerSecondItem)
		{
			const int		nMoney			= 10;
			const int16		nStackAmt		= 100;
			const int		nTradeMoney1	= 1;
			const int		nTradeMoney2	= 2;
			const int16		nTradeAmt1		= 1;
			const int16		nTradeAmt2		= 2;
			Trader			t1				= CreateTestTrader(nMoney, nStackAmt);
			Trader			t2				= CreateTestTrader(nMoney, nStackAmt);
			GTRADE_CHAR_VEC vtc				= CreateTradeData(t1, t2, nTradeMoney1, nTradeMoney2, nTradeAmt1, nTradeAmt2);

			SetTestCharCc(t1.m_pch);
			SetTestCharCc(t2.m_pch);

			SetTestItemCc(t1.m_pi1);
			SetTestItemCc(t1.m_pi2);

			SetTestItemCc(t2.m_pi1);
			SetTestItemCc(t2.m_pi2);

			t2.m_pi2->BeginCacheSync();

			GDBTaskTrade t;	
			CHECK(!t.Input(vtc));

			CHECK_TRADER(t1, nMoney, false, 0);
			CHECK_TRADER(t2, nMoney, false, 0);

			CHECK_ITEM_BEGIN_CACHE(t1.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t1.m_pi2, false);

			CHECK_ITEM_BEGIN_CACHE(t2.m_pi1, false);
			CHECK_ITEM_BEGIN_CACHE(t2.m_pi2, true);			
		}
	}
}

