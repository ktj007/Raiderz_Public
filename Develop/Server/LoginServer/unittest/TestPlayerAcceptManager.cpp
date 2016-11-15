#include "stdafx.h"
#include "LTestWrapper.h"
#include "LUnitTestUtil.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LPlayerAcceptManager.h"
#include "SDef.h"
#include "LSystem.h"



SUITE(PlayerAcceptManager)
{
	struct Fixture
	{
		Fixture()
		{

		}
		~Fixture()
		{
		}

		LTestMgrWrapper<LPlayerAcceptManager> m_wrprPlayerAcceptManager;
	};

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_Add_Success)
	{
		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);

		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());

		gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_Add_Failure_Duplicate)
	{
		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);

		bool bRet = gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(true, bRet);

		bRet = gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_Add_Failure_ConnectionKeyIsInvalid)
	{
		MUID uidInvalidConnectionKey = MUID::ZERO;
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidInvalidConnectionKey, nAID, strName, uidPlayer);

		bool bRet = gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(false, bRet);

		SAFE_DELETE(pAccepter);
	}

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_Del_Success)
	{
		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);

		gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());

		/// Act
		gmgr.pPlayerAcceptManager->Del(uidConnectionKey);

		/// Assert
		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_FindAcceptor_Success)
	{
		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);

		gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());

		/// Act
		LPlayerAcceptor_FromGameServer* pFoundAccepter = gmgr.pPlayerAcceptManager->FindAcceptor(uidConnectionKey);

		/// Assert
		CHECK_EQUAL((LPlayerAcceptor_FromGameServer*)pFoundAccepter, pAccepter);
	}

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_FindAcceptor_Failure_InvalidConnectionKey_ShouldReturnNULL)
	{
		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);

		gmgr.pPlayerAcceptManager->Add(pAccepter);

		/// Act
		LPlayerAcceptor_FromGameServer* pNULLAccepter = gmgr.pPlayerAcceptManager->FindAcceptor(MUID::ZERO);

		/// Assert
		CHECK_EQUAL((LPlayerAcceptor_FromGameServer*)NULL, pNULLAccepter);
	}

	TEST_FIXTURE(Fixture, TestPlayerAcceptManager_Timeout)
	{
		LTestSysWrapper2<LSystem, LUnitTestUtil::FakeSystem> m_wrprSystem;

		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();

		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);
		gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());

		gsys.pSystem->Sleep(TIMEOUT_TICK_MOVE_SERVER);
		gmgr.pPlayerAcceptManager->Update();

		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
	}

}