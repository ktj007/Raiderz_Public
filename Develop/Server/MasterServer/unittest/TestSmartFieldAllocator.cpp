#include "stdafx.h"
#include "ZSmartFieldAllocator.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZSharedField.h"
#include "ZDynamicFieldMaster.h"
#include "ZFitnessCalculator.h"
#include "ZSharedField.h"
#include "ZSharedFieldMaster.h"


SUITE(SmartFieldAllocator)
{
	struct FSmartFieldAllocator : FBaseServerField, FBasePlayer
	{
		FSmartFieldAllocator()
		{
			m_nTestFieldID = 110;

			InitGameServer();
			InitTestPlayer();
			InitFieldGroupInfo();
		}
		virtual ~FSmartFieldAllocator()
		{			
		}

		void InitGameServer()
		{
			m_nTestServerID = 1;
			InitDefaultGameServer(m_nTestServerID, m_nTestFieldID);
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

		void AddDummyPlayer(ZGameServerObject* pServer, int nCount)
		{
			for (int i = 0; i < nCount; ++i)
				pServer->GetPlayerMap().GetPlayerMap().insert(ZPlayerAIDMap::value_type(ZTestHelper::NewAID(), NULL));
		}

		int m_nTestServerID;
		int m_nTestFieldID;
		int m_nTestFieldGroupID;
		ZPlayer* m_pTestPlayer;
		ZSharedFieldMaster* m_pSharedFieldMaster;
	};

	TEST_FIXTURE(FSmartFieldAllocator, TestSmartFieldAllocator_AllocNewChannel1)
	{
		/// Fixture 설치
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetChannelCount());
		ZSharedField* pSharedField2 = m_pSharedFieldMaster->FindChannel(2);
		CHECK_EQUAL((ZSharedField*)NULL, pSharedField2);
		ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);

		ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(m_nTestServerID);
		CHECK_EQUAL(1, pGameServerObject->GetFieldMap().GetSharedFieldCount());

		/// SUT 실행
		fieldAllocator.AllocNewChannel(m_pSharedFieldMaster);

		/// 검증
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetChannelCount());
		pSharedField2 = m_pSharedFieldMaster->FindChannel(2);
		CHECK(NULL != pSharedField2);
		CHECK_EQUAL(2, pGameServerObject->GetFieldMap().GetSharedFieldCount());
	}

	TEST_FIXTURE(FSmartFieldAllocator, TestSmartFieldAllocator_AllocNewChannel2)
	{
		/// Fixture 설치
		int nChannelID3 = 3;
		AddSharedField(m_pSharedFieldMaster->GetFieldID(), nChannelID3, m_nTestServerID);
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetChannelCount());			///< 1, 3 채널이 존재함.
		ZSharedField* pSharedField2 = m_pSharedFieldMaster->FindChannel(2);	///< 2 채널이 없음.
		CHECK_EQUAL((ZSharedField*)NULL, pSharedField2);
		ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);

		/// SUT 실행
		fieldAllocator.AllocNewChannel(m_pSharedFieldMaster);

		/// 검증 : 2 채널이 생성됨
		CHECK_EQUAL(3, m_pSharedFieldMaster->GetChannelCount());
		pSharedField2 = m_pSharedFieldMaster->FindChannel(2);
		CHECK(NULL != pSharedField2);
	}

	TEST_FIXTURE(FSmartFieldAllocator, TestSmartFieldAllocator_AllocNewDynamicFieldGroup)
	{
		/// Fixture 설치
		MUID uidNewFieldGroup;
		ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);

		/// SUT 실행
		ZGameServerObject* pTargetServerObject = fieldAllocator.AllocNewDynamicFieldGroup(m_pTestPlayer->GetUID(), m_nTestFieldGroupID, DFT_TRIAL, &uidNewFieldGroup);

		/// 검증
		CHECK_EQUAL(1, pTargetServerObject->GetFieldMap().GetDynamicFieldMasterCount());

		ZDynamicFieldMaster* pFieldGroup = pTargetServerObject->GetFieldMap().FindDynamicFieldMaster(uidNewFieldGroup);
		CHECK_EQUAL(uidNewFieldGroup, pFieldGroup->GetUID());
		CHECK_EQUAL(pFieldGroup->GetGameServerID(), pTargetServerObject->GetID());
	}

	TEST_FIXTURE(FSmartFieldAllocator, TestSmartFieldAllocator_AllocNewDynamicFieldGroup_AllocToRequesterServer)
	{
		/// Fixture 설치
		CHECK_EQUAL(m_nTestServerID, m_pTestPlayer->GetGameServerID());

		ZGameServerObject* pServer1 = gmgr.pGameServerObjectManager->FindGameServer(m_pTestPlayer->GetGameServerID());
		MUID uidNewFieldGroup;
		ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);

		/// SUT 실행
		ZGameServerObject* pTargetServerObject = fieldAllocator.AllocNewDynamicFieldGroup(m_pTestPlayer->GetUID(), m_nTestFieldGroupID, DFT_TRIAL, &uidNewFieldGroup);

		/// 검증
		CHECK_EQUAL(pServer1->GetID(), pTargetServerObject->GetID());
	}

	TEST_FIXTURE(FSmartFieldAllocator, TestSmartFieldAllocator_AllocNewDynamicFieldGroup_AllocToAnotherServer)
	{
		/// Fixture 설치
		int nTestServerID2 = 2;
		int nTestChannelID2 = 2;	///< TestField 의 2번 채널은 2번 서버에 생성
		ZGameServerInfo* pGameServerInfo2 = AddGameServerInfo(nTestServerID2, m_nTestFieldID, 1, nTestChannelID2);
		AddGameServerObject(pGameServerInfo2);

		CHECK_EQUAL(m_nTestServerID, m_pTestPlayer->GetGameServerID());

		ZGameServerObject* pServer1 = gmgr.pGameServerObjectManager->FindGameServer(m_pTestPlayer->GetGameServerID());
		ZGameServerObject* pServer2 = gmgr.pGameServerObjectManager->FindGameServer(nTestServerID2);

		AddDummyPlayer(pServer1, 500);	///< 1번 필드에 더미 플레이어를 500명 채운다. (Fitness 값을 낮춤)

		MUID uidNewFieldGroup;
		ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);

		/// SUT 실행
		ZGameServerObject* pTargetServerObject = fieldAllocator.AllocNewDynamicFieldGroup(m_pTestPlayer->GetUID(), m_nTestFieldGroupID, DFT_TRIAL, &uidNewFieldGroup);

		/// 검증
		CHECK(pServer1->GetID() != pTargetServerObject->GetID());
		CHECK_EQUAL(pServer2->GetID(), pTargetServerObject->GetID());
	}

}