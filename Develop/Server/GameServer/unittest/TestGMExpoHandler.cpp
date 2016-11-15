#include "stdafx.h"
#include "GGMExpoHandler.h"
#include "MockDBManager.h"
#include "FBasePlayer.h"
#include "MockField.h"
#include "GUTHelper_Field.h"
#include "GServer.h"
#include "GAsyncTask.h"
#include "GEntityPlayer.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"


SUITE(GMExpoHandler)
{
	class TestDBManager : public MockDBManager
	{
	public:
		bool m_bCall_ResetExpoCharacters;
		bool m_bCall_ResetExpoPublicBoothCharacter;

		TestDBManager() : m_bCall_ResetExpoCharacters(false), m_bCall_ResetExpoPublicBoothCharacter(false) {}

		virtual bool ResetExpoCharacters() override
		{
			m_bCall_ResetExpoCharacters= true;
			return true;
		}

		virtual bool ResetExpoPublicBoothCharacter(CID nCID) override
		{
			m_bCall_ResetExpoPublicBoothCharacter= true;
			return true;
		}
	};

	struct Fixture : public FBasePlayer
	{
		Fixture()
		{
		}
		~Fixture()
		{
			gsys.pServer->GetAsyncTaskManager()->Clear();
		}

		GGMExpoHandler	m_gmExpoHandler;

		GTestSysWrapper2<GDBManager, TestDBManager>	m_wrapperDBManager;
	};

	TEST_FIXTURE(Fixture, TestGMExpoHandler_IsExpoAccount)
	{
		wchar_t szName[256];

		for (int i = 1; i < 6; ++i)
		{
			wsprintf(szName, L"e3expo%02d", i);
			CHECK_EQUAL(true, m_gmExpoHandler.IsExpoAccount(szName));
		}
		for (int i = 11; i < 16; ++i)
		{
			wsprintf(szName, L"e3expo%d", i);
			CHECK_EQUAL(true, m_gmExpoHandler.IsExpoAccount(szName));
		}
		for (int i = 21; i < 26; ++i)
		{
			wsprintf(szName, L"e3expo%d", i);
			CHECK_EQUAL(true, m_gmExpoHandler.IsExpoAccount(szName));
		}
		for (int i = 31; i < 36; ++i)
		{
			wsprintf(szName, L"e3expo%d", i);
			CHECK_EQUAL(true, m_gmExpoHandler.IsExpoAccount(szName));
		}
	}

	TEST_FIXTURE(Fixture, TestGMExpoHandler_IsExpoPublicBoothAccount)
	{
		wchar_t szName[256];

		for (int i = 1; i < 9; ++i)
		{
			wsprintf(szName, L"pe3expo0%d", i);
			CHECK_EQUAL(true, m_gmExpoHandler.IsExpoPublicBoothAccount(szName));
		}

		for (int i = 0; i < 2; ++i)
		{
			wsprintf(szName, L"pe3expo1%d", i);
			CHECK_EQUAL(true, m_gmExpoHandler.IsExpoPublicBoothAccount(szName));
		}
	}

	TEST_FIXTURE(Fixture, TestGMExpoHandler_ResetCharacters)
	{
		/// Arrange
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pEntityPlayer = NewEntityPlayer(pField);
		GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(pEntityPlayer->GetUID());

		wsprintf(pPlayer->GetAccountInfo().szUserID, L"e3expo01");

		CHECK_EQUAL(POS_INWORLD, pPlayer->GetState());
		CHECK_EQUAL(0, gsys.pServer->GetAsyncTaskManager()->GetTaskCount());

		CHECK_EQUAL(false, static_cast<TestDBManager*>(gsys.pDBManager)->m_bCall_ResetExpoCharacters);

		/// Act
		m_gmExpoHandler.ResetCharacters();

		/// Assert
		CHECK_EQUAL(POS_READY_MOVE_LOGIN_SERVER, pPlayer->GetState());
		CHECK_EQUAL(1, gsys.pServer->GetAsyncTaskManager()->GetTaskCount());

		CHECK_EQUAL(true, static_cast<TestDBManager*>(gsys.pDBManager)->m_bCall_ResetExpoCharacters);

		pField->Destroy();
	}


	TEST_FIXTURE(Fixture, TestGMExpoHandler_ResetPublicBoothCharacter)
	{
		/// Arrange
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pEntityPlayer = NewEntityPlayer(pField);
		GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(pEntityPlayer->GetUID());

		wsprintf(pPlayer->GetAccountInfo().szUserID, L"pe3expo01");

		CHECK_EQUAL(POS_INWORLD, pPlayer->GetState());
		CHECK_EQUAL(0, gsys.pServer->GetAsyncTaskManager()->GetTaskCount());

		CHECK_EQUAL(false, static_cast<TestDBManager*>(gsys.pDBManager)->m_bCall_ResetExpoPublicBoothCharacter);

		/// Act
		m_gmExpoHandler.ResetPublicBoothCharacter(pPlayer);

		/// Assert
		CHECK_EQUAL(POS_READY_MOVE_LOGIN_SERVER, pPlayer->GetState());
		CHECK_EQUAL(1, gsys.pServer->GetAsyncTaskManager()->GetTaskCount());

		CHECK_EQUAL(true, static_cast<TestDBManager*>(gsys.pDBManager)->m_bCall_ResetExpoPublicBoothCharacter);

		pField->Destroy();
	}

}