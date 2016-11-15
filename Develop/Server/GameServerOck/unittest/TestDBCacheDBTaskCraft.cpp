#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "GDBTaskCraftInsert.h"
#include "GDBTaskCraftUpdate.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"
#include "SUnitTestUtil.h"


namespace test_dbcache_fixture
{
	SUITE(TestDBCache_Craft)
	{
		class FIXTURE_CRAFT : public FIXTURE
		{
		public :
			void PushRecpInfo(GITEM_STACK_AMT_VEC& vRecp, const GItem* pRecp, const int16 nRecpAmt)
			{
				GITEM_STACK_AMT recp;

				recp.nIUID			= pRecp->m_nIUID;
				recp.nToStackAmt	= pRecp->GetAmount() - nRecpAmt;
				recp.nSlotType		= pRecp->m_nSlotType;
				recp.nSlotID		= pRecp->m_nSlotID;
				recp.nModStackAmt	= nRecpAmt;

				vRecp.push_back(recp);
			}

			GDBT_CRAFT_DATA CreateInsertTaskData(GEntityPlayer* pPlayer, GItem* pRecp1, GItem* pRecp2
				 , const int nDecMoney, const int16 nRecpAmt1, const int16 nRecpAmt2
				 , const int nProdItemID, const int16 nProdSlotID, const int16 nProdStackAmt)
			{
				GITEM_STACK_AMT_VEC vRecp;
				PushRecpInfo(vRecp, pRecp1, nRecpAmt1);
				PushRecpInfo(vRecp, pRecp2, nRecpAmt2);

				GUTHelper_Item::NewItemData(nProdItemID);

				GDBT_CRAFT_DATA data(pPlayer->GetAID()
					, pPlayer->GetUID()
					, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
					, pPlayer->GetLevel()
					, pPlayer->GetPlayerInfo()->GetMoney() - nDecMoney
					, nDecMoney
					, pPlayer->GetPlayerInfo()->nXP
					, nProdStackAmt
					, -1
					, SUnitTestUtil::NewID()
					, vRecp);	

				data.m_Item.Set(pPlayer->GetCID()
					, SLOTTYPE_INVENTORY
					, nProdSlotID
					, nProdItemID
					, nProdStackAmt
					, pPlayer->GetPlayerInfo()->nPlayTimeSec
					, 0
					, 0
					, 10
					, 10
					, 0
					, 0
					, false
					, false
					, 0, L"NULL");

				return data;
			}

			GDBT_CRAFT_DATA CreateUpdateTaskData(GEntityPlayer* pPlayer, GItem* pRecp1, GItem* pRecp2, GItem* pProd
				, const int nDecMoney, const int16 nRecpAmt1, const int16 nRecpAmt2, const int16 nProdStackAmt)
			{
				GITEM_STACK_AMT_VEC vRecp;
				PushRecpInfo(vRecp, pRecp1, nRecpAmt1);
				PushRecpInfo(vRecp, pRecp2, nRecpAmt2);

				GDBT_CRAFT_DATA data(pPlayer->GetAID()
					, pPlayer->GetUID()
					, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
					, pPlayer->GetLevel()
					, pPlayer->GetPlayerInfo()->GetMoney() - nDecMoney
					, nDecMoney
					, pPlayer->GetPlayerInfo()->nXP
					, nProdStackAmt
					, -1
					, SUnitTestUtil::NewID()
					, vRecp);	

				data.m_Item.Build(pPlayer->GetCID(), pProd);
				data.m_Item.m_nStackAmt = pProd->GetAmount() + nProdStackAmt;
				data.m_Item.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;				

				return data;
			}
		};

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftInsertSyncSuccess)
		{
			MockDBManager			db;
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdItemID		= 5;
			const int16				nProdSlotID		= 10;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateInsertTaskData(pPlayer, pRecp1, pRecp2, nPrice, nRecpAmt1, nRecpAmt2, nProdItemID, nProdSlotID, nAmt);			
			const int16				nRecp2SlotID	= pRecp2->m_nSlotID;
			const IUID				nRecp2IUID		= pRecp2->m_nIUID;

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			GDBTaskCraftInsert* pTask = new GDBTaskCraftInsert(pPlayer->GetUID());

			CHECK(pTask->Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, true);
			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);
			
			CHECK(db.Post(pTask));

			CHECK_CHAR_END_CACHE(pch);
			CHECK_ITEM_END_CACHE(pi1);
			CHECK(NULL == gsys.pDBCacheSystem->FindPlayerCache(pPlayer->GetUID())->GetItemCcMgr().Find(nRecp2IUID));
			
			CHECK_EQUAL(data.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt - nRecpAmt1, pRecp1->GetAmount());
			CHECK_EQUAL(2, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));

			GItem* pDeletedRecpItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nRecp2SlotID);
			CHECK(NULL == pDeletedRecpItem);

			GItem* pProdItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nProdSlotID);
			CHECK_EQUAL(nProdItemID, pProdItem->m_pItemData->m_nID);		
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftInsertSyncFailChar)
		{
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdItemID		= 5;
			const int16				nProdSlotID		= 10;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateInsertTaskData(pPlayer, pRecp1, pRecp2, nPrice, nRecpAmt1, nRecpAmt2, nProdItemID, nProdSlotID, nAmt);			
			const int16				nRecp2SlotID	= pRecp2->m_nSlotID;

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pch->BeginCacheSync();

			GDBTaskCraftInsert t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, true);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
			
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			GItem* pProdItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nProdSlotID);
			CHECK(NULL == pProdItem);				
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftInsertSyncFailFirstItem)
		{
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdItemID		= 5;
			const int16				nProdSlotID		= 10;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateInsertTaskData(pPlayer, pRecp1, pRecp2, nPrice, nRecpAmt1, nRecpAmt2, nProdItemID, nProdSlotID, nAmt);			
			const int16				nRecp2SlotID	= pRecp2->m_nSlotID;

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pi1->BeginCacheSync();

			GDBTaskCraftInsert t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);

			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			GItem* pProdItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nProdSlotID);
			CHECK(NULL == pProdItem);				
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftInsertSyncFailSecondItem)
		{
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdItemID		= 5;
			const int16				nProdSlotID		= 10;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateInsertTaskData(pPlayer, pRecp1, pRecp2, nPrice, nRecpAmt1, nRecpAmt2, nProdItemID, nProdSlotID, nAmt);			
			const int16				nRecp2SlotID	= pRecp2->m_nSlotID;

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);

			pi2->BeginCacheSync();

			GDBTaskCraftInsert t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);

			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			GItem* pProdItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nProdSlotID);
			CHECK(NULL == pProdItem);				
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftUpdateSyncSuccess)
		{
			MockDBManager			db;
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdAmt		= 1;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GItem*					pProd			= GiveTestItem(pPlayer, 3, nAmt);	
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GItemDBCache*			ppi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pProd->m_nSlotType, pProd->m_nSlotID, pProd->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateUpdateTaskData(pPlayer, pRecp1, pRecp2, pProd, nPrice, nRecpAmt1, nRecpAmt2, nProdAmt);
			const int16				nRecp2SlotID	= pRecp2->m_nSlotID;
			const IUID				nRecp2IUID		= pRecp2->m_nIUID;

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);
			SetTestItemCc(ppi);

			GDBTaskCraftUpdate* pTask = new GDBTaskCraftUpdate(pPlayer->GetUID());

			CHECK(pTask->Input(data));
			
			CHECK_CHAR_BEGIN_CACHE(pch, true);
			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);
			CHECK_ITEM_BEGIN_CACHE(ppi, true);
			
			CHECK(db.Post(pTask));

			CHECK_CHAR_END_CACHE(pch);
			CHECK_ITEM_END_CACHE(pi1);
			CHECK(NULL == gsys.pDBCacheSystem->FindPlayerCache(pPlayer->GetUID())->GetItemCcMgr().Find(nRecp2IUID));
			CHECK_ITEM_END_CACHE(ppi);

			CHECK_EQUAL(data.m_nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt - nRecpAmt1, pRecp1->GetAmount());
			CHECK_EQUAL(2, pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY));

			GItem* pDeletedRecpItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nRecp2SlotID);
			CHECK(NULL == pDeletedRecpItem);

			CHECK_EQUAL(data.m_Item.m_nStackAmt, pProd->GetAmount());
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftUpdateSyncFailChar)
		{
			MockDBManager			db;
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdAmt		= 1;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GItem*					pProd			= GiveTestItem(pPlayer, 3, nAmt);	
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GItemDBCache*			ppi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pProd->m_nSlotType, pProd->m_nSlotID, pProd->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateUpdateTaskData(pPlayer, pRecp1, pRecp2, pProd, nPrice, nRecpAmt1, nRecpAmt2, nProdAmt);
			
			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);
			SetTestItemCc(ppi);
			pch->BeginCacheSync();

			GDBTaskCraftUpdate t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, true);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
			CHECK_ITEM_BEGIN_CACHE(ppi, false);
			
			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			CHECK_EQUAL(nAmt, pProd->GetAmount());		
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftUpdateSyncFailFirstRecp)
		{
			MockDBManager			db;
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdAmt		= 1;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GItem*					pProd			= GiveTestItem(pPlayer, 3, nAmt);	
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GItemDBCache*			ppi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pProd->m_nSlotType, pProd->m_nSlotID, pProd->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateUpdateTaskData(pPlayer, pRecp1, pRecp2, pProd, nPrice, nRecpAmt1, nRecpAmt2, nProdAmt);

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);
			SetTestItemCc(ppi);
			pi1->BeginCacheSync();

			GDBTaskCraftUpdate t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, true);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
			CHECK_ITEM_BEGIN_CACHE(ppi, false);

			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			CHECK_EQUAL(nAmt, pProd->GetAmount());		
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftUpdateSyncFailSecondRecp)
		{
			MockDBManager			db;
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdAmt		= 1;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GItem*					pProd			= GiveTestItem(pPlayer, 3, nAmt);	
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GItemDBCache*			ppi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pProd->m_nSlotType, pProd->m_nSlotID, pProd->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateUpdateTaskData(pPlayer, pRecp1, pRecp2, pProd, nPrice, nRecpAmt1, nRecpAmt2, nProdAmt);

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);
			SetTestItemCc(ppi);
			pi2->BeginCacheSync();

			GDBTaskCraftUpdate t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, true);
			CHECK_ITEM_BEGIN_CACHE(ppi, false);

			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			CHECK_EQUAL(nAmt, pProd->GetAmount());		
		}

		TEST_FIXTURE(FIXTURE_CRAFT, TestCraftUpdateSyncFailProd)
		{
			MockDBManager			db;
			const int				nMoney			= 100;
			const int				nAmt			= 10;
			const int				nRecpAmt1		= 1;
			const int				nRecpAmt2		= nAmt;
			const int				nPrice			= 10;
			const int				nProdAmt		= 1;
			GEntityPlayer*			pPlayer			= CreateTestPlayer(2, 0, nMoney);
			GItem*					pRecp1			= GiveTestItem(pPlayer, 1, nAmt);
			GItem*					pRecp2			= GiveTestItem(pPlayer, 2, nAmt);
			GItem*					pProd			= GiveTestItem(pPlayer, 3, nAmt);	
			GCharacterDBCache*		pch				= gsys.pDBCacheSystem->FindCharCache(pPlayer->GetUID());
			GItemDBCache*			pi1				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp1->m_nSlotType, pRecp1->m_nSlotID, pRecp1->m_nIUID);
			GItemDBCache*			pi2				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pRecp2->m_nSlotType, pRecp2->m_nSlotID, pRecp2->m_nIUID);
			GItemDBCache*			ppi				= gsys.pDBCacheSystem->FindItemCache(pPlayer->GetUID(), pProd->m_nSlotType, pProd->m_nSlotID, pProd->m_nIUID);
			GDBT_CRAFT_DATA	data			= CreateUpdateTaskData(pPlayer, pRecp1, pRecp2, pProd, nPrice, nRecpAmt1, nRecpAmt2, nProdAmt);

			SetTestCharCc(pch);
			SetTestItemCc(pi1);
			SetTestItemCc(pi2);
			SetTestItemCc(ppi);
			ppi->BeginCacheSync();

			GDBTaskCraftUpdate t(pPlayer->GetUID());

			CHECK(!t.Input(data));

			CHECK_CHAR_BEGIN_CACHE(pch, false);
			CHECK_ITEM_BEGIN_CACHE(pi1, false);
			CHECK_ITEM_BEGIN_CACHE(pi2, false);
			CHECK_ITEM_BEGIN_CACHE(ppi, true);

			CHECK_EQUAL(nMoney, pPlayer->GetMoney());
			CHECK_EQUAL(nAmt, pRecp1->GetAmount());
			CHECK_EQUAL(nAmt, pRecp2->GetAmount());
			CHECK_EQUAL(nAmt, pProd->GetAmount());		
		}
	}
}

