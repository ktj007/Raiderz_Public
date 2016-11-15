#include "stdafx.h"
#include "ZLoginFieldGuide.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZFieldManager.h"
#include "ZSharedField.h"
#include "ZDynamicFieldMaster.h"


SUITE(LoginFieldGuide)
{
	struct FLoginFieldGuide : FBaseServerField, FBasePlayer
	{
		FLoginFieldGuide()
		{
			m_nTestSharedFieldID = 100;

			InitGameServer();
			InitFirstEnterSharedField();
			InitTestPlayer();
			InitFieldGroup();
		}
		~FLoginFieldGuide() {}
		void InitGameServer()
		{
			m_nTestServerID = 1;
			InitDefaultGameServer(m_nTestServerID, m_nTestSharedFieldID);

			m_pTestSharedField = gmgr.pFieldManager->FindSharedField(m_nTestSharedFieldID, PRIME_CHANNELID);
		}
		void InitFirstEnterSharedField()
		{
			AddFieldInfo(FIRST_ENTER_FIELD_ID);
			AddSharedField(FIRST_ENTER_FIELD_ID, 1, m_nTestServerID);
		}
		void InitTestPlayer()
		{
			m_pTestPlayer = AddNewPlayerInWorld(m_nTestServerID);
		}
		void InitFieldGroup()
		{
			m_nTestDynamicFieldID = ZTestHelper::NewID();
			AddFieldInfo(m_nTestDynamicFieldID, L"TestField", L"TestFIeld", true);

			m_nTestFieldGroupID = ZTestHelper::NewID();
			AddFieldGroupInfo(m_nTestFieldGroupID, m_nTestDynamicFieldID);
			m_pTestDynamicFieldMaster = AddFieldGroup(m_nTestFieldGroupID, m_pTestPlayer->GetUID());
			m_pTestDynamicField = m_pTestDynamicFieldMaster->FindField(m_nTestDynamicFieldID);
		}

		ZLoginFieldGuide::PLAYER_FIELD_INFO Make_PLAYER_FIELD_INFO(int nSharedFieldID, MUID uidFieldGroup = MUID::ZERO, int nDynamicFieldID = INVALID_ID)
		{
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo;
			stPlayerFieldInfo.nSharedFieldID		= nSharedFieldID;
			stPlayerFieldInfo.uidDynamicFieldGroup	= uidFieldGroup;
			stPlayerFieldInfo.nDynamicFieldID		= nDynamicFieldID;
			return stPlayerFieldInfo;
		}
		void SetSharedFieldFull(ZSharedField* pTestSharedField)
		{
			for (int i = 0; i < pTestSharedField->GetPlayerLimit(); ++i)
				pTestSharedField->IncreasePlayerCount();
		}

		int						m_nTestServerID;
		
		int						m_nTestSharedFieldID;
		ZSharedField*			m_pTestSharedField;

		int						m_nTestDynamicFieldID;
		ZDynamicField*			m_pTestDynamicField;
		int						m_nTestFieldGroupID;
		ZDynamicFieldMaster*	m_pTestDynamicFieldMaster;
		
		ZPlayer*				m_pTestPlayer;
	};

	TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Fixture)
	{
		CHECK_EQUAL(2, gmgr.pFieldManager->GetSharedFieldCount());			///< 1. FirstEnterField, 2. TestField
		CHECK_EQUAL(1, gmgr.pFieldManager->GetDynamicFieldMasterCount());	///< TestFieldGroup
	}

	namespace Positive
	{
		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_FirstLogin)
		{
			/// Fixture 설치
			int nSharedFieldID_WhenFirstLogin = 0;
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(nSharedFieldID_WhenFirstLogin);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			ASSERT_CHECK((ZGameServerObject*)NULL	!= pDestServer);
			CHECK_EQUAL(m_nTestServerID,			pDestServer->GetID());

			ASSERT_CHECK((ZField*)NULL				!= pDestField);
			CHECK_EQUAL(FIRST_ENTER_FIELD_ID,		pDestField->GetID());
		}

		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_DynamicField)
		{
			/// Fixture 설치
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(m_nTestSharedFieldID, m_pTestDynamicFieldMaster->GetUID(), m_nTestDynamicFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(m_nTestServerID, pDestServer->GetID());
			CHECK_EQUAL(m_nTestDynamicFieldID, pDestField->GetID());
		}

		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_SharedField)
		{
			/// Fixture 설치
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(m_nTestSharedFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			ZGameServerObject* pExpectedServer = gmgr.pGameServerObjectManager->FindGameServer(m_nTestServerID);
			CHECK_EQUAL(pExpectedServer, pDestServer);

			ZField* pExpectedField = reinterpret_cast<ZField*>(gmgr.pFieldManager->FindSharedField(m_nTestSharedFieldID, PRIME_CHANNELID));
			CHECK_EQUAL(pExpectedField, pDestField);
		}

		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_SharedField_DestSharedFIeldIsFull_Should_ReturnTRUE_n_DestInfoisNULL)
		{
			/// Fixture 설치
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(m_nTestSharedFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			SetSharedFieldFull(m_pTestSharedField);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL((ZGameServerObject*)NULL,	pDestServer);
			CHECK_EQUAL((ZField*)NULL,				pDestField);
		}

		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_NotExistFieldGroupUID_DestFieldIsSharedField)
		{
			/// Fixture 설치
			int nNotExistFieldID = 999;
			MUID uidNotExistFieldGroupUID = 999;
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(m_nTestSharedFieldID, uidNotExistFieldGroupUID, m_nTestDynamicFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(m_nTestServerID,		pDestServer->GetID());
			CHECK_EQUAL(m_nTestSharedFieldID,	pDestField->GetID());
		}

		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_NotExistDynamicFieldID_DestFieldIsSharedField)
		{
			/// Fixture 설치
			int nNotExistFieldID = 999;
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(m_nTestSharedFieldID, m_pTestDynamicFieldMaster->GetUID(), nNotExistFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(m_nTestServerID,	pDestServer->GetID());
			CHECK_EQUAL(m_nTestSharedFieldID,		pDestField->GetID());
		}

		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_NotExistSharedFieldID_ValidFieldGroup)
		{
			/// Fixture 설치
			int nNotExistFieldID = 999;
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(nNotExistFieldID, m_pTestDynamicFieldMaster->GetUID(), m_nTestDynamicFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			// 검증
			CHECK_EQUAL(true, bRet);
			CHECK_EQUAL(m_nTestServerID,		pDestServer->GetID());
			CHECK_EQUAL(m_nTestDynamicFieldID,	pDestField->GetID());
		}

	}


	namespace Nagative
	{
		TEST_FIXTURE(FLoginFieldGuide, TestLoginFieldGuide_Search_NotExistSharedFieldID)
		{
			/// Fixture 설치
			int nNotExistFieldID = 999;
			ZLoginFieldGuide::PLAYER_FIELD_INFO stPlayerFieldInfo = Make_PLAYER_FIELD_INFO(nNotExistFieldID);
			ZLoginFieldGuide	m_LoginFieldGuide(m_pTestPlayer->GetCID(), stPlayerFieldInfo);

			/// SUT 실행
			ZGameServerObject* pDestServer = NULL;
			ZField* pDestField = NULL;
			bool bRet = m_LoginFieldGuide.Search(pDestServer, pDestField);

			
			// 검증 - 시작필드로 보낸다.
			CHECK_EQUAL(true, bRet);
			ASSERT_CHECK((ZGameServerObject*)NULL	!= pDestServer);
			CHECK_EQUAL(m_nTestServerID,			pDestServer->GetID());

			ASSERT_CHECK((ZField*)NULL				!= pDestField);
			CHECK_EQUAL(FIRST_ENTER_FIELD_ID,		pDestField->GetID());
		}
	}

}
