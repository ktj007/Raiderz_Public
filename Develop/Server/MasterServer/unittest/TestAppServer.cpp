#include "stdafx.h"
#include "CCommandTable_Master.h"
#include "ZAppServerInfoManager.h"
#include "ZAppServerInfoLoader.h"
#include "ZFixtureHelper.h"
#include "ZConfig.h"
#include "STransData_M2G.h"

SUITE(AppServer)
{	
	struct Fixture : public FBaseAppServer, FBaseMockLink
	{
		static const int WORLD_ID = 200;
		static const int SERVER_ID = 20;
		MockLink* m_pLink;

		Fixture()
		{			
			ZConfig::m_nMyWorldID = WORLD_ID;
			ZConfig::m_nMyServerID = SERVER_ID;			
			m_pLink = NewLink();
		}

		MCommandResult RecvLogin(wstring strServerName, int nServerType, int nWorldID, int nServerID, int nCommandVersion, vector<TD_APP_SERVER_INFO>& vecAppServerInfo)
		{
			return m_pLink->OnRecv(MAC_COMM_REQUEST_LOGIN, 6,
									NEW_WSTR(strServerName.c_str()),
									NEW_INT(nServerType),
									NEW_INT(nWorldID),
									NEW_INT(nServerID),
									NEW_INT(nCommandVersion),
									NEW_BLOB(vecAppServerInfo));
		}
	};
	
	TEST_FIXTURE(Fixture, LoadAppServerInfo)
	{
		string strXmlSample1 = "<Maiet>						\
							   <AppServer id='20'>			\
							   </AppServer>				\
							   <AppServer id='21'>			\
							   </AppServer>				\
							   <AppServer id='22'>			\
							   </AppServer>				\
							   </Maiet>";

		ZAppServerInfoManager loginServerInfoManager;
		ZAppServerInfoLoader loader;

		bool bSuccess = loader.LoadStream(&loginServerInfoManager, strXmlSample1.c_str());
		CHECK_EQUAL(true, bSuccess);

		CHECK_EQUAL(3, loginServerInfoManager.GetSize());

		// 1번 서버
		ZAppServerInfo* pAppServerInfo = loginServerInfoManager.GetInfo(20);
		ASSERT_CHECK(pAppServerInfo != NULL);
		CHECK_EQUAL(20, pAppServerInfo->m_nID);

		// 2번 서버
		ZAppServerInfo* pAppServerInfo2 = loginServerInfoManager.GetInfo(21);
		ASSERT_CHECK(pAppServerInfo2 != NULL);
		CHECK_EQUAL(21, pAppServerInfo2->m_nID);

		// 3번 서버
		ZAppServerInfo* pAppServerInfo3 = loginServerInfoManager.GetInfo(22);
		ASSERT_CHECK(pAppServerInfo3 != NULL);
		CHECK_EQUAL(22, pAppServerInfo3->m_nID);		
	}

	TEST_FIXTURE(Fixture, LoginAppServer)
	{
		// arrange
		AddAppServerInfo(20);

		wstring strServerName = L"AppServer";
		int nServerType = SERVER_APPLICATION;
		int nWorldID = WORLD_ID;
		int nServerID = SERVER_ID;
		int nCommandVersion = SH_COMMAND_VERSION;
		TD_APP_SERVER_INFO info;
		info.nIP = 100;
		info.nPort = 1000;
		info.nAppServerMode = APP_SERVER_CHAT;
		vector<TD_APP_SERVER_INFO> vecAppServerInfo(1, info);
		
		// act
		MCommandResult nResult = RecvLogin(strServerName, nServerType, nWorldID, nServerID, nCommandVersion, vecAppServerInfo);

		// assert
		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MAC_COMM_RESPONSE_LOGIN, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_SUCCESS, m_pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, LoginAppServer_Filure_DuplicatedLogin)
	{
		// arrange
		AddAppServerInfo(20);

		wstring strServerName = L"AppServer";
		int nServerType = SERVER_APPLICATION;
		int nWorldID = WORLD_ID;
		int nServerID = SERVER_ID;
		int nCommandVersion = SH_COMMAND_VERSION;
		TD_APP_SERVER_INFO info;
		info.nIP = 100;
		info.nPort = 1000;
		info.nAppServerMode = APP_SERVER_CHAT;
		vector<TD_APP_SERVER_INFO> vecAppServerInfo(1, info);

		RecvLogin(strServerName, nServerType, nWorldID, nServerID, nCommandVersion, vecAppServerInfo);
		m_pLink->ResetCommands();

		// act
		RecvLogin(strServerName, nServerType, nWorldID, nServerID, nCommandVersion, vecAppServerInfo);

		// assert
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MAC_COMM_RESPONSE_LOGIN, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(CRM_FAIL_LOGIN_ALREADY_EXIST, m_pLink->GetParam<int>(0, 0));
	}
}
