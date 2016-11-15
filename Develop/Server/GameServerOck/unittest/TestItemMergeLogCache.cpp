#include "stdafx.h"
#include "GItemMergeLogCache.h"
#include "GTimeCalculator.h"


class MockItemMergeLogCache : public GItemMergeAndSplitLogCache
{
public :
	MockItemMergeLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GItemMergeAndSplitLogCache() {}

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
			CHECK_EQUAL(0, m_Log[i].m_nOwnerID);
			CHECK_EQUAL(0, m_Log[i].m_nCode);
			CHECK_EQUAL(0, m_Log[i].m_nCharPtm);
			CHECK_EQUAL(0, m_Log[i].m_nIUID);
			CHECK_EQUAL(0, m_Log[i].m_nItemID);
			CHECK_EQUAL(0, m_Log[i].m_nCurStackAmt);
			CHECK_EQUAL(0, m_Log[i].m_nDeltaStackAmt);
			CHECK_EQUAL(0, m_Log[i].m_nRefAID);
			CHECK_EQUAL(0, m_Log[i].m_nRefOwnerID);
			CHECK_EQUAL(0, m_Log[i].m_nRefIUID);
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

SUITE(TEST_ITEM_MERGE_LOG_CACHE)
{
	TEST(TEST_ITEM_MERGE_LOG_CACHE_COUNT_OVER)
	{
		MockItemMergeLogCache lc;

		for (int i = 0; i < MockItemMergeLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_ITEM_MERGE_AND_SPLIT(i + 1, i + 2, i +  3, i + 4, i + 5, i + 6, i + 7, i+8, i+9, i+10, i+11, i + 12, i + 13, i + 14, i + 15);

		lc.CheckReset();
	}

	TEST(TEST_ITEM_MERGE_LOG_CACHE_POST_1)
	{
		MockItemMergeLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.SetTime(MockItemMergeLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_ITEM_MERGE_LOG_CACHE_POST_2)
	{
		MockItemMergeLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.SetTime(MockItemMergeLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_ITEM_MERGE_LOG_CACHE_POST_3)
	{
		MockItemMergeLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.SetTime(MockItemMergeLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_ITEM_MERGE_LOG_CACHE_POST_4)
	{
		MockItemMergeLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.SetTime(MockItemMergeLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_ITEM_MERGE_LOG_CACHE_POST_5)
	{
		MockItemMergeLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());
		lc.SetTime(MockItemMergeLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_ITEM_MERGE_AND_SPLIT());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}
}
