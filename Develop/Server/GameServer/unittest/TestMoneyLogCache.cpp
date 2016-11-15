#include "stdafx.h"
#include "GMoneyLogCache.h"
#include "GLogCacheData.h"

class MockMoneyLootLogCache : public GMoneyLogCache
{
public :
	MockMoneyLootLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GMoneyLogCache() {}

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
			CHECK(L"NULL"== m_Log[i].m_strRegDate);
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

SUITE(TEST_MONEY_LOOT_LOG_CACHE)
{
	TEST(TEST_MONEY_LOOT_LOG_CACHE_COUNT_OVER)
	{
		MockMoneyLootLogCache lc;

		for (int i = 0; i < MockMoneyLootLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_MONEY(i, i, i, i, i, i, i, i, L"test", i);


		lc.CheckReset();
	}

	TEST(TEST_MONEY_LOOT_LOG_POST_1)
	{
		MockMoneyLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_MONEY());
		lc.SetTime(MockMoneyLootLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_MONEY_LOOT_LOG_POST_2)
	{
		MockMoneyLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_MONEY());
		lc.SetTime(MockMoneyLootLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_MONEY());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_MONEY_LOOT_LOG_POST_3)
	{
		MockMoneyLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_MONEY());
		lc.PushBack(GLOG_DATA_MONEY());
		lc.SetTime(MockMoneyLootLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_MONEY());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_MONEY_LOOT_LOG_POST_4)
	{
		MockMoneyLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_MONEY());
		lc.PushBack(GLOG_DATA_MONEY());
		lc.PushBack(GLOG_DATA_MONEY());
		lc.SetTime(MockMoneyLootLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_MONEY());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_MONEY_LOOT_LOG_POST_5)
	{
		MockMoneyLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_MONEY());
		lc.PushBack(GLOG_DATA_MONEY());
		lc.PushBack(GLOG_DATA_MONEY());
		lc.PushBack(GLOG_DATA_MONEY());
		lc.SetTime(MockMoneyLootLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_MONEY());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}
}