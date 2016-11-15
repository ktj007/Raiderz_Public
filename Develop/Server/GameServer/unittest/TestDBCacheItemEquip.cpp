#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "SUnitTestUtil.h"
#include "GDBTaskItemEquip.h"
#include "GItemEquiper.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"

SUITE(TestDBCache_ItemEquip)
{
	using namespace test_dbcache_fixture;

	TEST_FIXTURE(FIXTURE, TestItemEquipSyncSuccess)
	{
		MockDBManager		db;
		const CID			nCID			= SUnitTestUtil::NewCID();
		GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);		
		const MUID			uidPlayer		= pPlayer->GetUID();
		GItem*				pEquipItem		= GUTHelper_Item::GainWeaponItem(pPlayer, INVALID_ID, WEAPON_TWO_HANDED);
		GItem*				pUnequipItem1	= GUTHelper_Item::EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem*				pUnequipItem2	= GUTHelper_Item::EquipNewShield(pPlayer);
		GItemDBCache*		pEquipItemCache = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pEquipItem->m_nSlotType, pEquipItem->m_nSlotID, pEquipItem->m_nIUID);
		GItemDBCache*		pUnequipItemCache1 = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pUnequipItem1->m_nSlotType, pUnequipItem1->m_nSlotID, pUnequipItem1->m_nIUID);
		GItemDBCache*		pUnequipItemCache2 = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pUnequipItem2->m_nSlotType, pUnequipItem2->m_nSlotID, pUnequipItem2->m_nIUID);
		GDBT_ITEM_EQUIP		data;
		gsys.pItemSystem->GetEquiper().MakeDBTaskData(pPlayer, pEquipItem, pEquipItem->m_pItemData->m_nItemSlot, pEquipItem->m_pItemData->m_nSubSlot, false, data);

		GDBTaskItemEquip* pTask = new GDBTaskItemEquip(uidPlayer);

		CHECK(pTask->Input(data));
		CHECK(pEquipItemCache->IsSyncing());
		CHECK(pUnequipItemCache1->IsSyncing());
		CHECK(pUnequipItemCache2->IsSyncing());

		CHECK(db.Post(pTask));
		CHECK(pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pEquipItem));
		CHECK(!pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pUnequipItem1));
		CHECK(!pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pUnequipItem2));

		CHECK_EQUAL(pEquipItemCache->GetSlotType(), pEquipItem->m_nSlotType);
		CHECK_EQUAL(pEquipItemCache->GetSlotID(), pEquipItem->m_nSlotID);

		CHECK_EQUAL(pUnequipItemCache1->GetSlotType(), pUnequipItem1->m_nSlotType);
		CHECK_EQUAL(pUnequipItemCache1->GetSlotID(), pUnequipItem1->m_nSlotID);

		CHECK_EQUAL(pUnequipItemCache2->GetSlotType(), pUnequipItem2->m_nSlotType);
		CHECK_EQUAL(pUnequipItemCache2->GetSlotID(), pUnequipItem2->m_nSlotID);
	}

	TEST_FIXTURE(FIXTURE, TestItemEquipSyncFail)
	{
		MockDBManager		db;
		const CID			nCID			= SUnitTestUtil::NewCID();
		GEntityPlayer*		pPlayer			= CreateTestPlayer(nCID, 0, 0);
		const MUID			uidPlayer		= pPlayer->GetUID();
		GItem*				pEquipItem		= GUTHelper_Item::GainWeaponItem(pPlayer, INVALID_ID, WEAPON_TWO_HANDED);
		GItem*				pUnequipItem1	= GUTHelper_Item::EquipNewWeapon(pPlayer, WEAPON_1H_SLASH);
		GItem*				pUnequipItem2	= GUTHelper_Item::EquipNewShield(pPlayer);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pEquipItem->m_nSlotType, pEquipItem->m_nSlotID, pEquipItem->m_nIUID);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pUnequipItem1->m_nSlotType, pUnequipItem1->m_nSlotID, pUnequipItem1->m_nIUID);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pUnequipItem2->m_nSlotType, pUnequipItem2->m_nSlotID, pUnequipItem2->m_nIUID);	
		GItemDBCache*		pEquipItemCache = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pEquipItem->m_nSlotType, pEquipItem->m_nSlotID, pEquipItem->m_nIUID);
		GItemDBCache*		pUnequipItemCache1 = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pUnequipItem1->m_nSlotType, pUnequipItem1->m_nSlotID, pUnequipItem1->m_nIUID);
		GItemDBCache*		pUnequipItemCache2 = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pUnequipItem2->m_nSlotType, pUnequipItem2->m_nSlotID, pUnequipItem2->m_nIUID);
		GDBT_ITEM_EQUIP		data;
		gsys.pItemSystem->GetEquiper().MakeDBTaskData(pPlayer, pEquipItem, pEquipItem->m_pItemData->m_nItemSlot, pEquipItem->m_pItemData->m_nSubSlot, false, data);

		GDBTaskItemEquip t(uidPlayer);

		// 실패하기 위해 강제로 동기화 중으로 설정.
		pEquipItemCache->BeginCacheSync();
		pUnequipItemCache1->BeginCacheSync();
		pUnequipItemCache2->BeginCacheSync();		

		CHECK(!t.Input(data));
		CHECK(pEquipItemCache->IsSyncing());
		CHECK(pUnequipItemCache1->IsSyncing());
		CHECK(pUnequipItemCache2->IsSyncing());

		CHECK(!pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pEquipItem));
		CHECK(pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pUnequipItem1));
		CHECK(pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pUnequipItem2));

		CHECK_EQUAL(pEquipItemCache->GetSlotType(), pEquipItem->m_nSlotType);
		CHECK_EQUAL(pEquipItemCache->GetSlotID(), pEquipItem->m_nSlotID);

		CHECK_EQUAL(pUnequipItemCache1->GetSlotType(), pUnequipItem1->m_nSlotType);
		CHECK_EQUAL(pUnequipItemCache1->GetSlotID(), pUnequipItem1->m_nSlotID);

		CHECK_EQUAL(pUnequipItemCache2->GetSlotType(), pUnequipItem2->m_nSlotType);
		CHECK_EQUAL(pUnequipItemCache2->GetSlotID(), pUnequipItem2->m_nSlotID);
	}
}	
