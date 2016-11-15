#include "stdafx.h"
#include "GDBCacheSystem.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GEntityPlayer.h"
#include "GItemAdder.h"
#include "GUTHelper_Item.h"
#include "GItemManager.h"
#include "GPlayerDBCache.h"
#include "TestDBCacheFixture.h"


SUITE(TestDBCache_AdderInsertItem)
{
	class MockEntityPlayer_ItemCache : public GEntityPlayer
	{
	public :
		~MockEntityPlayer_ItemCache()
		{
			gsys.pDBCacheSystem->DeletePlayerCache(GetUID());
			gmgr.pItemManager->Clear();
		}

		void SetUID(const MUID& uid)
		{
			m_UID = uid;
		}
	};

	TEST(TestItemAdderInsertItemCache)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		const MUID uidPlayer	= 1;
		const int nItemID		= 2;
		const IUID nIUID		= 3;
		const CID  nCID			= 4;

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		
		gsys.pDBCacheSystem->CreatePlayerCache(uidPlayer, nCID);

		GUTHelper_Item::NewItemData(nItemID);
		GItemAdder adder;
		
		CHECK(NULL == gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(nIUID));

		adder.AddForDBTask(uidPlayer, ADD_ITEM(1, 0, 1, 1, nIUID, nItemID));

		GItemDBCache* pi = gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(nIUID);
		CHECK(NULL != pi);
		CHECK(!pi->IsModified());
		CHECK(!pi->IsSyncing());

		if (DBCACHE_ITEM_MAX_MOD_COUNT > 0 && DBCACHE_MAX_MOD_STACKAMT > 0 && DBCACHE_MAX_MOD_DURA > 0)
			CHECK(!pi->IsOverflow());
	}
}