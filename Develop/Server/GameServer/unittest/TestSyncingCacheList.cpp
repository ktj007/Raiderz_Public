#include "stdafx.h"
#include "GTestWrapper.h"
#include "GPlayerDBCache.h"
#include "GObjectDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "AStlUtil.h"
#include "GDBCacheSystem.h"
#include "GSyncingCacheList.h"
#include "GCharacterSyncingCache.h"
#include "GItemSyncingCache.h"
#include "TestDBCacheFixture.h"


class GPlayerDBCache;

SUITE(TestDBCache_SyncingList)
{
	class FIXTURE_SYNCING_LIST
	{
	public :
		~FIXTURE_SYNCING_LIST()
		{
			gsys.pDBCacheSystem->ReleasePlayerDBCacheMgr();
			gsys.pDBCacheSystem->Release();
		}

		GPlayerDBCache* CreateTestPlayerDBCache(const MUID& uidPlayer, const CID nCID)
		{
			return gsys.pDBCacheSystem->CreatePlayerCache(uidPlayer, nCID);
		}

		void CreateTestItemDBCache(GPlayerDBCache* pc, const IUID nIUID)
		{
			pc->GetItemCcMgr().Insert(new GItemDBCache_Leaf(pc->GetUID(), nIUID));
		}
		
	public :
		GTestMgrWrapper<GPlayerDBCacheMgr>		m_PlayerDBCacheMgrWrapper;		
	};

	TEST(TestSyncingList_Push)
	{
		const MUID	uidPlayer	= 1;
		const CID	nCID		= 1;
		const IUID	nIUID		= 2;

		GCharacterDBCache	cc(uidPlayer, nCID);
		GItemDBCache		ci(uidPlayer, nIUID);

		GSyncingCacheList	clst(uidPlayer);

		CHECK(NULL != clst.PushChar());
		CHECK(NULL != clst.PushItem(ci.GetIUID()));
		CHECK_EQUAL(2, clst.GetSize());
	}

	TEST(TestSyncingList_Release)
	{
		GSyncingCacheList clst(1);

		clst.PushChar();
		clst.PushItem(1);

		clst.Release();

		CHECK_EQUAL(0, clst.GetSize());
	}

	TEST_FIXTURE(FIXTURE_SYNCING_LIST, TestSyncingList_EndSync_bZero_True)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		const MUID			uidPlayer	= 1;
		const CID			nCID		= 2;
		const IUID			nIUID		= 3;
		const int			nXP			= 4;
		GSyncingCacheList	clst(uidPlayer);

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);

		GPlayerDBCache* pc = CreateTestPlayerDBCache(uidPlayer, nCID);
		CreateTestItemDBCache(pc, nIUID);

		pc->GetCharCc()->IncXP(1, nXP);
		pc->GetItemCcMgr().Find(nIUID)->IncStackAmt(1);

		clst.PushChar();
		clst.PushItem(nIUID);

		clst.EndCacheSync(true);

		CHECK_EQUAL(0, clst.GetSize());
		CHECK(!pc->GetCharCc()->IsModified());
		CHECK(!pc->GetItemCcMgr().Find(nIUID)->IsModified());		
	}

	TEST_FIXTURE(FIXTURE_SYNCING_LIST, TestSyncingList_EndSync_bZero_False)
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

		const MUID			uidPlayer	= 1;
		const CID			nCID		= 2;
		const IUID			nIUID		= 3;
		const int			nXP			= 5;
		const int16			nStackAmt	= 10;
		GSyncingCacheList	clst(uidPlayer);

		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);

		GPlayerDBCache* pc = CreateTestPlayerDBCache(uidPlayer, nCID);
		CreateTestItemDBCache(pc, nIUID);

		pc->GetCharCc()->IncXP(1, nXP);
		pc->GetItemCcMgr().Find(nIUID)->IncStackAmt(1);

		clst.PushChar();
		clst.PushItem(nIUID);

		clst.EndCacheSync(false);

		CHECK_EQUAL(0, clst.GetSize());
		CHECK(pc->GetCharCc()->IsModified());
		CHECK(pc->GetItemCcMgr().Find(nIUID)->IsModified());		
	}
}