#include "stdafx.h"
#include "ZPlayer.h"
#include "ZCommandTable.h"
#include "ZFixtureHelper.h"
#include "ZPlayerLogic.h"
#include "ZTestHelper_CmdHandler.h"
#include "ZPlayerStateChanger.h"
#include "ZPMSSystem.h"


SUITE(AddPlayerPmang)
{
	struct FAddPlayerPmang : public FBasePlayer, FBaseMockLink, FBaseLoginServer
	{
		FAddPlayerPmang()
		{
			InitLoginServer();
			InitInstanceVariable();
		}
		~FAddPlayerPmang()
		{
		}

		void InitLoginServer()
		{
			m_uidLoginServer = Connected(TEST_LOGINSERVER_ID);
			m_pLinkLoginServer = NewLink(m_uidLoginServer);
		}
		void InitInstanceVariable()
		{
			m_nAID = ZTestHelper::NewAID();
			m_strUserID = ZTestHelper::NewName();
			m_nCID = ZTestHelper::NewCID();
			m_uidPlayer = ZTestHelper::NewUID();
			m_strPlayerName = L"aibeast";
			m_nGID = ZTestHelper::NewID();
		}
		void AddPlayer(MUID uidPlayer, AID nAID)
		{
			m_playerLogic.AddPlayer(uidPlayer, nAID, m_strUserID);
		}

		static const int TEST_LOGINSERVER_ID = 10;

		ZPlayerLogic m_playerLogic;
		ZPlayerStateChanger m_stateChanger;

		MUID m_uidLoginServer;
		MockLink* m_pLinkLoginServer;

		///	테스트용 인스턴스 변수들
		int m_nAID;
		wstring m_strUserID;
		int m_nCID;
		MUID m_uidPlayer;
		wstring m_strPlayerName;
		int m_nGID;

		ZTestSysWrapper<ZPMSSystem > m_testPMSSystem;
	};
	
	TEST_FIXTURE(FAddPlayerPmang, TestPlayer_OnCommand_MLC_PLAYER_PMANG_ADD_PLAYER_REQ_Success)
	{
		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
		
		TD_PMANG_USER_DATA tdUserData;
		wcscpy_s(tdUserData.szUserSerial, L"serial");
		wcscpy_s(tdUserData.szUserID, L"aibeast");
		tdUserData.nUserType = ZTestHelper::NewID();
		tdUserData.nRegionCode = PRC_GYEONGGI;
		tdUserData.nAgeCode = PAC_25TO29;
		tdUserData.nGenderCode = PGC_MALE;
		tdUserData.nYearOfBirth = 1986;
		
		ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID, &tdUserData);

		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());

		CHECK_EQUAL(1,								m_pLinkLoginServer->GetCommandCount());
		CHECK_EQUAL(MLC_PLAYER_PMANG_ADD_PLAYER_RES,m_pLinkLoginServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_uidPlayer,					m_pLinkLoginServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(CR_SUCCESS,						m_pLinkLoginServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(FAddPlayerPmang, TestPlayer_OnCommand_MLC_PLAYER_PMANG_ADD_PLAYER_REQ_Success_PmangUserDataEndBoundary)
	{
		TD_PMANG_USER_DATA tdUserData;
		wcscpy_s(tdUserData.szUserSerial, L"serial");
		wcscpy_s(tdUserData.szUserID, L"aibeast");
		tdUserData.nUserType = ZTestHelper::NewID();
		tdUserData.nRegionCode = PRC_FOREIGN;
		tdUserData.nAgeCode = PAC_40TO99;
		tdUserData.nGenderCode = PGC_FEMALE;
		tdUserData.nYearOfBirth = 1900;

		ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID, &tdUserData);

		CHECK_EQUAL(1, gmgr.pPlayerManager->GetObjectCount());

		CHECK_EQUAL(1,								m_pLinkLoginServer->GetCommandCount());
		CHECK_EQUAL(MLC_PLAYER_PMANG_ADD_PLAYER_RES,m_pLinkLoginServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_uidPlayer,					m_pLinkLoginServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(CR_SUCCESS,						m_pLinkLoginServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(FAddPlayerPmang, TestPlayer_OnCommand_MLC_PLAYER_PMANG_ADD_PLAYER_REQ_Failure_InvalidPmangUserData_InvalidRegionCode)
	{
		TD_PMANG_USER_DATA tdUserData;
		wcscpy_s(tdUserData.szUserSerial, L"serial");
		wcscpy_s(tdUserData.szUserID, L"aibeast");
		tdUserData.nUserType = ZTestHelper::NewID();
		tdUserData.nRegionCode = MAX_PRC;
		tdUserData.nAgeCode = PAC_25TO29;
		tdUserData.nGenderCode = PGC_MALE;
		tdUserData.nYearOfBirth = 1986;

		ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID, &tdUserData);

		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FAddPlayerPmang, TestPlayer_OnCommand_MLC_PLAYER_PMANG_ADD_PLAYER_REQ_Failure_InvalidPmangUserData_InvalidAgeCode)
	{
		TD_PMANG_USER_DATA tdUserData;
		wcscpy_s(tdUserData.szUserSerial, L"serial");
		wcscpy_s(tdUserData.szUserID, L"aibeast");
		tdUserData.nUserType = ZTestHelper::NewID();
		tdUserData.nRegionCode = PRC_GYEONGGI;
		tdUserData.nAgeCode = MAX_PAC;
		tdUserData.nGenderCode = PGC_MALE;
		tdUserData.nYearOfBirth = 1986;

		ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID, &tdUserData);

		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FAddPlayerPmang, TestPlayer_OnCommand_MLC_PLAYER_PMANG_ADD_PLAYER_REQ_Failure_InvalidPmangUserData_InvalidGenderCode)
	{
		TD_PMANG_USER_DATA tdUserData;
		wcscpy_s(tdUserData.szUserSerial, L"serial");
		wcscpy_s(tdUserData.szUserID, L"aibeast");
		tdUserData.nUserType = ZTestHelper::NewID();
		tdUserData.nRegionCode = PRC_GYEONGGI;
		tdUserData.nAgeCode = PAC_25TO29;
		tdUserData.nGenderCode = MAX_PGC;
		tdUserData.nYearOfBirth = 1986;

		ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID, &tdUserData);

		CHECK_EQUAL(0, gmgr.pPlayerManager->GetObjectCount());
	}

	TEST_FIXTURE(FAddPlayerPmang, TestPlayer_OnCommand_MLC_PLAYER_PMANG_ADD_PLAYER_REQ_Failure_OverLimit)
	{
		for (int i = 0; i < WORLD_PLAYER_LIMIT; ++i)
		{
			ZPlayer* pPlayer = gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewAID(), ZTestHelper::NewName());
			gmgr.pPlayerManager->Add(pPlayer);
		}
		CHECK_EQUAL(WORLD_PLAYER_LIMIT, gmgr.pPlayerManager->GetObjectCount());


		TD_PMANG_USER_DATA tdUserData;
		wcscpy_s(tdUserData.szUserSerial, L"serial");
		wcscpy_s(tdUserData.szUserID, L"aibeast");
		tdUserData.nUserType = ZTestHelper::NewID();
		tdUserData.nRegionCode = PRC_GYEONGGI;
		tdUserData.nAgeCode = PAC_25TO29;
		tdUserData.nGenderCode = PGC_MALE;
		tdUserData.nYearOfBirth = 1986;

		ZTestHelper_CmdHandler::OnRecv_MLC_PLAYER_PMANG_ADD_PLAYER_REQ(m_pLinkLoginServer, m_uidPlayer, m_nAID, m_strUserID, &tdUserData);

		CHECK_EQUAL(1,								m_pLinkLoginServer->GetCommandCount());
		CHECK_EQUAL(MLC_PLAYER_PMANG_ADD_PLAYER_RES,m_pLinkLoginServer->GetCommandID(0));
		CHECK_EQUAL(m_uidPlayer,					m_pLinkLoginServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(CR_FAIL_LOGIN_WORLD_IS_FULL,	m_pLinkLoginServer->GetParam<int>(0, 1));
	}


}