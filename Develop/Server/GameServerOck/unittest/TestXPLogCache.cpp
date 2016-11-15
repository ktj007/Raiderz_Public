#include "stdafx.h"
#include "GXPLogCache.h"
#include "GTimeCalculator.h"


SUITE(TEST_XP_LOG_CACHE)
{
	class MockXPLogCache : public GXPLogCache
	{
	public :
		MockXPLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GXPLogCache() {}

		void SetTime(uint32 nTime)
		{
			m_nTime = nTime;
		}

		void SetLastWriteLogingTime(uint32 nTime)
		{
			m_nLastWriteLogingTime = nTime;
		}

		void CheckReset()
		{
			m_nIdx = MAX_LOG_COUNT;
			Reset();

			for (size_t i = 0; i < GXPLogCache::MAX_LOG_COUNT; ++i)
			{
				CHECK_EQUAL(0, m_Log[i].m_nAID);
				CHECK_EQUAL(0, m_Log[i].m_nCID);
				CHECK_EQUAL(0, m_Log[i].m_nCharPtm);
				CHECK_EQUAL(0, m_Log[i].m_nLevel);
				CHECK_EQUAL(0, m_Log[i].m_nCurXP);
				CHECK_EQUAL(0, m_Log[i].m_nDeltaXP);
				CHECK(L"NULL" == m_Log[i].m_strRegDate);
			}
		}

	protected :
		uint32 GetTime() override { return m_nTime; }
		void OnPostLog() override 
		{ 
			m_bPost = true; 
			m_nPostID = m_nIdx;

			PostString();
		}

	public :
		using GLogCache::m_Log;

		uint32	m_nTime;
		bool	m_bPost;
		int		m_nPostID;
	};

	TEST(TEST_XP_LOG_CACHE_COUNT_OVER)
	{
		MockXPLogCache lc;

		for (int i = 0; i < MockXPLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_XP(i, i, i, i, i, i, i, i, GTimeCalculator::GetTimeAsString(GTimeSec(0)), i);

		lc.CheckReset();
	}

	TEST(TEST_XP_LOG_POST_1)
	{
		MockXPLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_XP());
		lc.SetTime(MockXPLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_XP_LOG_POST_2)
	{
		MockXPLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_XP());
		lc.SetTime(MockXPLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_XP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_XP_LOG_POST_3)
	{
		MockXPLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_XP());
		lc.PushBack(GLOG_DATA_XP());
		lc.SetTime(MockXPLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_XP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_XP_LOG_POST_4)
	{
		MockXPLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_XP());
		lc.PushBack(GLOG_DATA_XP());
		lc.PushBack(GLOG_DATA_XP());
		lc.SetTime(MockXPLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_XP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_XP_LOG_POST_5)
	{
		MockXPLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_XP());
		lc.PushBack(GLOG_DATA_XP());
		lc.PushBack(GLOG_DATA_XP());
		lc.PushBack(GLOG_DATA_XP());
		lc.SetTime(MockXPLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_XP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}
}


