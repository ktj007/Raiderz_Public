#include "stdafx.h"
#include "GDBCacheSystem.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GEntityPlayer.h"
#include "GItemRemover.h"
#include "GUTHelper_Item.h"
#include "GItemManager.h"
#include "GPlayerDBCache.h"
#include "GTestWrapper.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GTestForward.h"


SUITE(TestDBCache_RemoveDeleteItemCache)
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

	TEST(TestItemRemoverDeleteItemcCache)
	{
		const int nItemID		= 2;
		const IUID nIUID		= 3;
		GEntityPlayer* pPlayer = test.player.NewPlayer();

		gsys.pDBCacheSystem->CreatePlayerCache(pPlayer->GetUID(), pPlayer->GetCID());

		GUTHelper_Item::NewItemData(nItemID);
		GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayer, nItemID);		
		pItem->m_nIUID = nIUID;
		gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), pItem->m_nSlotType, pItem->m_nSlotID, nIUID);

		GItemRemover remover;

		CHECK(NULL != gmgr.pPlayerDBCacheMgr->Find(pPlayer->GetUID())->GetItemCcMgr().Find(nIUID));
		remover.RemoveForDBTask(pPlayer->GetUID(), REMOVE_ITEM(pItem->m_nSlotType, pItem->m_nSlotID, 0, pItem->GetAmount()));
		CHECK(NULL == gmgr.pPlayerDBCacheMgr->Find(pPlayer->GetUID())->GetItemCcMgr().Find(nIUID));
	}
}