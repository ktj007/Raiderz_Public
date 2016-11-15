#include "stdafx.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "PFixtureHelper.h"
#include "MockServer.h"
#include "MockLink.h"
#include "PChattingLogic.h"
#include "CCommandTable_App.h"
#include "PChatChannel.h"
#include "PChatChannelManager.h"
#include "PCommandCenter.h"
#include "CCommandResultTable.h"
#include "SUnitTestUtil.h"

SUITE(ChattingLogic)
{
	struct Fixture : public FBasePlayer, FBaseChatChannel, FBaseGameServer
	{
		Fixture()
		{
			InitTestGameServer();
			InitTestPlayer();
		}
		~Fixture()
		{
		}

		void InitTestGameServer()
		{
			m_nGameServerID = SUnitTestUtil::NewID();
			m_uidGameServer = AddGameServerObject(m_nGameServerID);
			m_pLinkGameServer = PTestHelper::NewLink(m_uidGameServer);
		}
		void InitTestPlayer()
		{
			m_nPlayerCID = AddPlayerObject(m_nGameServerID);
		}

		PChattingLogic m_ChattingLogic;

		int m_nGameServerID;
		MUID m_uidGameServer;
		MockLink* m_pLinkGameServer;

		int m_nPlayerCID;
	};

	TEST_FIXTURE(Fixture, TestChattingLogic_CreateChannel_Private)
	{
		CHECK_EQUAL(0, gmgr.pChatChannelManager->GetChannelCount());

		std::wstring strChannelName = L"TestChannel";
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName);

		CHECK_EQUAL(1, gmgr.pChatChannelManager->GetChannelCount());

		CHECK_EQUAL(3,							m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_CHANNEL_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(1).GetID());

		PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(strChannelName);
		CHECK_EQUAL(1, pChannel->GetPlayerCount());
	}
	TEST_FIXTURE(Fixture, TestChattingLogic_CreatePrivateChannel_Failure_ChannelNameIsLong)
	{
		std::wstring strChannelName = L"TestLongLongChannelName_123456789012345678901234567890123456789012345678901234567890";
		CHECK(strChannelName.size() > MAX_CHATTING_CHANNEL_NAME_LEN);
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName);

		CHECK_EQUAL(0, gmgr.pChatChannelManager->GetChannelCount());

		CHECK_EQUAL(1,										m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,							m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_OVER_CHATTING_CHANNEL_NAME,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChattingLogic_CreatePrivateChannel_Failure_LimitEnteredChannelCount)
	{
		std::wstring strChannelName1 = L"TestChannel1";
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName1);

		std::wstring strChannelName2 = L"TestChannel2";
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName2);

		std::wstring strChannelName3 = L"TestChannel3";
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName3);
		CHECK_EQUAL(3, gmgr.pChatChannelManager->GetChannelCount());
		m_pLinkGameServer->ResetCommands();

		std::wstring strChannelName4 = L"TestChannel4";
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName4);
		CHECK_EQUAL(3, gmgr.pChatChannelManager->GetChannelCount());

		CHECK_EQUAL(3,											m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,								m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_LIMIT_ENTER_PRIVATE_CHANNEL,	m_pLinkGameServer->GetParam<int>(1, 4));
	}
	TEST_FIXTURE(Fixture, TestChattingLogic_CreateChannel_Private_Duplication)
	{
		std::wstring strChannelName = L"TestChannel";
		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName);

		CHECK_EQUAL(1, gmgr.pChatChannelManager->GetChannelCount());
		m_pLinkGameServer->ResetCommands();

		m_ChattingLogic.CreateChannel_Private(m_nPlayerCID, strChannelName);
		CHECK_EQUAL(1, gmgr.pChatChannelManager->GetChannelCount());

		CHECK_EQUAL(1,										m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,							m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_ALREADY_EXIST_CHANNEL_NAME,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
}