#include "stdafx.h"
#include "GCraftLogCache.h"

class MockCraftLogCache : public GCraftLogCache
{
public :
	MockCraftLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GCraftLogCache() {}

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

		for (size_t i = 0; i < MAX_LOG_COUNT; ++i)
		{
			CHECK_EQUAL(0, m_Log[i].m_nAID);
			CHECK_EQUAL(0, m_Log[i].m_nCID);
			CHECK_EQUAL(0, m_Log[i].m_nCharPtm);
			CHECK_EQUAL(0, m_Log[i].m_nCurMoney);
			CHECK_EQUAL(0, m_Log[i].m_nDeltaMoney);
			CHECK_EQUAL(0, m_Log[i].m_nIUID);
			CHECK_EQUAL(0, m_Log[i].m_nItemID);
			CHECK_EQUAL(0, m_Log[i].m_nCurStackAmt);
			CHECK_EQUAL(0, m_Log[i].m_nDeltaStackAmt);
			CHECK(L"NULL" == m_Log[i].m_strRegDate);
			CHECK(L"NULL" == m_Log[i].m_strRefNpcID);
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

SUITE(TEST_CRAFT_LOG_CACHE)
{
	TEST(TEST_CRAFT_LOG_CACHE_COUNT_OVER)
	{
		MockCraftLogCache lc;

		for (int i = 0; i < MockCraftLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_CRAFT(i, i, i, i, i, i, i, i, i, i, i, i, L"test", i, i);

		lc.CheckReset();
	}

	TEST(TEST_CRAFT_LOG_POST_1)
	{
		MockCraftLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.SetTime(MockCraftLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_CRAFT_LOG_POST_2)
	{
		MockCraftLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.SetTime(MockCraftLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_CRAFT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_CRAFT_LOG_POST_3)
	{
		MockCraftLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.SetTime(MockCraftLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_CRAFT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_CRAFT_LOG_POST_4)
	{
		MockCraftLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.SetTime(MockCraftLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_CRAFT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_CRAFT_LOG_POST_5)
	{
		MockCraftLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.PushBack(GLOG_DATA_CRAFT());
		lc.SetTime(MockCraftLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_CRAFT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}
}