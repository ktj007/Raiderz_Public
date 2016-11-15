#include "stdafx.h"
#include "GUTHelper.h"
#include "FBaseAppNetClient.h"
#include "FBaseMockLink.h"
#include "GAppServerChatChannelRouter.h"
#include "GClientChatChannelRouter.h"
#include "CCommandTable_App.h"
#include "GProxyChatChannel.h"
#include "CCommandTable.h"

SUITE(ChatChannelRouter)
{
	struct Fixture : public FBaseAppNetClient, public FBaseMockLink
	{
		GAppServerChatChannelRouter m_serverRouter;
		GClientChatChannelRouter m_clientRouter;
		
		CID m_cidPlayer;
		MUID m_uidChannel;
		wstring m_strChannelName;
		wstring m_strPlayerName;
		MUID m_uidPlayer;
		MockLink* m_pLink;

		MUID m_uidSameChannelMember;
		MockLink* m_pSameChannelLink;
		MUID m_uidOtherChannelMember;
		MockLink* m_pOtherChannelLink;


		Fixture()
		{
			m_cidPlayer = SUnitTestUtil::NewCID();
			m_uidChannel = SUnitTestUtil::NewUID();
			m_strChannelName = L"TestChannelName";
			m_strPlayerName = L"TestPlayerName";

			m_uidPlayer = SUnitTestUtil::NewUID();
			m_pLink = NewLink(m_uidPlayer);

			// 채널 설정
			GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
			pManager->AddChannel(m_uidChannel, m_strChannelName);
			pManager->AddPlayer(m_uidChannel, m_cidPlayer, 1, m_uidPlayer);

			// 채널 라우팅 비교군 설정 - 같은 채널 멤버
			m_uidSameChannelMember = SUnitTestUtil::NewUID();
			m_pSameChannelLink = NewLink(m_uidSameChannelMember);
			pManager->AddPlayer(m_uidChannel, SUnitTestUtil::NewCID(), 1, m_uidSameChannelMember);

			// 채널 라우팅 비교군 설정 - 다른 채널 멤버
			MUID uidOtherChannel = SUnitTestUtil::NewUID();
			m_uidOtherChannelMember = SUnitTestUtil::NewUID();
			m_pOtherChannelLink = NewLink(m_uidOtherChannelMember);	
			pManager->AddChannel(uidOtherChannel, L"TestChannelName2");
			pManager->AddPlayer(uidOtherChannel, SUnitTestUtil::NewCID(), 1, m_uidOtherChannelMember);
		}

		~Fixture()	{}
	};

	TEST_FIXTURE(Fixture, CreateChannelReq)
	{
		// act
		m_serverRouter.CreateChannelReq(m_cidPlayer, m_strChannelName);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_CREATE_PRIVATE_CHANNEL_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK(m_pAppNetAgent->GetSendParam<const wchar_t*>(0, 1)== m_strChannelName);
	}

	TEST_FIXTURE(Fixture, EnterChannelReq)	
	{
		// act
		m_serverRouter.EnterChannelReq(m_cidPlayer, m_strChannelName);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_ENTER_CHANNEL_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK(m_pAppNetAgent->GetSendParam<const wchar_t*>(0, 1) == m_strChannelName);
	}

	TEST_FIXTURE(Fixture, LeaveChannelReq)	
	{
		// act
		m_serverRouter.LeaveChannelReq(m_cidPlayer, m_uidChannel);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_LEAVE_CHANNEL_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<MUID>(0, 1), m_uidChannel);
	}

	TEST_FIXTURE(Fixture, KickPlayerReq)	
	{
		// act
		m_serverRouter.KickPlayerReq(m_cidPlayer, m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_KICK_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<MUID>(0, 1), m_uidChannel);
		CHECK(m_pAppNetAgent->GetSendParam<const wchar_t*>(0, 2) == m_strPlayerName);
	}

	TEST_FIXTURE(Fixture, BanPlayerReq)		
	{
		// act
		m_serverRouter.BanPlayerReq(m_cidPlayer, m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_BAN_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<MUID>(0, 1), m_uidChannel);
		CHECK(m_pAppNetAgent->GetSendParam<const wchar_t*>(0, 2) == m_strPlayerName);
	}

	TEST_FIXTURE(Fixture, UnbanPlayerReq)	
	{
		// act
		m_serverRouter.UnbanPlayerReq(m_cidPlayer, m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_UNBAN_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<MUID>(0, 1), m_uidChannel);
		CHECK(m_pAppNetAgent->GetSendParam<const wchar_t*>(0, 2) == m_strPlayerName);
	}

	TEST_FIXTURE(Fixture, ChangeHostReq)	
	{
		// act
		m_serverRouter.ChangeHostReq(m_cidPlayer, m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pAppNetAgent->GetSendCommandID(0), MPC_MSG_CHANGE_HOST_REQ);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<int>(0, 0), m_cidPlayer);
		CHECK_EQUAL(m_pAppNetAgent->GetSendParam<MUID>(0, 1), m_uidChannel);
		CHECK(m_pAppNetAgent->GetSendParam<const wchar_t*>(0, 2) == m_strPlayerName);
	}

	TEST_FIXTURE(Fixture, EnterChannelRes)
	{
		// arrange
		TD_CHAT_CHANNEL_INFO tdInfo;		
		tdInfo.uidChannel = SUnitTestUtil::NewUID();
		tdInfo.nChannelType = CCT_PRIVATE;
		wcscpy(tdInfo.strChannelName, m_strChannelName.c_str());
		tdInfo.isOwner = true;

		// act
		m_clientRouter.EnterChannelRes(m_uidPlayer, tdInfo);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_ENTER_CHANNEL);

		TD_CHAT_CHANNEL_INFO tdSendInfo;
		MCommand& cmd = m_pLink->GetCommand(0);
		ASSERT_CHECK(cmd.GetParameter(&tdSendInfo, 0, MPT_SINGLE_BLOB));
		CHECK_EQUAL(tdSendInfo.uidChannel, tdInfo.uidChannel);
		CHECK_EQUAL(tdSendInfo.nChannelType, tdInfo.nChannelType);
		CHECK_EQUAL(tdSendInfo.isOwner, tdInfo.isOwner);
		CHECK(0 == wcscmp(tdSendInfo.strChannelName, tdInfo.strChannelName));
	}

	TEST_FIXTURE(Fixture, LeaveChannelRes)	
	{
		// act
		m_clientRouter.LeaveChannelRes(m_uidPlayer, m_uidChannel);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_LEAVE_CHANNEL);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
	}

	TEST_FIXTURE(Fixture, KickPlayerRes)	
	{
		// act
		m_clientRouter.KickPlayerRes(m_uidPlayer, m_uidChannel);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_KICK_PLAYER);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
	}

	TEST_FIXTURE(Fixture, BanPlayerRes)		
	{
		// act
		m_clientRouter.BanPlayerRes(m_uidPlayer, m_uidChannel);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_BAN_PLAYER);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
	}

	TEST_FIXTURE(Fixture, UnbanPlayerRes)
	{
		// act
		m_clientRouter.UnbanPlayerRes(m_uidPlayer, m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_UNBAN_PLAYER);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == m_strPlayerName);
	}

	TEST_FIXTURE(Fixture, ChangeHostRes)	
	{
		// act
		m_clientRouter.ChangeHostRes(m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_CHANGE_HOST);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == m_strPlayerName);

		ASSERT_EQUAL(m_pSameChannelLink->GetCommandID(0), MC_MSG_CHANGE_HOST);
		ASSERT_EQUAL(m_pOtherChannelLink->GetCommandID(0), 0);
	}

	TEST_FIXTURE(Fixture, EnterPlayerInfo)	
	{
		// act
		m_clientRouter.EnterPlayerInfo(m_uidChannel, m_strPlayerName);

		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_MSG_ENTER_CHANNEL_PLAYER_INFO);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == m_strPlayerName);

		ASSERT_EQUAL(m_pSameChannelLink->GetCommandID(0), MC_MSG_ENTER_CHANNEL_PLAYER_INFO);
		ASSERT_EQUAL(m_pOtherChannelLink->GetCommandID(0), 0);
	}

	TEST_FIXTURE(Fixture, LeavePlayerInfo)	
	{
		// act
		m_clientRouter.LeavePlayerInfo(m_uidChannel, m_strPlayerName);

		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_MSG_LEAVE_CHANNEL_PLAYER_INFO);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == m_strPlayerName);

		ASSERT_EQUAL(m_pSameChannelLink->GetCommandID(0), MC_MSG_LEAVE_CHANNEL_PLAYER_INFO);
		ASSERT_EQUAL(m_pOtherChannelLink->GetCommandID(0), 0);
	}

	TEST_FIXTURE(Fixture, KickedPlayerInfo)	
	{
		// act
		m_clientRouter.KickedPlayerInfo(m_uidChannel, m_strPlayerName);

		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_MSG_KICK_PLAYER_INFO);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_uidChannel);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == m_strPlayerName);

		ASSERT_EQUAL(m_pSameChannelLink->GetCommandID(0), MC_MSG_KICK_PLAYER_INFO);
		ASSERT_EQUAL(m_pOtherChannelLink->GetCommandID(0), 0);
	}
}
