#include "stdafx.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GDBTaskCacheFlush.h"
#include "GDBCacheFlush.h"
#include "GEntityPlayer.h"
#include "GItem.h"
#include "GUTHelper_Item.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"

SUITE(TestDBCache_Flush)
{
	class FIXTURE_CACHE_FLUSH
	{
	public :
		FIXTURE_CACHE_FLUSH() : uidPlayer(1), nCID(2), nIUID(3)
		{

		}

		class MockDBCacheChar : public GCharacterDBCache
		{
		public :
			MockDBCacheChar(const MUID& uidPlayer, const int nCID) 
				: GCharacterDBCache(uidPlayer, nCID), m_bBeginSync(false), m_bEndSync(false), m_bZeor(false) {}

			bool BeginCacheSync() override
			{
				m_bBeginSync = true;
				m_bLock		 = true;			

				return true;
			}

			void EndCacheSync(const bool bSetZero)
			{
				m_bEndSync	= true;
				m_bZeor		= bSetZero;
				m_bLock		= false;
			}

		public :
			bool m_bBeginSync;
			bool m_bEndSync;
			bool m_bZeor;
		};

		class MockDBCacheItem : public GItemDBCache
		{
		public :
			MockDBCacheItem(const MUID& uidPlayer, const IUID nIUID, GItem* pItem) 
			: GItemDBCache(uidPlayer, nIUID), m_bBeginSync(false), m_bEndSync(false), m_bZeor(false)
			{
				m_nSlotType = pItem->m_nSlotType;
				m_nSlotID = pItem->m_nSlotID;				
			}

			bool BeginCacheSync() override
			{
				m_bBeginSync = true;
				m_bLock		 = true;		

				return true;
			}

			void EndCacheSync(const bool bSetZero)
			{
				m_bEndSync	= true;
				m_bZeor		= bSetZero;
				m_bLock		= false;
			}

		public :
			bool m_bBeginSync;
			bool m_bEndSync;
			bool m_bZeor;
		};

		GPlayerDBCache* CreateCachedPlayerCc(const MUID& uidPlayer, const CID nCID, const IUID nIUID)
		{
			GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
			pPlayerObj->GetAccountInfo().nAID = 1;
			pPlayerObj->GetEntity()->GetPlayerInfo()->nCID = SUnitTestUtil::NewCID();
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
			GItem* pItem = GUTHelper_Item::GiveNewItem(pPlayerObj->GetEntity());
			pItem->m_nIUID = nIUID;

			GPlayerDBCache* pc = new GPlayerDBCache(uidPlayer, nCID, new MockDBCacheChar(uidPlayer, nCID));

			pc->GetItemCcMgr().Insert(new MockDBCacheItem(uidPlayer, nIUID, pItem));

			pc->GetCharCc()->IncMoney(1);
			pc->GetItemCcMgr().Find(nIUID)->IncStackAmt(1);

			// 캐시를 변경하면 자동으로 상태가 Syncing으로 바뀌기 때문에 테스트를 위해서 강제로 변경한다.
			pc->GetCharCc()->EndCacheSync(false);
			pc->GetItemCcMgr().Find(nIUID)->EndCacheSync(false);

			return pc;
		}

		void ReleasePlayerCc(GPlayerDBCache* pc)
		{
			GItemDBCache* pi = pc->GetItemCcMgr().Find(nIUID);
			pc->GetItemCcMgr().Erase(nIUID);
			if (NULL != pi)
				SAFE_DELETE(pi);

			SAFE_DELETE(pc);
		}

	public :
		const MUID	uidPlayer;
		const CID	nCID;
		const IUID	nIUID;
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;
	};

	class MockDBCachFlushTaskPoster : public GDBCacheFlush
	{
	public :
		MockDBCachFlushTaskPoster (GPlayerDBCache* pPlayerCc) 
			: GDBCacheFlush(pPlayerCc), m_pItem(NULL) {}
		~MockDBCachFlushTaskPoster()
		{
			if (NULL != m_pItem)
				SAFE_DELETE(m_pItem);
		}

		class MockDB : public GDBManager
		{
		public :
			bool Post(GDBAsyncTask* pTask) override { SAFE_DELETE(pTask); return true; }
		};

		GDBManager*	GetDBManager() override
		{
			return &m_db;
		}

		GDBTaskCacheFlush* GetTask()				{ return m_pTask; }
		GDBTaskCacheFlush* SetupTestDumyTask()		{ m_pTask = new GDBTaskCacheFlush(m_pPlayerCc->GetUID()); return m_pTask; }

		void SetTestRollbackData(const CID nCID, const IUID nIUID)
		{
			m_nCID = nCID;
			m_vIUID.push_back(nIUID);
		}

		using GDBCacheFlush::BuidDBCacheChar;		
		using GDBCacheFlush::BuldDBCacheItem;
		using GDBCacheFlush::Rollback;
		using GDBCacheFlush::PostTask;

	public :
		MockDB			m_db;
		GEntityPlayer	m_Player;
		GItem*			m_pItem;
	};

	TEST_FIXTURE(FIXTURE_CACHE_FLUSH, TestCacheFlushTaskPoster_BuildDBCacheChar)
	{
		GPlayerDBCache* pc = CreateCachedPlayerCc(uidPlayer, nCID, nIUID);
		MockDBCachFlushTaskPoster poster(pc);

		CHECK(poster.BuidDBCacheChar(pc->GetCharCc()));		

		CHECK_EQUAL(GDBCacheFlush::FLUSH_BUILD_DBCACHE_CHAR, poster.GetLastState());
		CHECK(!pc->IsFlushing());
		CHECK(pc->GetCharCc()->IsSyncing());
		CHECK(NULL != poster.GetTask());
		CHECK_EQUAL(nCID, poster.GetTask()->GetCharQuery().GetID());
		CHECK_EQUAL(0, poster.GetTask()->GetItemQueryVec().size());

		ReleasePlayerCc(pc);
	}

	TEST_FIXTURE(FIXTURE_CACHE_FLUSH, TestCacheFlushTaskPoster_BuildDBCacheItem)
	{
		GPlayerDBCache* pc = CreateCachedPlayerCc(uidPlayer, nCID, nIUID);
		MockDBCachFlushTaskPoster poster(pc);

		CHECK(poster.BuldDBCacheItem(pc));		

		CHECK_EQUAL(GDBCacheFlush::FLUSH_BUILD_DBCACHE_ITEM, poster.GetLastState());
		CHECK(!pc->IsFlushing());
		CHECK(!pc->GetCharCc()->IsSyncing());
		CHECK(NULL != poster.GetTask());
		CHECK_EQUAL(0, poster.GetTask()->GetCharQuery().GetID());
		CHECK_EQUAL(1, poster.GetTask()->GetItemQueryVec().size());
		CHECK_EQUAL(nIUID, poster.GetTask()->GetItemQueryVec()[0].GetID());
		CHECK(pc->GetItemCcMgr().Find(nIUID)->IsSyncing());

		ReleasePlayerCc(pc);
	}

	TEST_FIXTURE(FIXTURE_CACHE_FLUSH, TestCacheFlushTaskPoster_ROLLBACK)
	{
		GPlayerDBCache* pc = CreateCachedPlayerCc(uidPlayer, nCID, nIUID);
		MockDBCachFlushTaskPoster poster(pc);

		pc->GetCharCc()->BeginCacheSync();
		pc->GetItemCcMgr().Find(nIUID)->BeginCacheSync();
		poster.SetTestRollbackData(nCID, nIUID);

		poster.Rollback(pc);

		CHECK_EQUAL(GDBCacheFlush::FLUSH_ROLLBACK, poster.GetLastState());
		CHECK(!pc->IsFlushing());
		CHECK(!pc->GetCharCc()->IsSyncing());
		CHECK(!pc->GetItemCcMgr().Find(nIUID)->IsSyncing());

		MockDBCacheChar* pmc = (MockDBCacheChar*)pc->GetCharCc();
		MockDBCacheItem* pmi = (MockDBCacheItem*)pc->GetItemCcMgr().Find(nIUID);

		CHECK(pmc->m_bEndSync);
		CHECK(pmi->m_bEndSync);

		ReleasePlayerCc(pc);
	}

	TEST_FIXTURE(FIXTURE_CACHE_FLUSH, TestCacheFlushTaskPoster_PostTask)
	{
		GPlayerDBCache* pc = CreateCachedPlayerCc(uidPlayer, nCID, nIUID);
		MockDBCachFlushTaskPoster poster(pc);
		GDBTaskCacheFlush* pTask = poster.SetupTestDumyTask();

		CHECK(poster.PostTask(pc, pTask));

		CHECK_EQUAL(GDBCacheFlush::FLUSH_POST_TASK, poster.GetLastState());
		CHECK(NULL == poster.GetTask());
		CHECK(pc->IsFlushing());

		ReleasePlayerCc(pc);
	}
}