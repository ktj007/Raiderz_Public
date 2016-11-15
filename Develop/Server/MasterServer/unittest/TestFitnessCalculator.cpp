#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZFitnessCalculator.h"
#include "ZGameServerObject.h"
#include "ZConst.h"
#include "ZSharedField.h"


SUITE(FitnessCalculator)
{
	struct Fixture : public FBaseServerField
	{
		ZGameServerObject* m_pGameServerObject;
		ZGameServerObject* m_pGameServerObject2;

		Fixture()
		{
			InitGameServer();
		}
		~Fixture()
		{
			ClearGameServerMaps();
		}
		void InitGameServer()
		{
			/// 1번 서버 생성
			int nDefaultDummyFieldID = ZTestHelper::NewID();
			AddFieldInfo(nDefaultDummyFieldID);
			m_pGameServerObject = AddGameServerObject(1, nDefaultDummyFieldID);

			/// 디폴트 더미 필드 제거
			ZSharedField* pDefaultDummyField = gmgr.pFieldManager->FindSharedField(nDefaultDummyFieldID, 1);
			m_pGameServerObject->GetFieldMap().DelSharedField(pDefaultDummyField->GetUID());
			gmgr.pFieldManager->DestroySharedField(pDefaultDummyField->GetUID());

			/// 2번 서버 생성
			nDefaultDummyFieldID = ZTestHelper::NewID();
			AddFieldInfo(nDefaultDummyFieldID);
			m_pGameServerObject2 = AddGameServerObject(2, nDefaultDummyFieldID);
		}

		void AddDummySharedField(int nCount)
		{
			for (int i = 0; i < nCount; ++i)
				m_pGameServerObject->GetFieldMap().GetSharedFieldMap().insert(ZFieldManager::ZSharedFieldMap::value_type(ZTestHelper::NewUID(), NULL));
		}
		void AddDummyDynamicField(int nCount)
		{
			for (int i = 0; i < nCount; ++i)
				m_pGameServerObject->GetFieldMap().GetDynamicFieldMasterMap().insert(ZFieldManager::ZDynamicFieldMasterMap::value_type(ZTestHelper::NewUID(), NULL));
		}
		void AddDummyPlayer(int nCount)
		{
			for (int i = 0; i < nCount; ++i)
				m_pGameServerObject->GetPlayerMap().GetPlayerMap().insert(ZPlayerAIDMap::value_type(ZTestHelper::NewAID(), NULL));
		}
		void ClearGameServerMaps()
		{
			m_pGameServerObject->GetFieldMap().GetSharedFieldMap().clear();
			m_pGameServerObject->GetFieldMap().GetDynamicFieldMasterMap().clear();
		}

		int TestFitnessCalculator_Shared(int nPlayerCount, int nFieldCount)
		{
			float fTempPlayerRate	= 0.7f * (1.0f - ((float)nPlayerCount / (float)ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT));
			float fTempFieldRate	= 0.3f * (1.0f - ((float)nFieldCount / (float)gmgr.pFieldManager->GetSharedFieldCount()));
			float fFitnessMod = fTempPlayerRate + fTempFieldRate;
			int nExpectedValue = (int)((float)MAX_FITNESS_VALUE_TO_ALLOC_FIELD * fFitnessMod);

			return nExpectedValue;
		}

		int TestFitnessCalculator_Dynamic(int nPlayerCount, int nFieldGroupCount)
		{
			float fTempPlayerRate	= 0.7f * (1.0f - ((float)nPlayerCount / (float)ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT));
			float fTempFieldRate	= 0.3f * (1.0f - ((float)nFieldGroupCount / (float)ZConst::GAME_SERVER_DYNAMIC_FIELD_COUNT_LIMIT));
			float fFitnessMod = fTempPlayerRate + fTempFieldRate;
			int nExpectedValue = (int)((float)MAX_FITNESS_VALUE_TO_ALLOC_FIELD * fFitnessMod);

			return nExpectedValue;
		}
	};

	TEST_FIXTURE(Fixture, TestFitnessCalculator_Fixture)
	{
		CHECK(NULL != m_pGameServerObject);
	}

	namespace SharedField
	{
		TEST_FIXTURE(Fixture, TestCalcToCreateChannel_CleanGameServer)
		{
			int nFitnessValue = ZFitnessCalculator::CalcToCreateChannel(m_pGameServerObject);

			CHECK_EQUAL(100, nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateChannel_Calculate)
		{
			/// Fixture 설치
			int nPlayerCount = 1;
			int nSharedFieldCount = 1;

			AddDummyPlayer(nPlayerCount);
			AddDummySharedField(nSharedFieldCount);

			int nExpectedValue = TestFitnessCalculator_Shared(nPlayerCount, nSharedFieldCount);

			/// SUT 실행
			int nFitnessValue = ZFitnessCalculator::CalcToCreateChannel(m_pGameServerObject);

			/// 검증
			CHECK_EQUAL(nExpectedValue, nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateChannel_Calculate2)
		{
			/// Fixture 설치
			int nPlayerCount = 2;
			int nSharedFieldCount = 3;

			AddDummyPlayer(nPlayerCount);
			AddDummySharedField(nSharedFieldCount);

			int nExpectedValue = TestFitnessCalculator_Shared(nPlayerCount, nSharedFieldCount);

			/// SUT 실행
			int nFitnessValue = ZFitnessCalculator::CalcToCreateChannel(m_pGameServerObject);

			/// 검증
			CHECK_EQUAL(nExpectedValue,	nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateChannel_MinBoundaryCheck)
		{
			// GameServer 에 플레이어가 꽉차있을 겨우.
			AddDummyPlayer(ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT);
			AddDummySharedField(1);

			int nFitnessValue = ZFitnessCalculator::CalcToCreateChannel(m_pGameServerObject);

			CHECK_EQUAL(0,	nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateChannel_MinBoundaryCheck2)
		{
			// GameServer 에 플레이어가 100명 부족하게 차있고, 공유필드 역시 꽉차있을 경우 Fitness 값은 0
			AddDummyPlayer(ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT - 100);
			AddDummySharedField(1);

			int nFitnessValue = ZFitnessCalculator::CalcToCreateChannel(m_pGameServerObject);

			CHECK(0 < nFitnessValue);
		}

	} ///< namespace SharedField

	namespace DynamicField
	{
		TEST_FIXTURE(Fixture, TestCalcToCreateFieldGroup_CleanGameServer)
		{
			int nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject);

			CHECK_EQUAL(100, nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateFieldGroup_Calculate)
		{
			/// Fixture 설치
			int nPlayerCount = 1;
			int nDynamicFieldCount = 1;

			AddDummyPlayer(nPlayerCount);
			AddDummyDynamicField(nDynamicFieldCount);

			int nExpectedValue = TestFitnessCalculator_Dynamic(nPlayerCount, nDynamicFieldCount);

			/// SUT 실행
			int nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject);

			/// 검증
			CHECK_EQUAL(nExpectedValue,	nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateFieldGroup_Calculate2)
		{
			/// Fixture 설치
			int nPlayerCount = 2;
			int nDynamicFieldCount = 3;

			AddDummyPlayer(nPlayerCount);
			AddDummyDynamicField(nDynamicFieldCount);

			int nExpectedValue = TestFitnessCalculator_Dynamic(nPlayerCount, nDynamicFieldCount);

			/// SUT 실행
			int nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject);

			/// 검증
			CHECK_EQUAL(nExpectedValue,	nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateFieldGroup_Additional)
		{
			int nPlayerCount = 500;
			int nDynamicFieldCount = 50;

			AddDummyPlayer(nPlayerCount);
			AddDummyDynamicField(nDynamicFieldCount);

			int nExpectedValue = TestFitnessCalculator_Dynamic(nPlayerCount, nDynamicFieldCount);

			/// SUT 실행
			int nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject);

			/// 검증
			CHECK_EQUAL(nExpectedValue,	nFitnessValue);


			/// SUT 실행
			int nAdditionalValue = 10;
			nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject, nAdditionalValue);

			/// 검증
			CHECK_EQUAL(nExpectedValue + nAdditionalValue,	nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateFieldGroup_MaxBoundaryCheck)
		{
			// Dynamic Field 가 없고 Player 가 없을 때 Fitness 값은 100 점. (100점 = ZConst::MAX_FITNESS_VALUE_TO_ALLOC_DYNAMIC_FIELD)
			// 거기에 10점을 추가로 더하면 110이 돼야 하지만 최대값은 100 점을 넘지 않는다.

			int nAdditionalValue = 10;
			int nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject, nAdditionalValue);

			CHECK_EQUAL(MAX_FITNESS_VALUE_TO_ALLOC_FIELD,	nFitnessValue);
		}

		TEST_FIXTURE(Fixture, TestCalcToCreateFieldGroup_MinBoundaryCheck)
		{
			// GameServer 에 플레이어가 꽉차있고, 동적필드 역시 꽉차있을 경우 Fitness 값은 0
			AddDummyDynamicField(ZConst::GAME_SERVER_DYNAMIC_FIELD_COUNT_LIMIT);
			AddDummyPlayer(ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT);

			int nFitnessValue = ZFitnessCalculator::CalcToCreateFieldGroup(m_pGameServerObject);

			CHECK_EQUAL(0,	nFitnessValue);
		}
	}

}
