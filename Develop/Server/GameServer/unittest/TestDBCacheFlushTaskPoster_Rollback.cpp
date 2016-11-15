#include "stdafx.h"
#include "GDBCacheFlush.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_FlushRollback)
{
	class MockDBCacheFlushTaskPoster_Rollback : public GDBCacheFlush
	{
	public :
		MockDBCacheFlushTaskPoster_Rollback(GPlayerDBCache* pc = new GPlayerDBCache(0, 0)) 
			: GDBCacheFlush(pc), m_bBuildCacheChar(false), m_bBuildCacheItem(false), m_bPostTask(false), m_bCallRollback(false)
		, m_pc(pc) {}
		~MockDBCacheFlushTaskPoster_Rollback()
		{
			SAFE_DELETE(m_pc);
		}

		void SetReturn(const bool bBuildCacheChar, const bool bBuildCacheItem, const bool bPostTask)
		{
			m_bBuildCacheChar = bBuildCacheChar;
			m_bBuildCacheItem = bBuildCacheItem;
			m_bPostTask = bPostTask;
		}

		bool BuidDBCacheChar(GCharacterDBCache* cc) override
		{
			return m_bBuildCacheChar;
		}

		bool BuldDBCacheItem(GPlayerDBCache* pc) override
		{
			return m_bBuildCacheItem;
		}

		bool PostTask(GPlayerDBCache* pc, GDBTaskCacheFlush* pTask) override
		{
			return m_bPostTask;
		}

		void Rollback(GPlayerDBCache* pc) override
		{
			m_bCallRollback = true;
		}

	public :
		bool			m_bBuildCacheChar;
		bool			m_bBuildCacheItem;
		bool			m_bPostTask;
		bool			m_bCallRollback;
		GPlayerDBCache* m_pc;
	};

	TEST(TestCacheFlushTaskPoster_Rollback_BuildDBCacheChar)
	{
		MockDBCacheFlushTaskPoster_Rollback poster;
		poster.SetReturn(false, false, false);

		poster.Post();

		CHECK(poster.m_bCallRollback);
	}

	TEST(TestCacheFlushTaskPoster_Rollback_BuildDBCacheItem)
	{
		MockDBCacheFlushTaskPoster_Rollback poster;
		poster.SetReturn(true, false, false);

		poster.Post();

		CHECK(poster.m_bCallRollback);
	}

	TEST(TestCacheFlushTaskPoster_Rollback_PostTask)
	{
		MockDBCacheFlushTaskPoster_Rollback poster;
		poster.SetReturn(true, false, false);

		poster.Post();

		CHECK(poster.m_bCallRollback);
	}
}