#include "stdafx.h"
#include "ZTestHelper.h"
#include "MockLink.h"

#include "ZCommandTable.h"
#include "ZLoginServerAcceptor.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerInfoManager.h"
#include "MockFieldInfoManager.h"
#include "ZTestWrapper.h"
#include "ZFixtureHelper.h"
#include "ZConfig.h"

SUITE(LoginServerLogin)
{
	struct FLoginServerLogin : public FBaseMockLink, FBaseLoginServer
	{
		FLoginServerLogin() 
		{
			ZConfig::m_nMyWorldID = WORLD_ID;
			ZConfig::m_nMyServerID = SERVER_ID;

			SetupServerInfo();
		}
		~FLoginServerLogin()
		{

		}
		void SetupServerInfo()
		{
			AddLoginServerInfo(SERVER_ID);
		}
		bool CheckResponseCommand(MCommand* pCommand, MUID uidClient=MUID::ZERO)
		{
			if (pCommand->GetID() != MLC_COMM_RESPONSE_LOGIN) return false;

			int nResult;
			MUID uidAlloc;

			if (pCommand->GetParameter(&nResult,	0, MPT_INT)==false) return false;
			if (pCommand->GetParameter(&uidAlloc,	1, MPT_UID)==false) return false;

			return true;
		}

		static const int WORLD_ID = 200;
		static const int SERVER_ID = 10;

	};

	TEST_FIXTURE(FLoginServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_L2M)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"LoginServer1";
		const int nServerType = SERVER_LOGIN;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nWorldID = WORLD_ID;
		const int nServerID = SERVER_ID;


		MCommandResult nCommandResult = pLink->OnRecv(MLC_COMM_REQUEST_LOGIN, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(CR_TRUE, nCommandResult);


		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN_M2G

		CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_SUCCESS, pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(FLoginServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_L2M_Failure_NotEqualCommandVersion)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"LoginServer1";
		const int nServerType = SERVER_LOGIN;
		const int nParamCommandVersion = SH_COMMAND_VERSION + 1;
		const int nWorldID = WORLD_ID;
		const int nServerID = SERVER_ID;


		MCommandResult nCommandResult = pLink->OnRecv(MLC_COMM_REQUEST_LOGIN, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(CR_TRUE, nCommandResult);


		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MLC_COMM_RESPONSE_LOGIN

		CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_COMMAND_INVALID_VERSION, pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(FLoginServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_L2M_Failure_InvalidWorldID)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"LoginServer1";
		const int nServerType = SERVER_LOGIN;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nInvalidWorldID = 238947239;
		const int nServerID = SERVER_ID;


		MCommandResult nCommandResult = pLink->OnRecv(MLC_COMM_REQUEST_LOGIN, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nInvalidWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MLC_COMM_RESPONSE_LOGIN

		CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_INVALID_WORLD_ID, pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(FLoginServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_L2M_Failure_InvalidServerID)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"LoginServer1";
		const int nServerType = SERVER_LOGIN;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nWorldID = WORLD_ID;
		const int nInvalidServerID = 0;


		MCommandResult nCommandResult = pLink->OnRecv(MLC_COMM_REQUEST_LOGIN, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nInvalidServerID),
			NEW_INT(nParamCommandVersion));

		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MLC_COMM_RESPONSE_LOGIN

		CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_ACCEPT, pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(FLoginServerLogin, TestCmdHandler_MMC_COMM_REQUEST_LOGIN_L2M_Failure_Duplicated)
	{
		MockLink* pLink = NewLink();
		const wstring strServerName = L"LoginServer1";
		const int nServerType = SERVER_LOGIN;
		const int nParamCommandVersion = SH_COMMAND_VERSION;
		const int nWorldID = WORLD_ID;
		const int nServerID = SERVER_ID;


		pLink->OnRecv(MLC_COMM_REQUEST_LOGIN, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));

		pLink->ResetCommands();

		pLink->OnRecv(MLC_COMM_REQUEST_LOGIN, 5,
			NEW_WSTR(strServerName.c_str()),
			NEW_INT(nServerType),
			NEW_INT(nWorldID),
			NEW_INT(nServerID),
			NEW_INT(nParamCommandVersion));


		CHECK_EQUAL(1,	pLink->GetCommandCount());	// MMC_COMM_RESPONSE_LOGIN_M2G

		CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_ALREADY_EXIST, pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(FLoginServerLogin, TestServerAcceptor)
	{
		MUID uidClient = ZTestHelper::NewUID();
		int nServerID = SERVER_ID;

		ZLoginServerAcceptor acceptor(uidClient, nServerID);

		bool bRet = acceptor.Accept();

		CHECK_EQUAL(bRet, true);
		CHECK_EQUAL(true, gmgr.pLoginServerFacade->IsConnected());

		std::auto_ptr<MCommand> pNewCommand(acceptor.MakeResponseCommand());
		CHECK_EQUAL(MLC_COMM_RESPONSE_LOGIN, pNewCommand->GetID());
	}
}