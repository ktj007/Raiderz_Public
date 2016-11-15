#include "stdafx.h"
#include "GItemLogCache.h"
#include "GLogCacheData.h"
#include "GTimeCalculator.h"


class MockItemLootLogCache : public GItemLogCache
{
public :
	MockItemLootLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GItemLogCache() {}

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
			CHECK_EQUAL(0, m_Log[i].m_nIUID);
			CHECK_EQUAL(0, m_Log[i].m_nItemID);
			CHECK_EQUAL(0, m_Log[i].m_nCurStackAmt);
			CHECK_EQUAL(0, m_Log[i].m_nDeltaStackAmt);
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

SUITE(TEST_ITEM_LOOT_LOG_CACHE)
{
	TEST(TEST_ITEM_LOOT_LOG_CACHE_COUNT_OVER)
	{
		MockItemLootLogCache lc;

		for (int i = 0; i < MockItemLootLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_ITEM(i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, GTimeCalculator::GetTimeAsString(GTimeSec(i)), i+12);
		
		
		lc.CheckReset();
	}

	TEST(TEST_ITEM_LOOT_LOG_POST_1)
	{
		MockItemLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM());
		lc.SetTime(GItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_ITEM_LOOT_LOG_POST_2)
	{
		MockItemLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM());
		lc.SetTime(GItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM());
		
		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_ITEM_LOOT_LOG_POST_3)
	{
		MockItemLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM());
		lc.PushBack(GLOG_DATA_ITEM());
		lc.SetTime(GItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_ITEM_LOOT_LOG_POST_4)
	{
		MockItemLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM());
		lc.PushBack(GLOG_DATA_ITEM());
		lc.PushBack(GLOG_DATA_ITEM());
		lc.SetTime(GItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_ITEM_LOOT_LOG_POST_5)
	{
		MockItemLootLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM());
		lc.PushBack(GLOG_DATA_ITEM());
		lc.PushBack(GLOG_DATA_ITEM());
		lc.PushBack(GLOG_DATA_ITEM());
		lc.SetTime(GItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}	
}
