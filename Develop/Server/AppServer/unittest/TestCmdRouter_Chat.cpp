#include "stdafx.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "PFixtureHelper.h"
#include "SUnitTestUtil.h"
#include "MockServer.h"
#include "MockLink.h"
#include "PCmdRouter_Chat.h"
#include "CCommandTable_App.h"
#include "PChatChannel.h"
#include "PChatChannelManager.h"
#include "PChatChannelFactory.h"
#include "STransData.h"
#include "SProxyPlayerManager.h"
#include "STransData_A2G.h"
#include "CTransData.h"


SUITE(CmdRouter_Chat)
{
	struct Fixture : public FBaseGameServer, FBaseChatChannel
	{
		Fixture()
		{
			InitGameServer();
			InitProxyPlayer();
		}
		~Fixture()
		{
		}

		void InitGameServer()
		{
			m_nGameServerID = SUnitTestUtil::NewID();
			m_uidGameServer = AddGameServerObject(m_nGameServerID);
			m_pLinkGameServer = PTestHelper::NewLink(m_uidGameServer);

			m_nGameServerID2 = SUnitTestUtil::NewID();
			m_uidGameServer2 = AddGameServerObject(m_nGameServerID2);
			m_pLinkGameServer2 = PTestHelper::NewLink(m_uidGameServer2);
		}
		void InitProxyPlayer()
		{
			m_strTestPlayerName = L"TestPlayer";
			m_nTestPlayerCID = AddTestProxyPlayer(m_nGameServerID, m_strTestPlayerName);

			m_strTestPlayerName2 = L"TestPlayer2";
			m_nTestPlayerCID2 = AddTestProxyPlayer(m_nGameServerID2, m_strTestPlayerName2);
		}
		int AddTestProxyPlayer(int nGameServerID, wstring strTestPlayer)
		{
			int nTestPlayerCID = SUnitTestUtil::NewID();

			TD_PROXY_PLAYER_INFO tdPlayerInfo;
			tdPlayerInfo.UID = SUnitTestUtil::NewUID();
			tdPlayerInfo.nCID = nTestPlayerCID;
			tdPlayerInfo.nGameServerID = nGameServerID;
			wcsncpy_s(tdPlayerInfo.szName, strTestPlayer.c_str(), _TRUNCATE);

			gsys.pServer->GetPlayerManager()->AddPlayer(tdPlayerInfo);
			return nTestPlayerCID;
		}

		PCmdRouter_Chat m_CmdRouter_Chat;

		int m_nGameServerID;
		MUID m_uidGameServer;
		MockLink* m_pLinkGameServer;

		int m_nGameServerID2;
		MUID m_uidGameServer2;
		MockLink* m_pLinkGameServer2;

		CID m_nTestPlayerCID;
		wstring m_strTestPlayerName;
		CID m_nTestPlayerCID2;
		wstring m_strTestPlayerName2;

		PTestMgrWrapper<PChatChannelManager>	m_wrapperChatChannelManager;
	};
	
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteAllChannelInfo)
	{
		wstring strChannelName1 = L"TestChannel1";
		wstring strChannelName2 = L"TestChannel2";
		MUID uidChannel1 = AddChatChannel_Private(m_nTestPlayerCID, strChannelName1.c_str(), m_nGameServerID);
		MUID uidChannel2 = AddChatChannel_Private(m_nTestPlayerCID2, strChannelName2.c_str(), m_nGameServerID2);
		m_pLinkGameServer2->ResetCommands();

		m_CmdRouter_Chat.RouteAllChannelInfo(m_uidGameServer2);

		CHECK_EQUAL(1,							m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_CHANNEL_INFO,	m_pLinkGameServer2->GetCommand(0).GetID());
		TDAG_CHAT_CHANNEL_INFO* pChannelInfo;
		int nBlobCount;
		m_pLinkGameServer2->GetCommand(0).GetBlob(pChannelInfo, nBlobCount, 0);
		CHECK_EQUAL(nBlobCount, gmgr.pChatChannelManager->GetChannelCount());

		CHECK_EQUAL(pChannelInfo[0].uidChannel,		uidChannel1);
		CHECK(0 == wcscmp(pChannelInfo[0].strChannelName, strChannelName1.c_str()));		

		CHECK_EQUAL(pChannelInfo[1].uidChannel, uidChannel2);
		CHECK(0 == wcscmp(pChannelInfo[1].strChannelName, strChannelName2.c_str()));
	}

	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteAddedChannelInfo)
	{
		int nOwnerCID = SUnitTestUtil::NewID();
		std::wstring strChannelName = L"ChannelName";

		PChatChannelFactory factoryChatChannel;
		PChatChannel* pChatChannel = factoryChatChannel.NewChatChannel_Private(nOwnerCID, strChannelName);

		m_CmdRouter_Chat.RouteAddedChannelInfo(pChatChannel);


		TDAG_CHAT_CHANNEL_INFO* pChannelInfo = NULL;

		CHECK_EQUAL(1,							m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_CHANNEL_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		pChannelInfo = (TDAG_CHAT_CHANNEL_INFO*)m_pLinkGameServer->GetCommand(0).GetParameter(0)->GetPointer();
		CHECK_EQUAL(pChatChannel->GetUID(),		pChannelInfo->uidChannel);
		CHECK(0 == wcscmp(strChannelName.c_str(),		pChannelInfo->strChannelName));

		CHECK_EQUAL(1,							m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_CHANNEL_INFO,	m_pLinkGameServer2->GetCommand(0).GetID());
		pChannelInfo = (TDAG_CHAT_CHANNEL_INFO*)m_pLinkGameServer2->GetCommand(0).GetParameter(0)->GetPointer();
		CHECK(0 == wcscmp(strChannelName.c_str(),	pChannelInfo->strChannelName));

		SAFE_DELETE(pChatChannel);
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteDeletedChannelInfo)
	{
		MUID uidDummyChannel = SUnitTestUtil::NewUID();
		m_CmdRouter_Chat.RouteDeletedChannelInfo(uidDummyChannel);

		CHECK_EQUAL(1,								m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_CHANNEL_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(uidDummyChannel,				m_pLinkGameServer->GetParam<MUID>(0, 0));

		CHECK_EQUAL(1,								m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_CHANNEL_INFO,	m_pLinkGameServer2->GetCommand(0).GetID());
		CHECK_EQUAL(uidDummyChannel,				m_pLinkGameServer2->GetParam<MUID>(0, 0));
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteAllChatPlayerInfo)
	{
		wstring strChannelName1 = L"TestChannel1";
		wstring strChannelName2 = L"TestChannel2";
		MUID uidChannel1 = AddChatChannel_Private(m_nTestPlayerCID, strChannelName1.c_str(), m_nGameServerID);
		MUID uidChannel2 = AddChatChannel_Private(m_nTestPlayerCID2, strChannelName2.c_str(), m_nGameServerID2);
		m_pLinkGameServer2->ResetCommands();

		m_CmdRouter_Chat.RouteAllChatPlayerInfo(m_uidGameServer2);

		CHECK_EQUAL(1,							m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,	m_pLinkGameServer2->GetCommand(0).GetID());

		TDAG_CHAT_CHANNEL_PLAYER_INFO* pChannelPlayerInfo = NULL;
		int nBlobCount = 0;
		m_pLinkGameServer2->GetCommand(0).GetBlob(pChannelPlayerInfo, nBlobCount, 0);
		CHECK_EQUAL(nBlobCount, gmgr.pChatChannelManager->GetChatPlayerMap().size());

		CHECK_EQUAL(pChannelPlayerInfo[0].uidChannel,	uidChannel1);
		CHECK_EQUAL(pChannelPlayerInfo[0].nCID,			m_nTestPlayerCID);
		CHECK_EQUAL(pChannelPlayerInfo[0].nGameServer,	m_nGameServerID);

		CHECK_EQUAL(pChannelPlayerInfo[1].uidChannel,	uidChannel2);
		CHECK_EQUAL(pChannelPlayerInfo[1].nCID,			m_nTestPlayerCID2);
		CHECK_EQUAL(pChannelPlayerInfo[1].nGameServer,	m_nGameServerID2);
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteAddedChatPlayerInfo)
	{
		MUID uidDummyChannel = SUnitTestUtil::NewUID();
		m_CmdRouter_Chat.RouteAddedChatPlayerInfo(uidDummyChannel, m_nTestPlayerCID);


		MCommandParameterSingleBlob* pChannelPlayerInfoSingleBlob = NULL;	
		TDAG_CHAT_CHANNEL_PLAYER_INFO* pChannelPlayerInfo = NULL;

		CHECK_EQUAL(1,							m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		pChannelPlayerInfoSingleBlob = (MCommandParameterSingleBlob*)m_pLinkGameServer->GetCommand(0).GetParameter(0);
		pChannelPlayerInfo = (TDAG_CHAT_CHANNEL_PLAYER_INFO*)pChannelPlayerInfoSingleBlob->GetPointer();
		CHECK_EQUAL(uidDummyChannel,			pChannelPlayerInfo->uidChannel);
		CHECK_EQUAL(m_nTestPlayerCID,			pChannelPlayerInfo->nCID);
		CHECK_EQUAL(m_nGameServerID,			pChannelPlayerInfo->nGameServer);

		CHECK_EQUAL(1,							m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,	m_pLinkGameServer2->GetCommand(0).GetID());
		pChannelPlayerInfoSingleBlob = (MCommandParameterSingleBlob*)m_pLinkGameServer2->GetCommand(0).GetParameter(0);
		pChannelPlayerInfo = (TDAG_CHAT_CHANNEL_PLAYER_INFO*)pChannelPlayerInfoSingleBlob->GetPointer();
		CHECK_EQUAL(uidDummyChannel,			pChannelPlayerInfo->uidChannel);
		CHECK_EQUAL(m_nTestPlayerCID,			pChannelPlayerInfo->nCID);
		CHECK_EQUAL(m_nGameServerID,			pChannelPlayerInfo->nGameServer);
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteDeletedChatPlayerInfo)
	{
		MUID uidDummyChannel = SUnitTestUtil::NewUID();
		m_CmdRouter_Chat.RouteDeletedChatPlayerInfo(uidDummyChannel, m_nTestPlayerCID);

		CHECK_EQUAL(1,								m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(uidDummyChannel,				m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_nTestPlayerCID,				m_pLinkGameServer->GetParam<int>(0, 1));

		CHECK_EQUAL(1,								m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer2->GetCommand(0).GetID());
		CHECK_EQUAL(uidDummyChannel,				m_pLinkGameServer2->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_nTestPlayerCID,				m_pLinkGameServer2->GetParam<int>(0, 1));
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteChatPlayerGameServerInfo)
	{
		CID nTestCID = SUnitTestUtil::NewCID();
		int nGameServerID = SUnitTestUtil::NewID();
		m_CmdRouter_Chat.RouteChatPlayerGameServerInfo(nTestCID, nGameServerID);
	}

	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_PostEnterChannel)
	{
		MUID uidChannel = AddChatChannel_Private(m_nTestPlayerCID, L"TestChannel", m_nGameServerID);
		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);
		bool isOwner = true;
		m_pLinkGameServer->ResetCommands();

		m_CmdRouter_Chat.PostEnterChannel(m_uidGameServer, m_nTestPlayerCID, pChannel);

		CHECK_EQUAL(1,							m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL,		m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_nTestPlayerCID,			m_pLinkGameServer->GetParam<int>(0, 0));

		MCommandParameterSingleBlob* pChannelInfoSingleBlob = (MCommandParameterSingleBlob*)m_pLinkGameServer->GetCommand(0).GetParameter(1);
		TD_CHAT_CHANNEL_INFO* pChannelInfo = (TD_CHAT_CHANNEL_INFO*)pChannelInfoSingleBlob->GetPointer();
		CHECK_EQUAL(CCT_PRIVATE,				pChannelInfo->nChannelType);
		CHECK_EQUAL(uidChannel,					pChannelInfo->uidChannel);		
		CHECK(0 == wcscmp(L"TestChannel", pChannelInfo->strChannelName));
		CHECK_EQUAL(isOwner,					pChannelInfo->isOwner);
	}

	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_PostLeaveChannel)
	{
		MUID uidChannel = AddChatChannel_Private(m_nTestPlayerCID, L"TestChannel", m_nGameServerID);
		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);
		m_pLinkGameServer->ResetCommands();

		m_CmdRouter_Chat.PostLeaveChannel(m_uidGameServer, m_nTestPlayerCID, pChannel);

		CHECK_EQUAL(1,						m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_nTestPlayerCID,		m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(uidChannel,				m_pLinkGameServer->GetParam<MUID>(0, 1));
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_PostKick)
	{
		MUID uidTestChannel = SUnitTestUtil::NewUID();
		m_CmdRouter_Chat.PostKick(m_uidGameServer, m_nTestPlayerCID, uidTestChannel);

		CHECK_EQUAL(1,					m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_KICK,		m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_nTestPlayerCID,	m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(uidTestChannel,		m_pLinkGameServer->GetParam<MUID>(0, 1));
	}
	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteKickPlayerInfo)
	{
		MUID uidChannel = AddChatChannel_Private(m_nTestPlayerCID, L"TestChannel", m_nGameServerID);
		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);

		gmgr.pChatChannelManager->EnterChannel(pChannel, m_nTestPlayerCID2, m_nGameServerID2);

		m_pLinkGameServer->ResetCommands();
		m_pLinkGameServer2->ResetCommands();

		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		m_CmdRouter_Chat.RouteKickPlayerInfo(pChannel, m_strTestPlayerName);

		CHECK_EQUAL(1,										m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_KICK_PLAYER_INFO,				m_pLinkGameServer2->GetCommand(0).GetID());
		CHECK_EQUAL(uidChannel,								m_pLinkGameServer2->GetParam<MUID>(0, 0));
		CHECK_EQUAL(0, wcscmp(m_strTestPlayerName.c_str(),	m_pLinkGameServer2->GetParam<const wchar_t*>(0, 1)));
	}

	TEST_FIXTURE(Fixture, TestCmdRouter_Chat_RouteKickPlayerInfo_MultiPlayers)
	{
		USES_CONVERSION_EX;

		wstring strTestPlayer1_1 = L"TestPlayer1-1";
		int nTestPlayerCID1_1 = AddTestProxyPlayer(m_nGameServerID, strTestPlayer1_1);
		int nTestPlayerCID1_2 = AddTestProxyPlayer(m_nGameServerID, L"TestPlayer1-2");
		int nTestPlayerCID2_1 = AddTestProxyPlayer(m_nGameServerID2, L"TestPlayer2-1");
		int nTestPlayerCID2_2 = AddTestProxyPlayer(m_nGameServerID2, L"TestPlayer2-2");

		MUID uidChannel = AddChatChannel_Private(m_nTestPlayerCID, L"TestChannel", m_nGameServerID);
		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);

		gmgr.pChatChannelManager->EnterChannel(pChannel, nTestPlayerCID1_1, m_nGameServerID);
		gmgr.pChatChannelManager->EnterChannel(pChannel, nTestPlayerCID1_2, m_nGameServerID);
		gmgr.pChatChannelManager->EnterChannel(pChannel, nTestPlayerCID2_1, m_nGameServerID2);
		gmgr.pChatChannelManager->EnterChannel(pChannel, nTestPlayerCID2_2, m_nGameServerID2);

		m_pLinkGameServer->ResetCommands();
		m_pLinkGameServer2->ResetCommands();

		CHECK_EQUAL(5, pChannel->GetPlayerCount());	///< ÃÑ 5¸í

		/// 1¸í ÅðÀå½ÃÅ´ - ¹æÀå ¸»°í
		m_CmdRouter_Chat.RouteKickPlayerInfo(pChannel, strTestPlayer1_1);

		CHECK_EQUAL(1,										m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_KICK_PLAYER_INFO,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(uidChannel,								m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(0, wcscmp(strTestPlayer1_1.c_str(),		m_pLinkGameServer->GetParam<const wchar_t*>(0, 1)));

		CHECK_EQUAL(1,										m_pLinkGameServer2->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_KICK_PLAYER_INFO,				m_pLinkGameServer2->GetCommand(0).GetID());
		CHECK_EQUAL(uidChannel,								m_pLinkGameServer2->GetParam<MUID>(0, 0));
		CHECK_EQUAL(0, wcscmp(strTestPlayer1_1.c_str(),		m_pLinkGameServer2->GetParam<const wchar_t*>(0, 1)));
	}

}