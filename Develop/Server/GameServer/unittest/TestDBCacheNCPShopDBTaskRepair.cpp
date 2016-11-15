#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskItemRepair.h"
#include "GDBTaskItemRepairAll.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"

namespace test_dbcache_fixture
{
	SUITE(TestDBCache_NPCSopRepair)
	{
		class FIXTURE_REPAIR : public FIXTURE
		{
		public :
			GDBT_REPAIR_ITEM CreateRepairData(GEntityPlayer* pPlayer, GItem* pItem, int nDecMoney)
			{
				GDBT_REPAIR_ITEM data;
				data.m_uidPlayer		= pPlayer->GetUID();
				data.m_Char.m_nCID		= pPlayer->GetCID();
				data.m_Char.m_nMoney	= pPlayer->GetMoney() - nDecMoney;
				data.m_Char.m_nModMoney	= nDecMoney;
				data.m_nSlotType		= pItem->m_nSlotType;
				data.m_nSlotID			= pItem->m_nSlotID;
				data.m_nIUID			= pItem->m_nIUID;				

				return data;
			}

			GDBT_REPAIR_ALL_ITEM CreateRepairAllData(GEntityPlayer* pPlayer, GItem* pItem1, GItem* pItem2, const int nDecMoney)
			{
				GDBT_REPAIR_ALL_ITEM data;
				data.m_uidPlayer		= pPlayer->GetUID();
				data.m_Char.m_nCID		= pPlayer->GetCID();
				data.m_Char.m_nMoney	= pPlayer->GetMoney() - nDecMoney;
				data.m_Char.m_nModMoney	= nDecMoney;
				
				PushReapirItem(data.m_vRepair, pItem1);
				PushReapirItem(data.m_vRepair, pItem2);

				return data;
			}

			void PushReapirItem(GDBT_ITEM_REPAIR_VEC& vRepair, GItem* pItem)
			{
				GDBT_ITEM_REPAIR_ALL rep;

				rep.m_nIUID		= pItem->m_nIUID;
				rep.m_nSlotType = pItem->m_nSlotType;
				rep.m_nSlotID	= pItem->m_nSlotID;

				vRepair.push_back(rep);
			}
		};

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairSyncSuccess)
		{
			MockDBManager		db;
			const int			nItemID		= 3;
			const uint8			nDura		= 10;
			const uint8			nMaxDura	= 20;
			const int			nPrice		= 10;
			GEntityPlayer*		pPlayer		= CreateTestPlayer();
			GItem*				pItem		= GiveTestItem(pPlayer, nItemID, 1, nDura, nMaxDura);
			GCharacterDBCache*	pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*		pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_REPAIR_ITEM	data		= CreateRepairData(pPlayer, pItem, nPrice);

			pch->IncXP(1, false);
			pi->IncDura(1, false);

			GDBTaskItemRepair* pTask = new GDBTaskItemRepair(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			CHECK(pch->IsSyncing());
			CHECK(pi->IsSyncing());
			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK(!pi->IsSyncing());
			CHECK_EQUAL(data.m_Char.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nMaxDura, pItem->m_nDurability );
			CHECK_EQUAL(0, pch->GetXP().GetIncrease());
			CHECK_EQUAL(0, pi->GetDura().GetIncrease());		
		}

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairSyncFailCharacter)
		{
			const int			nMoney		= 100;
			const CID			nCID		= 2;
			const int			nItemID		= 3;
			const uint8			nDura		= 10;
			const uint8			nMaxDura	= 20;
			const int			nPrice		= 10;
			const int			nXP			= 20;
			GEntityPlayer*		pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID			uidPlayer	= pPlayer->GetUID();
			GItem*				pItem		= GiveTestItem(pPlayer, nItemID, 1, nDura, nMaxDura);
			GCharacterDBCache*	pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*		pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_REPAIR_ITEM	data		= CreateRepairData(pPlayer, pItem, nPrice);

			pch->IncXP(1, false);
			pi->IncDura(1, false);
			pch->BeginCacheSync();
			
			GDBTaskItemRepair t(uidPlayer);
			CHECK(!t.Input(data));

			CHECK(pch->IsSyncing());		
			CHECK(!pi->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nDura, pItem->m_nDurability);
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi->GetDura().GetIncrease());			
		}

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairSyncFailItem)
		{
			const int			nMoney		= 100;
			const CID			nCID		= 2;
			const int			nItemID		= 3;
			const uint8			nDura		= 10;
			const uint8			nMaxDura	= 20;
			const int			nPrice		= 10;
			const int			nXP			= 123;
			GEntityPlayer*		pPlayer		= CreateTestPlayer(nCID, 0, nMoney);
			const MUID			uidPlayer	= pPlayer->GetUID();
			GItem*				pItem		= GiveTestItem(pPlayer, nItemID, 1, nDura, nMaxDura);
			GCharacterDBCache*	pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*		pi			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
			GDBT_REPAIR_ITEM	data		= CreateRepairData(pPlayer, pItem, nPrice);

			pch->IncXP(1, false);
			pi->IncDura(1, false);
			pi->BeginCacheSync();

			GDBTaskItemRepair t(uidPlayer);
			CHECK(!t.Input(data));

			CHECK(!pch->IsSyncing());		
			CHECK(pi->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nDura, pItem->m_nDurability);
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi->GetDura().GetIncrease());
		}

		void PushReapirItem(GITEM_STACK_AMT_VEC& vStackAmt, GItem* pItem)
		{
			GITEM_STACK_AMT amt;

			amt.nIUID = pItem->m_nIUID;
			amt.nSlotType = pItem->m_nSlotType;
			amt.nSlotID = pItem->m_nSlotID;

			vStackAmt.push_back(amt);
		}

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairAllSyncSuccess)
		{
			MockDBManager			db;
			const uint8				nDura		= 10;
			const uint8				nMaxDura1	= 20;
			const uint8				nMaxDura2	= 20;
			const int				nPrice		= 10;			
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura1);
			GItem*					pItem2		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura2);
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_REPAIR_ALL_ITEM	data		= CreateRepairAllData(pPlayer, pItem1, pItem2, nPrice);

			pch->IncXP(1, false);
			pi1->IncDura(1, false);
			pi2->IncDura(2, false);

			GDBTaskItemRepairAll* pTask = new GDBTaskItemRepairAll(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			CHECK(pch->IsSyncing());
			CHECK(pi1->IsSyncing());
			CHECK(pi2->IsSyncing());
			CHECK(db.Post(pTask));

			CHECK(!pch->IsSyncing());
			CHECK(!pi1->IsSyncing());
			CHECK(!pi2->IsSyncing());
			CHECK_EQUAL(data.m_Char.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nMaxDura1, pItem1->m_nDurability );
			CHECK_EQUAL(nMaxDura2, pItem2->m_nDurability );
			CHECK_EQUAL(0, pch->GetXP().GetIncrease());
			CHECK_EQUAL(0, pi1->GetDura().GetIncrease());
			CHECK_EQUAL(0, pi2->GetDura().GetIncrease());			
		}

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairAllSyncFailChar)
		{
			const uint8				nDura		= 10;
			const uint8				nMaxDura1	= 20;
			const uint8				nMaxDura2	= 20;
			const int				nPrice		= 10;
			const int				nXP			= 10;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura1);
			GItem*					pItem2		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura2);
			const int				nMoney		= pPlayer->GetMoney();
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_REPAIR_ALL_ITEM	data		= CreateRepairAllData(pPlayer, pItem1, pItem2, nPrice);

			pch->IncXP(1, false);
			pi1->IncDura(1, false);
			pi2->IncDura(1, false);
			pch->BeginCacheSync();

			GDBTaskItemRepairAll t(pPlayer->GetUID());
			CHECK(!t.Input(data));

			CHECK(pch->IsSyncing());
			CHECK(!pi1->IsSyncing());
			CHECK(!pi2->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nDura, pItem1->m_nDurability );
			CHECK_EQUAL(nDura, pItem2->m_nDurability );		
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi1->GetDura().GetIncrease());
			CHECK_EQUAL(1, pi2->GetDura().GetIncrease());			
		}

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairAllSyncFailFirstItem)
		{
			const uint8				nDura		= 10;
			const uint8				nMaxDura1	= 20;
			const uint8				nMaxDura2	= 20;
			const int				nPrice		= 10;
			const int				nXP			= 21;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura1);
			GItem*					pItem2		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura2);
			const int				nMoney		= pPlayer->GetMoney();
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_REPAIR_ALL_ITEM	data		= CreateRepairAllData(pPlayer, pItem1, pItem2, nPrice);

			pch->IncXP(1, false);
			pi1->IncDura(1, false);
			pi2->IncDura(1, false);
			pi1->BeginCacheSync();

			GDBTaskItemRepairAll t(pPlayer->GetUID());
			CHECK(!t.Input(data));

			CHECK(!pch->IsSyncing());
			CHECK(pi1->IsSyncing());
			CHECK(!pi2->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nDura, pItem1->m_nDurability );
			CHECK_EQUAL(nDura, pItem2->m_nDurability );		
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi1->GetDura().GetIncrease());
			CHECK_EQUAL(1, pi2->GetDura().GetIncrease());						
		}

		TEST_FIXTURE(FIXTURE_REPAIR, TestItemRepairAllSyncFailSecondItem)
		{
			const uint8				nDura		= 10;
			const uint8				nMaxDura1	= 20;
			const uint8				nMaxDura2	= 20;
			const int				nPrice		= 10;
			const int				nXP			= 123;
			GEntityPlayer*			pPlayer		= CreateTestPlayer();
			GItem*					pItem1		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura1);
			GItem*					pItem2		= GiveTestItem(pPlayer, 1, 1, nDura, nMaxDura2);
			const int				nMoney		= pPlayer->GetMoney();
			GCharacterDBCache*		pch			= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem1->m_nSlotType, pItem1->m_nSlotID, pItem1->m_nIUID);
			GItemDBCache*			pi2			= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pItem2->m_nSlotType, pItem2->m_nSlotID, pItem2->m_nIUID);
			GDBT_REPAIR_ALL_ITEM	data		= CreateRepairAllData(pPlayer, pItem1, pItem2, nPrice);

			pch->IncXP(1, false);
			pi1->IncDura(1, false);
			pi2->IncDura(1, false);
			pi2->BeginCacheSync();

			GDBTaskItemRepairAll t(pPlayer->GetUID());
			CHECK(!t.Input(data));

			CHECK(!pch->IsSyncing());
			CHECK(!pi1->IsSyncing());
			CHECK(pi2->IsSyncing());
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nDura, pItem1->m_nDurability );
			CHECK_EQUAL(nDura, pItem2->m_nDurability );		
			CHECK_EQUAL(1, pch->GetXP().GetIncrease());
			CHECK_EQUAL(1, pi1->GetDura().GetIncrease());
			CHECK_EQUAL(1, pi2->GetDura().GetIncrease());
		}
	}
}