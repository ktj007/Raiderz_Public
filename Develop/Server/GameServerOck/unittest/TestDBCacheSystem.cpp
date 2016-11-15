#include "stdafx.h"
#include "GDBCacheSystem.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GPlayerDBCache.h"
#include "GTestWrapper.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "TestDBCacheFixture.h"
#include "GUTHelper_Item.h"


SUITE(TestDBCache_DBCacheSystem)
{
	class MockEntityPlayer_DBCachSystem : public GEntityPlayer
	{
	public :
		void SetUID(const MUID& uidPlayer) { m_UID = uidPlayer; }
	};

	class MockDBCacheSystem : public GDBCacheSystem
	{
	public :
		~MockDBCacheSystem()
		{
			ReleasePlayerDBCacheMgr();
			Release();
		}

		using GDBCacheSystem::FindCharCache;
		using GDBCacheSystem::FindItemCache;

		void SetTestEntityPlayer(const MUID& uidPlayer, const CID nCID)
		{
			m_EntityPlayer.SetUID(uidPlayer);
			m_EntityPlayer.GetPlayerInfo()->nCID = nCID;
		}

		void CreateTestPlayerCache(const MUID& uidPlayer, const CID nCID, GItemDBCache_Leaf* pTestItemCc = NULL)
		{
			GPlayerDBCache* pc = new GPlayerDBCache(uidPlayer, nCID);
			pc->GetItemCcMgr().Insert(pTestItemCc);
			gmgr.pPlayerDBCacheMgr->Insert(pc);		
		}

		GEntityPlayer* GetEntityPlayer(const MUID& uidPlayer) override
		{
			return &m_EntityPlayer;
		}

	public :
		MockEntityPlayer_DBCachSystem			m_EntityPlayer;
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;
		GTestMgrWrapper<GPlayerDBCacheMgr>		m_PlayerDBCacheWrapper;
	};


	TEST(TestDBCacheSystemCreatePlayerCache)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);

		GPlayerDBCache* pc = sys.CreatePlayerCache(uidPlayer, nCID);
		CHECK(NULL != pc);
		CHECK(gmgr.pPlayerDBCacheMgr->Find(uidPlayer));
		CHECK_EQUAL(pc, gmgr.pPlayerDBCacheMgr->Find(uidPlayer));
	}

	TEST(TestCreateTestDeletePlayerCache)
	{
		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		const IUID	nIUID		= 3;

		sys.CreateTestPlayerCache(uidPlayer, nCID, new GItemDBCache_Leaf(uidPlayer, nIUID));

		CHECK(NULL != gmgr.pPlayerDBCacheMgr->Find(uidPlayer));
		CHECK(NULL != gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetCharCc());
		CHECK(NULL != gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(nIUID));
	}

	TEST(TestDBCacheSystemDeletePlayerCache)
	{
		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		const IUID	nIUID		= 3;

		sys.CreateTestPlayerCache(uidPlayer, nCID, new GItemDBCache_Leaf(uidPlayer, nIUID));

		sys.DeletePlayerCache(uidPlayer);

		CHECK(NULL == gmgr.pPlayerDBCacheMgr->Find(uidPlayer));
	}

	TEST(TestDBCacheCreateItemWithExistPlayerCache)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayerObj->GetEntity());

		GPlayerDBCache* pExistPlayerCache = sys.CreatePlayerCache(uidPlayer, nCID);

		GItemDBCache* pi = sys.CreateItem(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
		CHECK(NULL != pi);
		CHECK_EQUAL(pExistPlayerCache->GetItemCcMgr().Find(pItem->m_nIUID), pi);
		CHECK_EQUAL(pItem->m_nIUID, pi->GetIUID());
	}

	TEST(TestDBCacheCreateItemWithNoExistPlayerCache)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayerObj->GetEntity());

		sys.SetTestEntityPlayer(uidPlayer, nCID);

		GItemDBCache* pi = sys.CreateItem(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);

		GPlayerDBCache* pc = gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
		CHECK(NULL != pc);
		CHECK_EQUAL(uidPlayer, pc->GetUID());
		CHECK_EQUAL(nCID, pc->GetCharCc()->GetCID());

		CHECK(NULL != pi);
		CHECK_EQUAL(pc->GetItemCcMgr().Find(pItem->m_nIUID), pi);
		CHECK_EQUAL(pItem->m_nIUID, pi->GetIUID());
	}

	TEST(TestDBCacheDeleteItem)
	{
		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		const IUID  nIUID		= 3;

		sys.CreateTestPlayerCache(uidPlayer, nCID, new GItemDBCache_Leaf(uidPlayer, nIUID));

		sys.DeleteItem(uidPlayer, nIUID);
		CHECK(NULL == gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(nIUID));
	}

	TEST(TestDBCacheSystemFindCharCache_CreatePlayerCacheWhenPlayerCacheNotExist)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;
		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);

		sys.SetTestEntityPlayer(uidPlayer, nCID);

		GCharacterDBCache* pch = sys.FindCharCache(uidPlayer);
		
		CHECK(NULL != pch);
		CHECK_EQUAL(uidPlayer, pch->GetPlayerUID());
		CHECK_EQUAL(nCID, pch->GetCID());

		GPlayerDBCache* pc = gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
		CHECK(NULL != pc);
		CHECK_EQUAL(uidPlayer, pc->GetUID());
		CHECK_EQUAL(uidPlayer, pc->GetCharCc()->GetPlayerUID());
		CHECK_EQUAL(nCID, pc->GetCharCc()->GetCID());
	}

	TEST(TestDBCacheSystemFindCharCache_OnlyFindWhenPlayerCacheExist)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;
		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);

		sys.CreateTestPlayerCache(uidPlayer, nCID);

		GCharacterDBCache* pch = sys.FindCharCache(uidPlayer);

		CHECK(NULL != pch);
		CHECK_EQUAL(uidPlayer, pch->GetPlayerUID());
		CHECK_EQUAL(nCID, pch->GetCID());

		GPlayerDBCache* pc = gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
		CHECK(NULL != pc);
		CHECK_EQUAL(uidPlayer, pc->GetUID());
		CHECK_EQUAL(uidPlayer, pc->GetCharCc()->GetPlayerUID());
		CHECK_EQUAL(nCID, pc->GetCharCc()->GetCID());
		CHECK_EQUAL(1, gmgr.pPlayerDBCacheMgr->size());
	}

	TEST(TestDBCacheSystemFindItemCache_CreatePlayerCacheAndItemCacheWhenPlayerCacheAndIteCacheBothNotExist)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		
		sys.SetTestEntityPlayer(uidPlayer, nCID);

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayerObj->GetEntity());

		GItemDBCache* pi = sys.FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID,  pItem->m_nIUID);
		CHECK(NULL != pi);
		CHECK_EQUAL(gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(pItem->m_nIUID), pi);
		CHECK_EQUAL(pItem->m_nIUID, pi->GetIUID());

		GPlayerDBCache* pc = gmgr.pPlayerDBCacheMgr->Find(uidPlayer);
		CHECK(NULL != pc);
		CHECK_EQUAL(uidPlayer, pc->GetUID());
		CHECK_EQUAL(nCID, pc->GetCharCc()->GetCID());		
	}

	TEST(TestDBCacheSystemFindItemCache_OnlyCreateItemCacheWhenPlayerCacheExistAndIteCacheNotExist)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayerObj->GetEntity());

		sys.CreateTestPlayerCache(uidPlayer, nCID);

		GItemDBCache* pi = sys.FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
		CHECK(NULL != pi);
		CHECK_EQUAL(gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(pItem->m_nIUID), pi);
		CHECK_EQUAL(pItem->m_nIUID, pi->GetIUID());
	}

	TEST(TestDBCacheSystemFindItemCache_OnlyFindWhenPlayerCacheAndIteCacheBothExist)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		MockDBCacheSystem sys;

		const MUID	uidPlayer	= 1;
		const CID	nCID		= 2;
		
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayerObj->GetEntity());

		sys.CreateTestPlayerCache(uidPlayer, nCID, new GItemDBCache_Leaf(uidPlayer, pItem->m_nIUID));

		GItemDBCache* pi = sys.FindItemCache(uidPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->m_nIUID);
		CHECK(NULL != pi);
		CHECK_EQUAL(gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().Find(pItem->m_nIUID), pi);
		CHECK_EQUAL(1, gmgr.pPlayerDBCacheMgr->Find(uidPlayer)->GetItemCcMgr().size());
		CHECK_EQUAL(pItem->m_nIUID, pi->GetIUID());
	}

	TEST(testDBCacheSystemRelease)
	{
		MockDBCacheSystem sys;

		const MUID	uidPlayer[2]	= {1, 2};
		const CID	nCID[2]			= {11,12};

		sys.CreateTestPlayerCache(uidPlayer[0], nCID[0]);
		sys.CreateTestPlayerCache(uidPlayer[1], nCID[1]);

		sys.Release();

		CHECK(gmgr.pPlayerDBCacheMgr->empty());
	}
}