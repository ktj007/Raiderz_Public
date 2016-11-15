#include "stdafx.h"
#include "TestDBCacheFixture.h"
#include "SUnitTestUtil.h"
#include "GDBTaskItemUnequip.h"
#include "GItemUnequiper.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"

SUITE(TestDBCache_ItemUnequip)
{
	using namespace test_dbcache_fixture;

	TEST_FIXTURE(FIXTURE, TestItemUnequipSyncSuccess)
	{
		MockDBManager		db;
		const CID			nCID				= SUnitTestUtil::NewCID();
		GEntityPlayer*		pPlayer				= CreateTestPlayer(nCID, 0, 0);
		const MUID			uidPlayer			= pPlayer->GetUID();
		GItem*				pUnequipItem		= GUTHelper_Item::GainWeaponItem(pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem*				pExtraUnequipItem	= GUTHelper_Item::GainWeaponItem(pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		pExtraUnequipItem->m_pItemData->m_bLeftWeaponEquipable = true;
		GUTHelper_Item::EquipItem(pPlayer, pUnequipItem, ITEMSLOT_RWEAPON);
		GUTHelper_Item::EquipItem(pPlayer, pExtraUnequipItem, ITEMSLOT_LWEAPON);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pUnequipItem->m_nSlotType, pUnequipItem->m_nSlotID, pUnequipItem->m_nIUID);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pExtraUnequipItem->m_nSlotType, pExtraUnequipItem->m_nSlotID, pExtraUnequipItem->m_nIUID);		
		GItemDBCache*		pUnequipItemCache = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pUnequipItem->m_nSlotType, pUnequipItem->m_nSlotID, pUnequipItem->m_nIUID);
		GItemDBCache*		pExtraUnequipItemCache = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pExtraUnequipItem->m_nSlotType, pExtraUnequipItem->m_nSlotID, pExtraUnequipItem->m_nIUID);
		GDBT_ITEM_UNEQUIP		data;
		gsys.pItemSystem->GetUnequiper().MakeDBTaskData(pPlayer, pUnequipItem, data);

		GDBTaskItemUnequip* pTask = new GDBTaskItemUnequip(uidPlayer);

		CHECK(pTask->Input(data));
		CHECK(pUnequipItemCache->IsSyncing());
		CHECK(pExtraUnequipItemCache->IsSyncing());

		CHECK(db.Post(pTask));
		CHECK(!pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pUnequipItem));
		CHECK(!pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pExtraUnequipItem));

		CHECK_EQUAL(pUnequipItemCache->GetSlotType(), pUnequipItem->m_nSlotType);
		CHECK_EQUAL(pUnequipItemCache->GetSlotID(), pUnequipItem->m_nSlotID);

		CHECK_EQUAL(pExtraUnequipItemCache->GetSlotType(), pExtraUnequipItem->m_nSlotType);
		CHECK_EQUAL(pExtraUnequipItemCache->GetSlotID(), pExtraUnequipItem->m_nSlotID);
	}

	TEST_FIXTURE(FIXTURE, TestItemUnequipSyncFail)
	{
		MockDBManager		db;
		const CID			nCID				= SUnitTestUtil::NewCID();
		GEntityPlayer*		pPlayer				= CreateTestPlayer(nCID, 0, 0);
		const MUID			uidPlayer			= pPlayer->GetUID();
		GItem*				pUnequipItem		= GUTHelper_Item::GainWeaponItem(pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		GItem*				pExtraUnequipItem	= GUTHelper_Item::GainWeaponItem(pPlayer, INVALID_ID, WEAPON_1H_SLASH);
		pExtraUnequipItem->m_pItemData->m_bLeftWeaponEquipable = true;
		GUTHelper_Item::EquipItem(pPlayer, pUnequipItem, ITEMSLOT_RWEAPON);
		GUTHelper_Item::EquipItem(pPlayer, pExtraUnequipItem, ITEMSLOT_LWEAPON);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pUnequipItem->m_nSlotType, pUnequipItem->m_nSlotID, pUnequipItem->m_nIUID);
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pExtraUnequipItem->m_nSlotType, pExtraUnequipItem->m_nSlotID, pExtraUnequipItem->m_nIUID);		
		GItemDBCache*		pUnequipItemCache = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pUnequipItem->m_nSlotType, pUnequipItem->m_nSlotID, pUnequipItem->m_nIUID);
		GItemDBCache*		pExtraUnequipItemCache = gsys.pDBCacheSystem->FindItemCache(uidPlayer, pExtraUnequipItem->m_nSlotType, pExtraUnequipItem->m_nSlotID, pExtraUnequipItem->m_nIUID);
		GDBT_ITEM_UNEQUIP		data;
		gsys.pItemSystem->GetUnequiper().MakeDBTaskData(pPlayer, pUnequipItem, data);

		GDBTaskItemUnequip* pTask = new GDBTaskItemUnequip(uidPlayer);

		// 실패하기 위해 강제로 동기화 중으로 설정.
		pUnequipItemCache->BeginCacheSync();
		pExtraUnequipItemCache->BeginCacheSync();

		CHECK(!pTask->Input(data));
		CHECK(pUnequipItemCache->IsSyncing());
		CHECK(pExtraUnequipItemCache->IsSyncing());

		CHECK(pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pUnequipItem));
		CHECK(pPlayer->GetItemHolder()->GetEquipment().IsEquipItem(pExtraUnequipItem));

		SAFE_DELETE(pTask);

		CHECK_EQUAL(pUnequipItemCache->GetSlotType(), pUnequipItem->m_nSlotType);
		CHECK_EQUAL(pUnequipItemCache->GetSlotID(), pUnequipItem->m_nSlotID);

		CHECK_EQUAL(pExtraUnequipItemCache->GetSlotType(), pExtraUnequipItem->m_nSlotType);
		CHECK_EQUAL(pExtraUnequipItemCache->GetSlotID(), pExtraUnequipItem->m_nSlotID);
	}
}	