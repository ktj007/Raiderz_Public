#include "stdafx.h"
#include "GNpcShopLogCache.h"


SUITE(TEST_NPC_SHOP_LOG_CACHE)
{
	class MockNpcShopLogCache : public GNpcShopLogCache
	{
	public :
		MockNpcShopLogCache() : m_nTime(0), m_bPost(false), m_nPostID(0), GNpcShopLogCache() {}

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

	TEST(TEST_NPC_SHOP_LOG_CACHE_CACHE_COUNT_OVER)
	{
		MockNpcShopLogCache lc;

		for (int i = 0; i < MockNpcShopLogCache::MAX_LOG_COUNT; ++i)
			lc.m_Log[i] = GLOG_DATA_NPC_SHOP(i, i, i, i, i, i, i, i, i, i, i, i, L"test");

		lc.CheckReset();
	}

	TEST(TEST_NPC_SHOP_CACHE_LOG_POST_1)
	{
		MockNpcShopLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.SetTime(MockNpcShopLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PostLog();

		CHECK(lc.m_bPost);
		CHECK_EQUAL(1, lc.m_nPostID);
	}

	TEST(TEST_NPC_SHOP_LOG_CACHE_LOG_POST_2)
	{
		MockNpcShopLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.SetTime(MockNpcShopLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_NPC_SHOP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(2, lc.m_nPostID);
	}

	TEST(TEST_NPC_SHOP_LOG_CACHE_LOG_POST_3)
	{
		MockNpcShopLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.SetTime(MockNpcShopLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_NPC_SHOP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(3, lc.m_nPostID);
	}

	TEST(TEST_NPC_SHOP_LOG_CACHE_LOG_POST_4)
	{
		MockNpcShopLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.SetTime(MockNpcShopLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_NPC_SHOP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(4, lc.m_nPostID);
	}

	TEST(TEST_NPC_SHOP_LOG_CACHE_LOG_POST_5)
	{
		MockNpcShopLogCache lc;

		lc.SetLastWriteLogingTime(0);
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.PushBack(GLOG_DATA_NPC_SHOP());
		lc.SetTime(MockNpcShopLogCache::MAX_CACHING_ELAPSED_TIME);
		lc.PushBack(GLOG_DATA_NPC_SHOP());

		CHECK(lc.m_bPost);
		CHECK_EQUAL(5, lc.m_nPostID);
	}
}