#include "stdafx.h"
#include "ZTestHelper.h"
#include "MockLink.h"
#include "ZCommandTable.h"
#include "ZGameServerAcceptor.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerInfoManager.h"
#include "MockFieldInfoManager.h"
#include "ZTestWrapper.h"
#include "ZFixtureHelper.h"
#include "ZConfig.h"

SUITE(GameServerLogin)
{
	struct FGameServerLogin : public FBaseMockLink2
	{
		FGameServerLogin() 
		{
			SetupFieldInfo();
			SetupServerInfo();

			ZConfig::m_nMyWorldID = WORLD_ID;
			ZConfig::m_nMyServerID = SERVER_ID;
		}
		~FGameServerLogin()
		{
			
		}
		void SetupFieldInfo()
		{
			AddFieldInfo(107, L"FieldName1", L"", false, 3);
			AddFieldInfo(108, L"FieldName2", L"", false, 3);
		}
		void SetupServerInfo()
		{
			// 필드 할당 --
			// 107 -1,2,3 
			// 108 -3
			//

			AddGameServerInfo(SERVER_ID, 107, 3, 1, 2, 3);

			AddGameServerInfo(SERVER_ID, 108, 1, 3);

		}
		bool CheckResponseCommand(MCommand* pCommand, MUID uidClient=MUID::ZERO)
		{
			if (pCommand->GetID() != MMC_COMM_RESPONSE_LOGIN_M2G) return false;

			int nResult;
			MUID uidAlloc;
			uint8 nServerMode;

			if (pCommand->GetParameter(&nResult,	0, MPT_INT)==false) return false;
			if (pCommand->GetParameter(&uidAlloc,	1, MPT_UID)==false) return false;
			if (pCommand->GetParameter(&nServerMode,2, MPT_UCHAR)==false) return false;

			MCommandParameter* pParam = pCommand->GetParameter(3);
			if(pParam->GetType()!=MPT_BLOB) return false;
				
			TDMG_MANAGED_SHARED_FIELD* pManagedSharedFields = (TDMG_MANAGED_SHARED_FIELD*)pParam->GetPointer();

			if (pParam->GetBlobCount() != 2) return false;		// 107, 108

			if (pManagedSharedFields[0].nFieldID != 107) return false;
			if (pManagedSharedFields[0].nChannelCount != 3) return false;
			if (pManagedSharedFields[0].nChannelID[0] != 1) return false;
			if (pManagedSharedFields[0].nChannelID[1] != 2) return false;
			if (pManagedSharedFields[0].nChannelID[2] != 3) return false;

			if (pManagedSharedFields[1].nFieldID != 108) return false;
			if (pManagedSharedFields[1].nChannelCount != 1) return false;
			if (pManagedSharedFields[1].nChannelID[0] != 3) return false;

			set<MUID> setUID;
			if (pManagedSharedFields[0].uidField[0] != MUID::ZERO) setUID.insert(pManagedSharedFields[0].uidField[0]);
			if (pManagedSharedFields[0].uidField[1] != MUID::ZERO) setUID.insert(pManagedSharedFields[0].uidField[1]);
			if (pManagedSharedFields[0].uidField[2] != MUID::ZERO) setUID.insert(pManagedSharedFields[0].uidField[2]);
			if (pManagedSharedFields[1].uidField[0] != MUID::ZERO) setUID.insert(pManagedSharedFields[1].uidField[0]);

			if (setUID.size() != 4) return false;

			return true;
		}

		static const int WORLD_ID = 100;
		static const int SERVER_ID = 1;
	};

	TEST_FIXTURE(FGameServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_G2M)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"GameServer1";
		const int nServerType = SERVER_GAME;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nWorldID = WORLD_ID;
		const int nServerID = SERVER_ID;


		MCommandResult nCommandResult = pLink->OnRecv(MMC_COMM_REQUEST_LOGIN_G2M, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(CR_TRUE, nCommandResult);


		CHECK_EQUAL(3,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN, MMC_FIELD_INFO_ALL, MMC_PARTY_INFO_ALL_RES

		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_M2G, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_SUCCESS, pLink->GetParam<int>(0, 0));

		// CheckResponseCommand 함수에서 커맨드 모든 파라메타를 검사한다.
		CHECK(CheckResponseCommand(&pLink->GetCommand(0)) == true);
	}

	TEST_FIXTURE(FGameServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_Failure_NotEqualCommandVersion)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"GameServer1";
		const int nServerType = SERVER_GAME;
		const int nParamCommandVersion = SH_COMMAND_VERSION + 1;
		const int nWorldID = WORLD_ID;
		const int nServerID = SERVER_ID;


		MCommandResult nCommandResult = pLink->OnRecv(MMC_COMM_REQUEST_LOGIN_G2M, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(CR_TRUE, nCommandResult);


		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN_M2G

		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_M2G, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_COMMAND_INVALID_VERSION, pLink->GetParam<int>(0, 0));

		// CheckResponseCommand 함수에서 커맨드 모든 파라메타를 검사한다.
		CHECK(CheckResponseCommand(&pLink->GetCommand(0)) == false);
	}

	TEST_FIXTURE(FGameServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_Failure_InvalidWorldID)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"GameServer1";
		const int nServerType = SERVER_GAME;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nInvalidWorldID = 234234;
		const int nServerID = SERVER_ID;


		MCommandResult nCommandResult = pLink->OnRecv(MMC_COMM_REQUEST_LOGIN_G2M, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nInvalidWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN_M2G

		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_M2G, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_INVALID_WORLD_ID, pLink->GetParam<int>(0, 0));

		// CheckResponseCommand 함수에서 커맨드 모든 파라메타를 검사한다.
		CHECK(CheckResponseCommand(&pLink->GetCommand(0)) == false);
	}

	TEST_FIXTURE(FGameServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_Failure_InvalidServerID)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"GameServer1";
		const int nServerType = SERVER_GAME;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nWorldID = WORLD_ID;
		const int nInvalidServerID = 0;


		MCommandResult nCommandResult = pLink->OnRecv(MMC_COMM_REQUEST_LOGIN_G2M, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nInvalidServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN_M2G

		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_M2G, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_ACCEPT, pLink->GetParam<int>(0, 0));

		// CheckResponseCommand 함수에서 커맨드 모든 파라메타를 검사한다.
		CHECK(CheckResponseCommand(&pLink->GetCommand(0)) == false);
	}

	TEST_FIXTURE(FGameServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_G2M_Failure_Duplicated)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"GameServer1";
		const int nServerType = SERVER_GAME;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nWorldID = WORLD_ID;
		const int nServerID = SERVER_ID;

		pLink->OnRecv(MMC_COMM_REQUEST_LOGIN_G2M, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		pLink->ResetCommands();

		pLink->OnRecv(MMC_COMM_REQUEST_LOGIN_G2M, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));


		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN_M2G

		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_M2G, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_ALREADY_EXIST, pLink->GetParam<int>(0, 0));

		// CheckResponseCommand 함수에서 커맨드 모든 파라메타를 검사한다.
		CHECK(CheckResponseCommand(&pLink->GetCommand(0)) == false);
	}

	TEST_FIXTURE(FGameServerLogin, TestServerObjectAcceptor)
	{
		MUID uidClient = ZTestHelper::NewUID();
		int nServerID = SERVER_ID;

		ZGameServerAcceptor acceptor(uidClient, nServerID);

		bool bRet = acceptor.Accept();

		CHECK_EQUAL(bRet, true);
		CHECK_EQUAL(1, gmgr.pGameServerObjectManager->GetClientsCount());
		
		std::auto_ptr<MCommand> pNewCommand(acceptor.MakeResponseCommand());
		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_M2G, pNewCommand->GetID());
		
		CHECK(CheckResponseCommand(pNewCommand.get(), uidClient) == true);
	}
}
