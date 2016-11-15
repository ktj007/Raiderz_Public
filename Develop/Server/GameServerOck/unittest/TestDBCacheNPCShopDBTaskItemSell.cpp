#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemSellDelete.h"
#include "GDBTaskItemSellUpdate.h"
#include "GDBTaskItemSellVeryCommon.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"
#include "GDBTaskNPCShopData.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_NPCShopItemSell)
	{
		GDBT_NPC_SHOP_TRADE_DATA CreateSellItemDeleteData(GEntityPlayer* pPlayer, GItem* pItem, const int nIncMoney, const int16 nDecStackAmt)
		{
			GDBT_NPC_SHOP_TRADE_DATA data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()
				, pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() + nIncMoney
				, nIncMoney
				, nDecStackAmt
				, -1);

			data.m_Item.Build(pPlayer->GetCID(), pItem);
			data.m_Item.m_nStackAmt = 0;
			data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;

			return data;
		}		

		GDBT_NPC_SHOP_TRADE_DATA CreateSellItemData(GEntityPlayer* pPlayer, GItem* pItem, const int nIncMoney, const int16 nDecStackAmt)
		{
			GDBT_NPC_SHOP_TRADE_DATA data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()
				, pPlayer->GetPlayerInfo()->nXP
				, pPlayer->GetMoney() + nIncMoney
				, nIncMoney
				, nDecStackAmt
				, -1);

			data.m_Item.Build(pPlayer->GetCID(), pItem);
			data.m_Item.m_nStackAmt -= nDecStackAmt;
			data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestItemSellDeleteSyncSuccess)
		{
			MockDBManager			db;
			const int				nMoney		= 100;
			const int				nIncMoney	= 10;
			const CID				nCID		= 2;
			const int				nItemID		= 3;
			const int16				nAmt		= 2;
			GEntityPlayer*			pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID				uidPlayer	= pPlayer->GetUID();
			GItem*					pItem		= GiveTestItem(pPlayer, nItemID, nAmt);
			const IUID				nIUID		= pItem->m_nIUID;
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(uidPlayer);
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA		data		= CreateSellItemDeleteData(pPlayer, pItem, nIncMoney, nAmt);
			
			GDBTaskItemSellDelete* pTask = new GDBTaskItemSellDelete(uidPlayer);

			CHECK(pTask->Input(data));
			CHECK(pch->IsSyncing());
			CHECK(pi->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK_EQUAL(data.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(0, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));
			CHECK_EQUAL(0, pch->GetMoney().GetDecrease());
			CHECK_EQUAL(0, gsys.pDBCacheSystem->GetItemCount(uidPlayer));
		}

		TEST_FIXTURE(FIXTURE, TestItemSellDeleteSyncFailCharacter)
		{
			const int				nMoney		= 100;
			const int				nIncMoney	= 10;
			const CID				nCID		= 2;
			const int				nItemID		= 3;
			const int16				nAmt		= 2;
			GEntityPlayer*			pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID				uidPlayer	= pPlayer->GetUID();
			GItem*					pItem		= GiveTestItem(pPlayer, nItemID, nAmt);
			GDBT_NPC_SHOP_TRADE_DATA		data		= CreateSellItemDeleteData(pPlayer, pItem, nIncMoney, nAmt);
			
			GDBTaskItemSellDelete t(uidPlayer);

			// 실패하기 위해 강제로 동기화 중으로 설정.
			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->BeginCacheSync();

			CHECK(!t.Input(data));

			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(!gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID)->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestItemSellDeleteSyncFailItem)
		{
			const int				nMoney		= 100;
			const int				nIncMoney	= 10;
			const CID				nCID		= 2;
			const int				nItemID		= 3;
			const int16				nAmt		= 2;
			GEntityPlayer*			pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID				uidPlayer	= pPlayer->GetUID();
			GItem*					pItem		= GiveTestItem(pPlayer, nItemID, nAmt);
			GDBT_NPC_SHOP_TRADE_DATA		data		= CreateSellItemDeleteData(pPlayer, pItem, nIncMoney, nAmt);

			GDBTaskItemSellDelete t(uidPlayer);

			// 실패하기 위해 강제로 동기화 중으로 설정.
			gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID)->BeginCacheSync();

			CHECK(!t.Input(data));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID)->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestItemSellUpdateSyncSuccess)
		{
			MockDBManager			db;
			const int				nMoney		= 100;
			const int				nIncMoney	= 10;
			const CID				nCID		= 2;
			const int				nItemID		= 3;
			const int16				nAmt		= 2;
			const int16				nDecAmt		= 1;
			GEntityPlayer*			pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID				uidPlayer	= pPlayer->GetUID();
			GItem*					pItem		= GiveTestItem(pPlayer, nItemID, nAmt);
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(uidPlayer);
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA		data		= CreateSellItemData(pPlayer, pItem, nIncMoney, nDecAmt);

			// 강제로 캐시 데이터를 만든다.
			pch->IncXP(1, false);
			pi->IncStackAmt(1, false);

			GDBTaskItemSellUpdate* pTask = new GDBTaskItemSellUpdate(uidPlayer);

			CHECK(pTask->Input(data));
			CHECK(pch->IsSyncing());
			CHECK(pi->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK(!pi->IsSyncing());
			CHECK_EQUAL(0, pch->GetMoney().GetDecrease());
			CHECK_EQUAL(0, pi->GetStackAmt().GetDecrease());
			CHECK_EQUAL(data.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(data.m_Item.m_nStackAmt, pItem->GetAmount());
		}

		TEST_FIXTURE(FIXTURE, TestItemSellUpdateSyncFailCharacter)
		{
			const int				nMoney		= 100;
			const int				nIncMoney	= 10;
			const CID				nCID		= 2;
			const int				nItemID		= 3;
			const int16				nAmt		= 2;
			const int16				nDecAmt		= 1;
			const int16				nStackAmt	= 10;
			GEntityPlayer*			pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID				uidPlayer	= pPlayer->GetUID();
			GItem*					pItem		= GiveTestItem(pPlayer, nItemID, nAmt);
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(uidPlayer);
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA		data		= CreateSellItemData(pPlayer, pItem, nIncMoney, nDecAmt);

			// 강제로 캐시 데이터를 만든다.
			pch->IncXP(1, false);
			pi->IncStackAmt(1, false);

			// 실패하기 위해서 강제로 상태를 변경한다.
			pch->BeginCacheSync();

			GDBTaskItemSellUpdate t(uidPlayer);

			CHECK(!t.Input(data));

			CHECK(pch->IsSyncing());		
			CHECK(!pi->IsSyncing());
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi->GetStackAmt().GetIncrease());			
		}

		TEST_FIXTURE(FIXTURE, TestItemSellUpdateSyncFailItem)
		{
			const int				nMoney		= 100;
			const int				nIncMoney	= 10;
			const CID				nCID		= 2;
			const int				nItemID		= 3;
			const int16				nAmt		= 2;
			const int16				nDecAmt		= 1;
			const int16				nStackAmt	= 10;
			const int				nXP			= 123;
			GEntityPlayer*			pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID				uidPlayer	= pPlayer->GetUID();
			GItem*					pItem		= GiveTestItem(pPlayer, nItemID, nAmt);
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(uidPlayer);
			GItemDBCache*			pi			= gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_NPC_SHOP_TRADE_DATA		data		= CreateSellItemData(pPlayer, pItem, nIncMoney, nDecAmt);

			// 강제로 캐시 데이터를 만든다.
			pch->IncXP(1, false);
			pi->IncStackAmt(1, false);

			// 실패하기 위해서 강제로 상태를 변경한다.
			pi->BeginCacheSync();

			GDBTaskItemSellUpdate t(uidPlayer);

			CHECK(!t.Input(data));

			CHECK(!pch->IsSyncing());		
			CHECK(pi->IsSyncing());
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi->GetStackAmt().GetIncrease());			
		}

		void PushVeryCommnItem(GITEM_STACK_AMT_VEC& v, GItem* pItem)
		{
			GITEM_STACK_AMT amt;

			amt.nIUID			= pItem->m_nIUID;
			amt.nModStackAmt	= pItem->GetAmount();
			amt.nToStackAmt		= 0;
			amt.nSlotType		= pItem->m_nSlotType;
			amt.nSlotID			= pItem->m_nSlotID;

			v.push_back(amt);
		}

		GDBT_SELL_ITEM_VERY_COMMON CreateSellVeryCommonData(GEntityPlayer* pPlayer, GItem* pItem1, GItem* pItem2, const int nIncMoney)
		{
			GITEM_STACK_AMT_VEC vVeryComm;
			PushVeryCommnItem(vVeryComm, pItem1);		
			PushVeryCommnItem(vVeryComm, pItem2);

			GDBT_SELL_ITEM_VERY_COMMON data(pPlayer->GetAID()
				, pPlayer->GetUID()
				, pPlayer->GetCID()
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()
				, 0
				, pPlayer->GetMoney() + nIncMoney
				, nIncMoney
				, 0
				, vVeryComm);

			return data;
		}

		TEST_FIXTURE(FIXTURE, TestItemSellVeryCommonSuccess)
		{
			MockDBManager				db;
			const int					nMoney		= 100;
			const int					nIncMoney	= 10;
			const CID					nCID		= 2;
			const int					nItemID		= 3;
			const int16					nStackAmt	= 10;
			GEntityPlayer*				pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID					uidPlayer	= pPlayer->GetUID();
			GItem*						pItem1		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GItem*						pItem2		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GDBT_SELL_ITEM_VERY_COMMON	data		= CreateSellVeryCommonData(pPlayer, pItem1, pItem2, nIncMoney);

			GDBTaskItemSellVeryCommon* pTask = new GDBTaskItemSellVeryCommon(uidPlayer);

			CHECK(pTask->Input(data));

			CHECK_EQUAL(2, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));
			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID)->IsSyncing());
			CHECK(gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID)->IsSyncing());

			CHECK(db.Post(pTask));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK_EQUAL(data.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(0, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));
			CHECK(0 == gsys.pDBCacheSystem->GetItemCount(uidPlayer));			
		}	

		TEST_FIXTURE(FIXTURE, TestItemSellVeryCommonFailCharacter)
		{
			const int					nMoney		= 100;
			const int					nIncMoney	= 10;
			const CID					nCID		= 2;
			const int					nItemID		= 3;
			const int16					nStackAmt	= 10;
			GEntityPlayer*				pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID					uidPlayer	= pPlayer->GetUID();
			GItem*						pItem1		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GItem*						pItem2		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GDBT_SELL_ITEM_VERY_COMMON	data		= CreateSellVeryCommonData(pPlayer, pItem1, pItem2, nIncMoney);
			
			GDBTaskItemSellVeryCommon* pTask = new GDBTaskItemSellVeryCommon(uidPlayer);

			// 실패하기 위해서 상태를 변경함.
			gsys.pDBCacheSystem->FindCharCache(uidPlayer)->BeginCacheSync();

			CHECK(!pTask->Input(data));

			CHECK(gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(!gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID)->IsSyncing());
			CHECK(!gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID)->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(2, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));		
		}
	
		TEST_FIXTURE(FIXTURE, TestItemSellVeryCommonFailFirstItem)
		{
			const int					nMoney		= 100;
			const int					nIncMoney	= 10;
			const CID					nCID		= 2;
			const int					nItemID		= 3;
			const int16					nStackAmt	= 10;
			GEntityPlayer*				pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID					uidPlayer	= pPlayer->GetUID();
			GItem*						pItem1		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GItem*						pItem2		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GDBT_SELL_ITEM_VERY_COMMON	data		= CreateSellVeryCommonData(pPlayer, pItem1, pItem2, nIncMoney);
			
			GDBTaskItemSellVeryCommon* pTask = new GDBTaskItemSellVeryCommon(uidPlayer);

			// 실패하기 위해서 상태를 변경함.
			gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID)->BeginCacheSync();

			CHECK(!pTask->Input(data));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID)->IsSyncing());
			CHECK(!gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID)->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(2, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));		
		}
	
		TEST_FIXTURE(FIXTURE, TestItemSellVeryCommonFailSecondItem)
		{
			const int					nMoney		= 100;
			const int					nIncMoney	= 10;
			const CID					nCID		= 2;
			const int					nItemID		= 3;
			const int16					nStackAmt	= 10;
			GEntityPlayer*				pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID					uidPlayer	= pPlayer->GetUID();
			GItem*						pItem1		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GItem*						pItem2		= GiveTestItem(pPlayer, nItemID, nStackAmt);
			GDBT_SELL_ITEM_VERY_COMMON	data		= CreateSellVeryCommonData(pPlayer, pItem1, pItem2, nIncMoney);
			
			GDBTaskItemSellVeryCommon* pTask = new GDBTaskItemSellVeryCommon(uidPlayer);

			// 실패하기 위해서 상태를 변경함.
			gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID)->BeginCacheSync();

			CHECK(!pTask->Input(data));

			CHECK(!gsys.pDBCacheSystem->FindCharCache(uidPlayer)->IsSyncing());
			CHECK(!gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID)->IsSyncing());
			CHECK(gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID)->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(2, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));		
		}
	}
}

