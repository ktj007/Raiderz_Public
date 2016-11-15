#include "stdafx.h"
#include "GQuestItemLogCache.h"

SUITE(TEST_QPVP_REWARD_ITEM_LOG_CACHE)
{
	class MockQPvPRewardItemLogCache : public GQuestItemLogCache
	{
	public :
		MockQPvPRewardItemLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GQuestItemLogCache() {}

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
				CHECK_EQUAL(0, m_Log[i].m_nCode);
				CHECK_EQUAL(0, m_Log[i].m_nCharPtm);
				CHECK(L"NULL" == m_Log[i].m_strCurMoney);
				CHECK(L"NULL" == m_Log[i].m_strDeltaMoney);
				CHECK(L"NULL" == m_Log[i].m_strIUID);
				CHECK(L"NULL" == m_Log[i].m_strItemID);
				CHECK(L"NULL" == m_Log[i].m_strCurStackAmt);
				CHECK(L"NULL" == m_Log[i].m_strDeltaStackAmt);
				CHECK(L"NULL" == m_Log[i].m_strRegDate);
				CHECK(L"NULL" == m_Log[i].m_strRefMailUID);
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

	TEST(TEST_QPVP_REWARD_ITEM_LOG_CACHE_CACHE_COUNT_OVER)
	{
		MockQPvPRewardItemLogCache lc;

		for (int i = 0; i < MockQPvPRewardItemLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_QUEST_ITEM(i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, i+12, L"test", i+13, i+14, i+15);

		lc.CheckReset();
	}

	TEST(TEST_QPVP_REWARD_ITEM_LOG_POST_1)
	{
		MockQPvPRewardItemLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.SetTime(MockQPvPRewardItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_QPVP_REWARD_ITEM_LOG_POST_2)
	{
		MockQPvPRewardItemLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.SetTime(MockQPvPRewardItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_QPVP_REWARD_ITEM_LOG_POST_3)
	{
		MockQPvPRewardItemLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.SetTime(MockQPvPRewardItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_QPVP_REWARD_ITEM_LOG_POST_4)
	{
		MockQPvPRewardItemLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.SetTime(MockQPvPRewardItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_QPVP_REWARD_ITEM_LOG_POST_5)
	{
		MockQPvPRewardItemLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.PushBack(GLOG_DATA_QUEST_ITEM());
		lc.SetTime(MockQPvPRewardItemLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_QUEST_ITEM());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}	
}
