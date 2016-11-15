#include "stdafx.h"
#include "GDBTaskCacheFlush.h"


SUITE(TestDBCache_FailJustSkip)
{
	class MockDBTaskCacheFlush_FailQuery : public GDBTaskCacheFlush
	{
	public :
		MockDBTaskCacheFlush_FailQuery() : GDBTaskCacheFlush(0) {}

		bool IsCharSuucess()		{ return m_CharSync.IsSuccess(); }
		bool IsFirstItemSuccess()	{ return m_vItemSync[0].IsSuccess(); }
		bool IsSecondItemSuccess()	{ return m_vItemSync[1].IsSuccess(); }

		void SetupTestData(const bool bCharSuccess, const bool bFirstItemSuccess, const bool bSecondItemSuccess)
		{
			m_bCharSuccess			= bCharSuccess;
			m_bFisrtItemSuccess		= bFirstItemSuccess;
			m_bSecondItemSuccess	= bSecondItemSuccess;

			m_strCharQuery			= L"character";
			m_strFirstItemQuery		= L"item1";
			m_strSecondItemQuery	= L"item2";
			
			m_CharSync.SetData(0, m_strCharQuery);
			m_vItemSync.push_back(GItemSyncQuery(0, m_strFirstItemQuery));
			m_vItemSync.push_back(GItemSyncQuery(0, m_strSecondItemQuery));
		}

	protected :
		bool ExecuteW(mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL)
		{
			if (m_strCharQuery == strSQL)
			{
				return m_bCharSuccess;
			}
			else if (m_strFirstItemQuery == strSQL)
			{
				return m_bFisrtItemSuccess;
			}
			else if (m_strSecondItemQuery == strSQL)
			{
				return m_bSecondItemSuccess;
			}

			_ASSERT(false);
			return false;
		}

	public :
		wstring			m_strCharQuery;
		wstring			m_strFirstItemQuery;
		wstring			m_strSecondItemQuery;
		
		bool			m_bCharSuccess;
		bool			m_bFisrtItemSuccess;
		bool			m_bSecondItemSuccess;
	};

	void CHECK_SYNC_QUERY_RESULT(MockDBTaskCacheFlush_FailQuery& tsk, const bool bCharSuccess, const bool bFirstItemSuccess, const bool bSecondItemSuccess)
	{
		CHECK_EQUAL(bCharSuccess, tsk.IsCharSuucess());
		CHECK_EQUAL(bFirstItemSuccess, tsk.IsFirstItemSuccess());
		CHECK_EQUAL(bSecondItemSuccess, tsk.IsSecondItemSuccess());
	}

	TEST(TestDBCacheFlush_OnExecute_AllSuccess)
	{
		mdb::MDatabase					mockdb;
		MockDBTaskCacheFlush_FailQuery	tsk;
		const bool bCharSuccess			= true;
		const bool bFirstItemSuccess	= true;
		const bool bSecondItemSuccess	= true;
		tsk.SetupTestData(bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);

		tsk.OnExecute(mockdb);

		CHECK_SYNC_QUERY_RESULT(tsk, bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);
	}

	TEST(TestDBCacheFlush_OnExecute_CharFail)
	{
		mdb::MDatabase					mockdb;
		MockDBTaskCacheFlush_FailQuery	tsk;
		const bool bCharSuccess			= false;
		const bool bFirstItemSuccess	= true;
		const bool bSecondItemSuccess	= true;
		tsk.SetupTestData(bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);

		tsk.OnExecute(mockdb);

		CHECK_SYNC_QUERY_RESULT(tsk, bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);
	}

	TEST(TestDBCacheFlush_OnExecute_FirstItemFail)
	{
		mdb::MDatabase					mockdb;
		MockDBTaskCacheFlush_FailQuery	tsk;
		const bool bCharSuccess			= true;
		const bool bFirstItemSuccess	= false;
		const bool bSecondItemSuccess	= true;
		tsk.SetupTestData(bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);

		tsk.OnExecute(mockdb);

		CHECK_SYNC_QUERY_RESULT(tsk, bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);
	}

	TEST(TestDBCacheFlush_OnExecute_SecondItemFail)
	{
		mdb::MDatabase					mockdb;
		MockDBTaskCacheFlush_FailQuery	tsk;
		const bool bCharSuccess			= true;
		const bool bFirstItemSuccess	= true;
		const bool bSecondItemSuccess	= false;
		tsk.SetupTestData(bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);

		tsk.OnExecute(mockdb);

		CHECK_SYNC_QUERY_RESULT(tsk, bCharSuccess, bFirstItemSuccess, bSecondItemSuccess);
	}
}