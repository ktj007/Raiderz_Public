#include "stdafx.h"
#include "GWarpInfoCollector.h"
#include "SUnitTestUtil.h"
#include "GTestWrapper.h"
#include "GUTHelper.h"
#include "GWarpInfoChecker.h"

SUITE(WarpInfoCollector)
{
	struct WarpInfoCheckerStub: public GWarpInfoChecker
	{
		bool m_bConditionResult;
		Result m_questResult;

		WarpInfoCheckerStub(const SENSOR_WARP_INFO&	warpInfo)
		: GWarpInfoChecker(warpInfo)
		, m_bConditionResult(false)
		, m_questResult(FAILURE)
		{}
		
		virtual bool CheckCondition(GEntityPlayer* pPlayer) const override
		{
			return m_bConditionResult;
		}

		virtual Result CheckQuestField(GEntityPlayer* pPlayer) const override
		{
			return m_questResult;
		}
	};

	struct TestWarpInfoCollector: public GWarpInfoCollector
	{
		bool m_bConditionResult;
		GWarpInfoChecker::Result m_questResult;

		TestWarpInfoCollector(): m_bConditionResult(false), m_questResult(GWarpInfoChecker::FAILURE)	
		{}

		virtual GWarpInfoChecker* NewWarpInfoChecker(const SENSOR_WARP_INFO& warpInfo) override
		{
			WarpInfoCheckerStub* pChecker = new WarpInfoCheckerStub(warpInfo);
			pChecker->m_bConditionResult = m_bConditionResult;
			pChecker->m_questResult = m_questResult;

			return pChecker;
		}
	};

	struct Fixture
	{
		TestWarpInfoCollector m_collector;
		GEntityPlayer* m_pPlayer;
		SENSOR_INFO m_sensorInfo;
	
		Fixture()
		{
			m_pPlayer = GUTHelper::NewEntityPlayer();
		}

		~Fixture()
		{
		}

		void AddSharedWarp(int nFieldID, int nMarkerID)
		{
			SENSOR_WARP_INFO warpInfo;
			warpInfo.nFieldID = nFieldID;
			warpInfo.nMarkerID = nMarkerID;
			warpInfo.eGateType = TGT_AUTO;
			
			vector<SENSOR_WARP_INFO>& vecWarpInfo = m_sensorInfo.vecWarpInfos;
			vecWarpInfo.push_back(warpInfo);
		}

		void AddTrialWarp(int nFieldGroupID, GATE_TYPE eGateType, int nQuestID)
		{
			SENSOR_WARP_INFO warpInfo;
			warpInfo.nFieldGroupID = nFieldGroupID;
			warpInfo.eGateType = eGateType;
			warpInfo.nQuestID = nQuestID;

			vector<SENSOR_WARP_INFO>& vecWarpInfo = m_sensorInfo.vecWarpInfos;
			vecWarpInfo.push_back(warpInfo);
		}

		void SetCheckerConditionResult(bool bConditionResult)
		{
			m_collector.m_bConditionResult = bConditionResult;
		}

		void SetCheckerQuestResult(GWarpInfoChecker::Result questResult)
		{
			m_collector.m_questResult = questResult;
		}

		void AddCheckerFromWarp(int nIndex, bool isFullHouse)
		{
			vector<SENSOR_WARP_INFO>& vecWarpInfo = m_sensorInfo.vecWarpInfos;
			VALID(0 <= nIndex);
			VALID(nIndex < (int)vecWarpInfo.size());

			GWarpInfoChecker* pWarpInfoChecker = new GWarpInfoChecker(vecWarpInfo[nIndex]);
			m_collector.AddWarpInfoChecker(pWarpInfoChecker, isFullHouse);
		}
	};

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_NoWarp)
	{
		// arrange
		// do nothing

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);
		
		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::NOTHING);		
	}

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_SharedWarp)
	{
		// arrange
		AddSharedWarp(1, 1);		
		SetCheckerConditionResult(true);

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);
		
		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::SINGLE);
	}

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_SharedMultyWarp)
	{
		// arrange
		AddSharedWarp(1, 1);
		AddSharedWarp(2, 1);
		SetCheckerConditionResult(true);

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);
		
		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::MULTY);
	}

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_TrialWarp)
	{
		// arrange		
		AddSharedWarp(1, 1);	// 진입 가능한 트라이얼 워프가 있으면 공유필드 워프는 무시된다.

		AddTrialWarp(1, TGT_AUTO, 1);
		SetCheckerConditionResult(true);
		SetCheckerQuestResult(GWarpInfoChecker::SUCCESS);		 

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);

		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::SINGLE);
	}

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_MultyTrialWarp)
	{
		// arrange
		AddTrialWarp(1, TGT_AUTO, 1);
		AddTrialWarp(2, TGT_AUTO, 2);
		SetCheckerConditionResult(true);
		SetCheckerQuestResult(GWarpInfoChecker::SUCCESS);

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);

		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::MULTY);
	}

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_FullHouse)
	{
		// arrange
		AddSharedWarp(1, 1);	// 트라이얼필드가 만원이라도 있으면 공유필드 워프는 무시된다.

		AddTrialWarp(1, TGT_AUTO, 1);		
		SetCheckerConditionResult(true);
		SetCheckerQuestResult(GWarpInfoChecker::FULLHOUSE);

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);

		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::FULLHOUSE);
	}

	TEST_FIXTURE(Fixture, TestCollectAutoWarp_MultyFullHouse)
	{
		// arrange
		AddTrialWarp(1, TGT_AUTO, 1);		
		AddTrialWarp(2, TGT_AUTO, 2);
		SetCheckerConditionResult(true);
		SetCheckerQuestResult(GWarpInfoChecker::FULLHOUSE);

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectAutoWarp(m_sensorInfo, m_pPlayer);

		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::MULTY);
	}

	TEST_FIXTURE(Fixture, TestCollectManualWarp)
	{
		// arrange
		AddSharedWarp(1, 1);
		AddTrialWarp(1, TGT_AUTO, 1);

		AddTrialWarp(2, TGT_MANUAL, 2);
		SetCheckerConditionResult(true);
		SetCheckerQuestResult(GWarpInfoChecker::SUCCESS);

		// act
		GWarpInfoCollector::Result collectResult = m_collector.CollectManualWarp(m_sensorInfo, m_pPlayer);

		// assert
		CHECK_EQUAL(collectResult, GWarpInfoCollector::SINGLE);
	}

	TEST_FIXTURE(Fixture, TestExtractWarpField_EmptyWarp)
	{
		// arrange
		// do nothing

		// act
		GWarpInfoCollector::Result extractResult = m_collector.ExtractWarpField();
		
		// assert
		CHECK_EQUAL(extractResult, GWarpInfoCollector::NOTHING);
	}

	TEST_FIXTURE(Fixture, TestExtractWarpField_FullHouseField)
	{
		// arrange
		AddTrialWarp(1, TGT_AUTO, 1);
		AddCheckerFromWarp(0, true);

		// act
		GWarpInfoCollector::Result extractResult = m_collector.ExtractWarpField();

		// assert
		CHECK_EQUAL(extractResult, GWarpInfoCollector::FULLHOUSE);
	}

	TEST_FIXTURE(Fixture, TestExtractWarpField_SameFieldFullHouse)
	{
		// arrange
		AddTrialWarp(1, TGT_AUTO, 1);
		AddTrialWarp(1, TGT_AUTO, 2);
		AddCheckerFromWarp(0, false);
		AddCheckerFromWarp(1, true);

		// act
		GWarpInfoCollector::Result extractResult = m_collector.ExtractWarpField();

		// assert
		CHECK_EQUAL(extractResult, GWarpInfoCollector::SINGLE);
	}

	TEST_FIXTURE(Fixture, TestExtractWarpField_SameFieldSharedWarp)
	{
		// arrange
		AddSharedWarp(1, 2);
		AddSharedWarp(1, 2);
		AddSharedWarp(1, 2);
		AddCheckerFromWarp(0, false);
		AddCheckerFromWarp(1, false);
		AddCheckerFromWarp(2, false);

		// act
		GWarpInfoCollector::Result extractResult = m_collector.ExtractWarpField();
		bool isSharedWarp = m_collector.IsSharedWarp(0);
		int nFieldID = m_collector.GetFieldID(0);
		int nMarkerID = m_collector.GetMarkerID(0);

		// assert
		CHECK_EQUAL(extractResult, GWarpInfoCollector::SINGLE);		
		CHECK(isSharedWarp);
		CHECK_EQUAL(nFieldID, 1);
		CHECK_EQUAL(nMarkerID, 2);
	}

	TEST_FIXTURE(Fixture, TestExtractWarpField_SameFieldTrialWarp)
	{
		// arrange
		AddTrialWarp(1, TGT_AUTO, 1);
		AddTrialWarp(1, TGT_AUTO, 2);
		AddTrialWarp(1, TGT_AUTO, 3);
		AddCheckerFromWarp(0, false);
		AddCheckerFromWarp(1, false);
		AddCheckerFromWarp(2, false);

		// act
		GWarpInfoCollector::Result extractResult = m_collector.ExtractWarpField();		
		bool isTrialWarp = m_collector.IsTrialWarp(0);
		int nFieldGroupID = m_collector.GetFieldGroupID(0);

		// assert
		ASSERT_EQUAL(extractResult, GWarpInfoCollector::SINGLE);
		CHECK(isTrialWarp);
		CHECK_EQUAL(nFieldGroupID, 1);
	}

	TEST_FIXTURE(Fixture, TestExtractWarpField_MultyField)
	{
		// arrange
		AddTrialWarp(1, TGT_AUTO, 1);
		AddTrialWarp(2, TGT_AUTO, 2);
		AddCheckerFromWarp(0, false);
		AddCheckerFromWarp(1, false);

		// act
		GWarpInfoCollector::Result extractResult = m_collector.ExtractWarpField();		
		bool isTrialWarp = m_collector.IsTrialWarp(1);
		int nFieldGroupID = m_collector.GetFieldGroupID(1);
		
		// assert
		CHECK_EQUAL(extractResult, GWarpInfoCollector::MULTY);
		CHECK(isTrialWarp);
		CHECK_EQUAL(nFieldGroupID, 2);
	}
}
