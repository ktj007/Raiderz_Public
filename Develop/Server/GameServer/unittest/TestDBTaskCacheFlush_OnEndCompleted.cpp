#include "stdafx.h"
#include "GDBTaskCacheFlush.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_OnEndCompleted)
{
	class MockDBCacheChar_FlushTask : public GCharacterDBCache
	{
	public :
		MockDBCacheChar_FlushTask(const MUID& uidPlayer, const CID nCID) : GCharacterDBCache(uidPlayer, nCID), m_bEnd(false), m_bZero(false) {}

		void EndCacheSync(const bool bSetZero) override
		{
			m_bEnd	= true;
			m_bZero = bSetZero;
		}

		bool m_bEnd;
		bool m_bZero;
	};

	class MockDBCacheItem_FlushTask : public GItemDBCache
	{
	public :
		MockDBCacheItem_FlushTask(const MUID& uidPlayer, const IUID nIUID) : GItemDBCache(uidPlayer, nIUID), m_bEnd(false), m_bZero(false) {}

		void EndCacheSync(const bool bSetZero) override
		{
			m_bEnd	= true;
			m_bZero = bSetZero;
		}

		bool m_bEnd;
		bool m_bZero;
	};

	class MockDBTaskCacheFlush_OnEndCompleted : public GDBTaskCacheFlush
	{
	public :
		MockDBTaskCacheFlush_OnEndCompleted() : GDBTaskCacheFlush(0), m_uidPlayer(1), m_nCID(2), m_nIUID(3), m_PlayerCc(1, 2, new MockDBCacheChar_FlushTask(1, 2)) {}
		~MockDBTaskCacheFlush_OnEndCompleted()
		{
			GItemDBCache* pi = m_PlayerCc.GetItemCcMgr().Find(m_nIUID);
			SAFE_DELETE(pi);
		}

		using GDBTaskCacheFlush::_OnEndCompleted;

		void SetupTestData(const bool bCharSuccess, const bool bItemSuccessd)
		{
			m_PlayerCc.BeginFlush();
			InsertData();
			SetSyncSuccess(bCharSuccess, bItemSuccessd);
		}

	protected :
		GPlayerDBCache* GetPlayerDBCache(const MUID& uidPlayer) override
		{
			return &m_PlayerCc;
		}

		void SetSyncSuccess(const bool bCharSuccess, const bool bItemSuccessd)
		{
			if (bCharSuccess)
				m_CharSync.SetSuccess();

			if (bItemSuccessd)
				m_vItemSync[0].SetSuccess();
		}		

		void InsertData()
		{
			m_PlayerCc.GetItemCcMgr().Insert(new MockDBCacheItem_FlushTask(m_uidPlayer, m_nIUID));

			SyncCharCache(m_nCID, CString("test"));
			SyncItemCache(m_nIUID, CString("test"));		
		}

	public :
		const MUID		m_uidPlayer;
		const CID		m_nCID;
		const IUID		m_nIUID;
		GPlayerDBCache	m_PlayerCc;

	};

	TEST(TestDBCacheFlush_OnEndCompleted_AllSuccess)
	{
		MockDBTaskCacheFlush_OnEndCompleted tsk;
		const bool bCharSuccess = true;
		const bool bItemSuccess = true;
		tsk.SetupTestData(bCharSuccess, bItemSuccess);

		tsk._OnEndCompleted();

		MockDBCacheChar_FlushTask* pc = (MockDBCacheChar_FlushTask*)tsk.m_PlayerCc.GetCharCc();
		CHECK(pc->m_bEnd);
		CHECK_EQUAL(bCharSuccess, pc->m_bZero);

		MockDBCacheItem_FlushTask* pi = (MockDBCacheItem_FlushTask*)tsk.m_PlayerCc.GetItemCcMgr().Find(tsk.m_nIUID);
		CHECK(pi->m_bEnd);
		CHECK_EQUAL(bItemSuccess, pi->m_bZero);
	}

	TEST(TestDBCacheFlush_OnEndCompleted_CharFail)
	{
		MockDBTaskCacheFlush_OnEndCompleted tsk;
		const bool bCharSuccess = false;
		const bool bItemSuccess = true;
		tsk.SetupTestData(bCharSuccess, bItemSuccess);

		tsk._OnEndCompleted();

		MockDBCacheChar_FlushTask* pc = (MockDBCacheChar_FlushTask*)tsk.m_PlayerCc.GetCharCc();
		CHECK(pc->m_bEnd);
		CHECK_EQUAL(bCharSuccess, pc->m_bZero);

		MockDBCacheItem_FlushTask* pi = (MockDBCacheItem_FlushTask*)tsk.m_PlayerCc.GetItemCcMgr().Find(tsk.m_nIUID);
		CHECK(pi->m_bEnd);
		CHECK_EQUAL(bItemSuccess, pi->m_bZero);
	}

	TEST(TestDBCacheFlush_OnEndCompleted_ItemFail)
	{
		MockDBTaskCacheFlush_OnEndCompleted tsk;
		const bool bCharSuccess = true;
		const bool bItemSuccess = false;
		tsk.SetupTestData(bCharSuccess, bItemSuccess);

		tsk._OnEndCompleted();

		MockDBCacheChar_FlushTask* pc = (MockDBCacheChar_FlushTask*)tsk.m_PlayerCc.GetCharCc();
		CHECK(pc->m_bEnd);
		CHECK_EQUAL(bCharSuccess, pc->m_bZero);

		MockDBCacheItem_FlushTask* pi = (MockDBCacheItem_FlushTask*)tsk.m_PlayerCc.GetItemCcMgr().Find(tsk.m_nIUID);
		CHECK(pi->m_bEnd);
		CHECK_EQUAL(bItemSuccess, pi->m_bZero);
	}

	TEST(TestDBCacheFlush_OnEndCompleted_AllFail)
	{
		MockDBTaskCacheFlush_OnEndCompleted tsk;
		const bool bCharSuccess = false;
		const bool bItemSuccess = false;
		tsk.SetupTestData(bCharSuccess, bItemSuccess);

		tsk._OnEndCompleted();

		MockDBCacheChar_FlushTask* pc = (MockDBCacheChar_FlushTask*)tsk.m_PlayerCc.GetCharCc();
		CHECK(pc->m_bEnd);
		CHECK_EQUAL(bCharSuccess, pc->m_bZero);

		MockDBCacheItem_FlushTask* pi = (MockDBCacheItem_FlushTask*)tsk.m_PlayerCc.GetItemCcMgr().Find(tsk.m_nIUID);
		CHECK(pi->m_bEnd);
		CHECK_EQUAL(bItemSuccess, pi->m_bZero);
	}
}