#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "SUnitTestUtil.h"
#include "GDBTaskItemDelete.h"
#include "GItemHolder.h"

SUITE(TestDBCache_ItemDelete)
{
	using namespace test_dbcache_fixture;

	TEST_FIXTURE(FIXTURE, TestItemDeleteSuccess)
	{
		MockDBManager		db;
		const CID			nCID			= SUnitTestUtil::NewCID();
		GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);		
		const MUID			uidPlayer		= pPlayer->GetUID();
		GItem*				pItem			= GiveTestItem(pPlayer);
		GItemDBCache*		pItemCache		= gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
		int					nSlotID			 = pItem->m_nSlotID;

		GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), pItem->m_pItemData->m_nTier, -pItem->GetAmount(), -1, 0);

		data.m_Item.Build(pPlayer->GetCID(), pItem);
		data.m_Item.m_nStackAmt = 0;

		GDBTaskItemDelete* pTask = new GDBTaskItemDelete(uidPlayer);

		CHECK(pTask->Input(data));
		CHECK(pItemCache->IsSyncing());

		CHECK(db.Post(pTask));
		CHECK(NULL == pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nSlotID));
	}

	TEST_FIXTURE(FIXTURE, TestItemDeleteFail)
	{
		MockDBManager		db;
		const CID			nCID			= SUnitTestUtil::NewCID();
		GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);		
		const MUID			uidPlayer		= pPlayer->GetUID();
		GItem*				pItem			= GiveTestItem(pPlayer);
		GItemDBCache*		pItemCache		= gsys.pDBCacheSystem->FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
		int					nSlotID			 = pItem->m_nSlotID;

		GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), pItem->m_pItemData->m_nTier, -pItem->GetAmount(), -1, 0);

		data.m_Item.Build(pPlayer->GetCID(), pItem);
		data.m_Item.m_nStackAmt = 0;

		GDBTaskItemDelete* pTask = new GDBTaskItemDelete(uidPlayer);

		// 실패하기 위해 강제로 동기화 중으로 설정.
		pItemCache->BeginCacheSync();

		CHECK(!pTask->Input(data));
		CHECK(pItemCache->IsSyncing());

		CHECK(pItem == pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nSlotID));
	}	
}	
