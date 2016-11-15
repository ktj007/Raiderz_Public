#include "stdafx.h"
#include "ZFieldFinder.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZSharedField.h"
#include "ZSharedFieldMaster.h"


SUITE(FieldFinder)
{
	struct FFieldFinder : FBaseServerField, FBasePlayer
	{
		FFieldFinder()
		{
			m_nTestFieldID = 110;

			InitGameServer();
			InitTestPlayer();
			InitFieldGroupInfo();
		}
		virtual ~FFieldFinder()
		{
		}
		void InitGameServer()
		{
			m_nTestServerID = 1;
			InitDefaultGameServer(m_nTestServerID, m_nTestFieldID, 2);
			m_pSharedFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(m_nTestFieldID);
		}
		void InitTestPlayer()
		{
			m_pTestPlayer = AddNewPlayerInWorld(m_nTestServerID);
		}
		void InitFieldGroupInfo()
		{
			m_nTestFieldGroupID = 1;
			AddFieldGroupInfo(m_nTestFieldGroupID, m_nTestFieldID);
		}

		ZFieldFinder m_fieldFinder;

		int m_nTestServerID;
		int m_nTestFieldID;
		int m_nTestFieldGroupID;
		ZPlayer* m_pTestPlayer;
		ZSharedFieldMaster* m_pSharedFieldMaster;
	};

	TEST_FIXTURE(FFieldFinder, TestFieldFinder_FindFirstGameServer)
	{
		/// SUT 실행
		ZGameServerObject* pGameServerObject = m_fieldFinder.FindFirstGameServer(m_nTestFieldID);

		/// 검증
		CHECK_EQUAL(m_nTestServerID, pGameServerObject->GetID());
	}

	TEST_FIXTURE(FFieldFinder, TestFieldFinder_FindEntrableSharedField)
	{
		/// Fixture 설치
		int nChannelID2 = 2;
		ZSharedField* pField = gmgr.pFieldManager->FindSharedField(m_nTestFieldID, nChannelID2);
		CHECK_EQUAL(true, pField->IsEnterable());

		/// SUT 실행
		ZSharedField* pEnterableField;
		m_fieldFinder.FindEnterableSharedField(pEnterableField, m_nTestFieldID, nChannelID2);

		/// 검증
		CHECK(NULL != pEnterableField);
		CHECK_EQUAL(pField->GetUID(), pEnterableField->GetUID());
	}

	TEST_FIXTURE(FFieldFinder, TestFieldFinder_FindEntrableSharedField_NotExistEnterableChannel)
	{
		/// Fixture 설치
		int nChannelID1 = 1;
		ZSharedField* pField1 = gmgr.pFieldManager->FindSharedField(m_nTestFieldID, nChannelID1);
		for (int i = 0; i < pField1->GetPlayerLimit(); ++i) pField1->IncreasePlayerCount();

		int nChannelID2 = 2;
		ZSharedField* pField2 = gmgr.pFieldManager->FindSharedField(m_nTestFieldID, nChannelID2);
		for (int i = 0; i < pField2->GetPlayerLimit(); ++i) pField2->IncreasePlayerCount();

		CHECK_EQUAL(false, pField1->IsEnterable());
		CHECK_EQUAL(false, pField2->IsEnterable());
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetChannelCount());

		/// SUT 실행
		ZSharedField* pEnterableField = NULL;
		m_fieldFinder.FindEnterableSharedField(pEnterableField, m_nTestFieldID, nChannelID2);

		/// 검증
		CHECK(NULL == pEnterableField);
	}

	TEST_FIXTURE(FFieldFinder, TestFieldFinder_FindEntrableSharedField_Ret_NULL_InvalidFieldID)
	{
		/// Fixture 설치
		int nInvalidFieldID = 999;

		/// SUT 실행
		ZSharedField* pEnterableField = NULL;
		m_fieldFinder.FindEnterableSharedField(pEnterableField, nInvalidFieldID, 1);

		/// 검증
		CHECK(NULL == pEnterableField);
	}

	TEST_FIXTURE(FFieldFinder, TestFieldFinder_FindGameServer_HadReservedChannel)
	{
		/// arrange
		int nServerID = 999;
		int nFieldID = 9999;
		int nFirstChannelID1 = 1;
		int nReservedChannelID2 = 2;
		int nReservedChannelID3 = 3;
		ZGameServerInfo* pGameServerInfo = AddGameServerInfo(nServerID, nFieldID, 1, nFirstChannelID1);
		pGameServerInfo->m_vecManagedSharedField[0].vecReservedChannelID.push_back(nReservedChannelID2);
		pGameServerInfo->m_vecManagedSharedField[0].vecReservedChannelID.push_back(nReservedChannelID3);

		ZGameServerObject* pGameServerObject = AddGameServerObject(pGameServerInfo);


		ZGameServerObject* pRetGameServer = NULL;
		/// 1. act - 1번 채널 예약된 게임서버 탐색.
		m_fieldFinder.FindGameServer_HadReservedChannel(nFieldID, nFirstChannelID1);
		/// 1. assert - 그런거 없음.
		CHECK_EQUAL((ZGameServerObject*)NULL, pRetGameServer);


		/// 2. act - 2번 채널 예약된 게임서버 탐색.
		pRetGameServer = m_fieldFinder.FindGameServer_HadReservedChannel(nFieldID, nReservedChannelID2);
		/// 2. assert
		CHECK_EQUAL(pGameServerObject, pRetGameServer);


		/// 3. act - 3번 채널 예약된 게임서버 탐색.
		pRetGameServer = m_fieldFinder.FindGameServer_HadReservedChannel(nFieldID, nReservedChannelID3);
		/// 3. assert
		CHECK_EQUAL(pGameServerObject, pRetGameServer);
	}

}