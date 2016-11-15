#include "StdAfx.h"
#include "SUnitTestUtil.h"
#include "LTestHelper.h"
#include "MockPlayerObject.h"
#include "LCommandTable.h"
#include "MockLink.h"
#include "LMasterServer.h"
#include "LMasterServerMgr.h"
#include "LGlobal.h"
#include "LDBManager.h"
#include "MockDBManager.h"
#include "FBaseMasterClient.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "LTestWrapper.h"
#include "LPlayerObjectManager.h"
#include "LDBTaskDataAccount.h"


SUITE(PlayerLogin)
{

const int TEST_nAID = LTestHelper::NewID();

	class LoginMockDBManager: public MockDBManager 
	{
	public:
		virtual bool LoginGetInfo(LDBT_ACC_LOGIN& data) override
		{
			if (data.m_strUSER_ID == L"TEST_ID" && data.m_strPWD == L"TEST_PASS")
			{
				MCommand* pNewCommand = gsys.pServer->GetMasterClient()->MakeNewSendCommand(MLC_PLAYER_ADD_PLAYER_REQ
				, 3
				, NEW_UID(data.m_uidPlayer)
				, NEW_INT(TEST_nAID)
				, NEW_WSTR(data.m_strUSER_ID.c_str()));

				gsys.pServer->GetMasterClient()->GetNetClient()->Post(pNewCommand);
			}
			else
			{
				MCommand* pCmdLoginFail = gsys.pCommandCenter->MakeNewCommand(MC_COMM_RESPONSE_LOGIN
				, data.m_uidPlayer
				, 3
				, NEW_INT(CR_FAIL_LOGIN_NOT_EXIST_USER)
				, NEW_UID(MUID::ZERO)
				, NEW_UCHAR(0));

				gsys.pCommandCenter->PostCommand(pCmdLoginFail);
			}			
		
			return true;
		}
	};

	struct Fixture : public FBaseMasterClient
	{
		Fixture()
		{
			pPlayerObject = LTestHelper::NewMockPlayerObject();
			pLinkTestPlayer = LTestHelper::NewLink(pPlayerObject->GetUID());
			pOriginalDBManager = gsys.Change(new LoginMockDBManager);
		}

		~Fixture()
		{
			delete gsys.Change(pOriginalDBManager);
			LTestHelper::ClearLinks();
		}

		void Login()
		{
			ACCOUNT_INFO info;
			info.nAID = 1;
			_tcscpy_s(info.szUserID, ACCOUNT_NAME_LEN + 1, _T("TEST_ID"));

			pPlayerObject->InitAccountInfo(info);
		}

		MCommandResult Recv_MC_COMM_REQUEST_LOGIN(std::wstring strID, std::wstring strPass)
		{
			return pLinkTestPlayer->OnRecv(MC_COMM_REQUEST_LOGIN
				, 3
				, NEW_WSTR(strID.c_str())
				, NEW_WSTR(strPass.c_str())
				, NEW_INT(SH_COMMAND_VERSION));
		}

		MCommandResult Recv_MLC_PLAYER_ADD_PLAYER_RES(MUID uidPlayer, int nResult)
		{
			return m_pNetAgent->OnRecv(MLC_PLAYER_ADD_PLAYER_RES
				, 2
				, NEW_UID(uidPlayer)
				, NEW_INT(nResult));
		}

		MockLink* pLinkTestPlayer;
		MockPlayerObject* pPlayerObject;
		LDBManager* pOriginalDBManager;

		MockLink* pLinkMasterServer;

		LTestMgrWrapper<LPlayerObjectManager> m_wrprPlayerObjectManager;
	};

	void CreateMasterServer()
	{
		{
			LMasterServerInfo Info;
			Info.strName = L"first";
			Info.nAddr[0] = 123;
			Info.nAddr[1] = 231;
			Info.nAddr[2] = 122;
			Info.nAddr[3] = 113;
			Info.nPort = 7777;
			gmgr.pMasterServerMgr->Add(Info);
		}
		{
			LMasterServerInfo Info;
			Info.strName = L"second";
			Info.nAddr[0] = 222;
			Info.nAddr[1] = 232;
			Info.nAddr[2] = 210;
			Info.nAddr[3] = 236;
			Info.nPort = 8145;
			gmgr.pMasterServerMgr->Add(Info);
		}		
	}


	TEST_FIXTURE(Fixture, RequestLogin)
	{
		gmgr.pPlayerObjectManager->DeleteAllObject();

		Recv_MC_COMM_REQUEST_LOGIN(L"TEST_ID", L"TEST_PASS");
		CHECK_EQUAL(0, pLinkTestPlayer->GetCommandCount());

		CHECK_EQUAL(1,							m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MLC_PLAYER_ADD_PLAYER_REQ,	m_pNetAgent->GetSendCommand(0).GetID());
		CHECK_EQUAL(pLinkTestPlayer->GetUID(),	m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(TEST_nAID,					m_pNetAgent->GetSendParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, RequestLogin_Failure)
	{
		gmgr.pPlayerObjectManager->DeleteAllObject();

		Recv_MC_COMM_REQUEST_LOGIN(L"TEST_ID", L"INVALID_PASS");
		CHECK_EQUAL(1,						pLinkTestPlayer->GetCommandCount());
		CHECK_EQUAL(MC_COMM_RESPONSE_LOGIN,	pLinkTestPlayer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_LOGIN_NOT_EXIST_USER, pLinkTestPlayer->GetParam<int>(0, 0));
		CHECK_EQUAL(MUID::ZERO,				pLinkTestPlayer->GetParam<MUID>(0, 1));
		CHECK_EQUAL(0,						pLinkTestPlayer->GetParam<uint8>(0, 2));

		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
	}

	TEST_FIXTURE(Fixture, ResponseAddPlayer)
	{
		Recv_MLC_PLAYER_ADD_PLAYER_RES(pPlayerObject->GetUID(), CR_SUCCESS);

		CHECK_EQUAL(1,						pLinkTestPlayer->GetCommandCount());
		CHECK_EQUAL(MC_COMM_RESPONSE_LOGIN,	pLinkTestPlayer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_SUCCESS,				pLinkTestPlayer->GetParam<int>(0, 0));
		CHECK_EQUAL(pPlayerObject->GetUID(),pLinkTestPlayer->GetParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, CharRequestSelMyChar)
	{
		/// Fixture 설치
		Login();

		SAccountCharFieldInfo charFieldInfo;
		charFieldInfo.nCID = SUnitTestUtil::NewID();
		charFieldInfo.nExitedSharedFieldID = SUnitTestUtil::NewID();
		pPlayerObject->AddCharFieldInfo(charFieldInfo);

		CHECK_EQUAL(pPlayerObject->GetState(), POS_SELECT_CHAR);

		/// SUT 실행
		pLinkTestPlayer->OnRecv(MC_CHAR_REQUEST_SEL_MYCHAR, 2, NEW_INT(0));
		
		TD_LOGIN_GAME_SERVER_INFO ResponseLoginGameServerInfo;
		wstring strTestHostName = SUnitTestUtil::NewString();
		wcsncpy_s(ResponseLoginGameServerInfo.strHostName, strTestHostName.c_str(), _TRUNCATE);
		ResponseLoginGameServerInfo.nPort = 0;
		ResponseLoginGameServerInfo.uidConnectionKey = MUID(1);
		m_pNetAgent->OnRecv(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER, 3, NEW_UID(pPlayerObject->GetUID()), NEW_INT(CR_SUCCESS), NEW_BLOB(&ResponseLoginGameServerInfo, sizeof(ResponseLoginGameServerInfo), 1));


		/// 검증
		ASSERT_EQUAL(MC_CHAR_RESPONSE_SEL_MYCHAR, pLinkTestPlayer->GetCommandID(0));
		ASSERT_EQUAL(CR_SUCCESS, pLinkTestPlayer->GetParam<int>(0, 0));

		ASSERT_EQUAL(MC_COMM_MOVE_TO_GAME_SERVER, pLinkTestPlayer->GetCommandID(1));

		TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo;
		int nCount;
		ASSERT_EQUAL(true, pLinkTestPlayer->GetCommand(1).GetBlob(pLoginGameServerInfo, nCount, 0));
		CHECK(!wcsncmp(pLoginGameServerInfo->strHostName, strTestHostName.c_str(), MAX_HOSTNAME_LEN));
		CHECK_EQUAL(0, pLoginGameServerInfo->nPort);
		CHECK_EQUAL(MUID(1), pLoginGameServerInfo->uidConnectionKey);
	}

}
