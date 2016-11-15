#include "stdafx.h"
#include "PChatModules_PrivateChannel.h"
#include "PGlobal.h"
#include "PCommandCenter.h"
#include "PChatModuleBase.h"
#include "PChatModule_EnterChannel.h"
#include "PChatModule_LeaveChannel.h"
#include "PChatModule_EnterPrivateChannel.h"
#include "PChatModule_LeavePrivateChannel.h"
#include "PChatChannel.h"
#include "PChatPlayer.h"
#include "CCommandTable_App.h"
#include "CCommandResultTable.h"

#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"

#include "SDef.h"
#include "SUnitTestUtil.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "MockServer.h"
#include "PFixtureHelper.h"
#include "MockLink.h"


SUITE(TestChatModules)
{
	struct Fixture : public FBaseChatChannel, FBaseGameServer, FBasePlayer
	{
		Fixture()
		{
			InitGameServer();
			InitPlayers();
			InitChatChannel();
			
			m_pLinkGameServer->ResetCommands();
		}
		~Fixture()
		{
		}

		void InitGameServer()
		{
			m_nGameServerID = SUnitTestUtil::NewID();
			m_uidGameServer = AddGameServerObject(m_nGameServerID);

			m_pLinkGameServer = PTestHelper::NewLink(m_uidGameServer);
		}
		void InitPlayers()
		{
			/// Channel Owner
			m_cidOwner = SUnitTestUtil::NewID();
			m_strOwnerName = L"OwnerName";
			AddPlayerObject(m_nGameServerID, m_cidOwner, m_strOwnerName);

			/// Test Player
			m_cidTestPlayer = SUnitTestUtil::NewID();
			m_strTestPlayerName = L"TestPlayer";
			AddPlayerObject(m_nGameServerID, m_cidTestPlayer, m_strTestPlayerName);
		}
		void InitChatChannel()
		{
			m_strChannelName = SUnitTestUtil::NewName();
			MUID uidChannel = AddChatChannel_Private(m_cidOwner, m_strChannelName, m_nGameServerID);

			m_pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);
		}
		void TestPlayerEnterChannel(PChatChannel* pChannel)
		{
			auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, pChannel->GetName()));
			auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterChannel(pChannel));
			chatModule.get()->Run(cmd.get());

			m_pLinkGameServer->ResetCommands();
		}
		void TestPlayerBan(PChatChannel* pChannel)
		{
			auto_ptr<MCommand> cmd(MakeCmd_BAN_REQ(m_uidGameServer, m_cidOwner, pChannel->GetUID(), m_strTestPlayerName));
			auto_ptr<PChatModuleBase> chatModule(new PChatModule_Ban(pChannel));
			chatModule.get()->Run(cmd.get());

			m_pLinkGameServer->ResetCommands();
		}

		minet::MCommand* MakeCmd_ENTER_CHANNEL_REQ(MUID uidGameServer, CID nPlayerCID, wstring& strChannelName)
		{
			MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_ENTER_CHANNEL_REQ
				, 2
				, NEW_INT(nPlayerCID)
				, NEW_WSTR(strChannelName.c_str())
				);
			VALID_RET(pCmd->CheckRule(), NULL);

			pCmd->SetSenderUID(uidGameServer);

			return pCmd;
		}
		minet::MCommand* MakeCmd_LEAVE_CHANNEL_REQ(MUID uidGameServer, CID nPlayerCID, MUID uidChannel)
		{
			MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_LEAVE_CHANNEL_REQ
				, 2
				, NEW_INT(nPlayerCID)
				, NEW_UID(uidChannel)
				);
			VALID_RET(pCmd->CheckRule(), NULL);

			pCmd->SetSenderUID(uidGameServer);

			return pCmd;
		}
		minet::MCommand* MakeCmd_KICK_REQ(MUID uidGameServer, CID nPlayerCID, MUID uidChannel, wstring& strTargetPlayerName)
		{
			MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_KICK_REQ
				, 3
				, NEW_INT(nPlayerCID)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayerName.c_str())
				);
			VALID_RET(pCmd->CheckRule(), NULL);

			pCmd->SetSenderUID(uidGameServer);

			return pCmd;
		}
		minet::MCommand* MakeCmd_BAN_REQ(MUID uidGameServer, CID nPlayerCID, MUID uidChannel, wstring& strTargetPlayerName)
		{
			MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_BAN_REQ
				, 3
				, NEW_INT(nPlayerCID)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayerName.c_str())
				);
			VALID_RET(pCmd->CheckRule(), NULL);

			pCmd->SetSenderUID(uidGameServer);

			return pCmd;
		}
		minet::MCommand* MakeCmd_UNBAN_REQ(MUID uidGameServer, CID nPlayerCID, MUID uidChannel, wstring& strTargetPlayerName)
		{
			MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_UNBAN_REQ
				, 3
				, NEW_INT(nPlayerCID)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayerName.c_str())
				);
			VALID_RET(pCmd->CheckRule(), NULL);

			pCmd->SetSenderUID(uidGameServer);

			return pCmd;
		}
		minet::MCommand* MakeCmd_CHANGE_HOST_REQ(MUID uidGameServer, CID nPlayerCID, MUID uidChannel, wstring& strTargetPlayerName)
		{
			MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MSG_CHANGE_HOST_REQ
				, 3
				, NEW_INT(nPlayerCID)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayerName.c_str())
				);
			VALID_RET(pCmd->CheckRule(), NULL);

			pCmd->SetSenderUID(uidGameServer);

			return pCmd;
		}

		int m_nGameServerID;
		MUID m_uidGameServer;
		MockLink* m_pLinkGameServer;

		wstring m_strChannelName;
		PChatChannel* m_pChannel;

		CID m_cidOwner;
		wstring m_strOwnerName;

		CID m_cidTestPlayer;
		wstring m_strTestPlayerName;
	};

	TEST_FIXTURE(Fixture, TestChatModule_EnterChannel)
	{
		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());

		auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, m_strChannelName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(2, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL,		m_pLinkGameServer->GetCommand(1).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_EnterChannel_Failure_NotExistChannel)
	{
		wstring strNotExistChannelName = L"NotExistChannel";
		auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, strNotExistChannelName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
	}
	TEST_FIXTURE(Fixture, TestChatModule_EnterChannel_Failure_AlreadyEntered)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, m_strChannelName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,							m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_ALREADY_ENTERED_CHANNEL,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_EnterChannel_Failure_LimitEnteredChannelCount)
	{
		TestPlayerEnterChannel(m_pChannel);

		wstring strChannelName1 = SUnitTestUtil::NewName();
		MUID uidChannel1 = AddChatChannel_Private(m_cidOwner, strChannelName1, m_nGameServerID);
		PChatChannel* pChannel1 = gmgr.pChatChannelManager->FindChannel(uidChannel1);
		TestPlayerEnterChannel(pChannel1);

		wstring strChannelName2 = SUnitTestUtil::NewName();
		MUID uidChannel2 = AddChatChannel_Private(m_cidOwner, strChannelName2, m_nGameServerID);
		PChatChannel* pChannel2 = gmgr.pChatChannelManager->FindChannel(uidChannel2);
		TestPlayerEnterChannel(pChannel2);

		PChatPlayer* pTestChatPlayer = gmgr.pChatChannelManager->FindPlayer(m_cidTestPlayer);
		CHECK_EQUAL(3, pTestChatPlayer->GetEnteredPrivateChannelCount());
		/// Test Player 가 3개의 채널에 입장함. ///


		/// New Channel Owner
		CID cidNewOwner = SUnitTestUtil::NewID();
		wstring strNewOwnerName = L"NewOwnerName";
		AddPlayerObject(m_nGameServerID, cidNewOwner, strNewOwnerName);

		/// New Owner 가 가진 새로운 채널을 생성.
		wstring strChannelName3 = SUnitTestUtil::NewName();
		MUID uidChannel3 = AddChatChannel_Private(cidNewOwner, strChannelName3, m_nGameServerID);
		PChatChannel* pChannel3 = gmgr.pChatChannelManager->FindChannel(uidChannel3);

		CHECK_EQUAL(4, gmgr.pChatChannelManager->GetChannelCount());
		m_pLinkGameServer->ResetCommands();

		/// Test Player 를 4번째 채널에 입장 시도
		auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, strChannelName3));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterChannel(pChannel3));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(3, pTestChatPlayer->GetEnteredPrivateChannelCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,								m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_LIMIT_ENTER_PRIVATE_CHANNEL,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_LeaveChannel)
	{
		TestPlayerEnterChannel(m_pChannel);

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());

		auto_ptr<MCommand> cmd(MakeCmd_LEAVE_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, m_pChannel->GetUID()));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_LeaveChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(2, m_pLinkGameServer->GetCommandCount());
		
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL,			m_pLinkGameServer->GetCommand(1).GetID());
	}

	TEST_FIXTURE(Fixture, TestChatModule_EnterPrivateChannel_Success)
	{
		auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, m_strChannelName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterPrivateChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL,				m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL_PLAYER_INFO,	m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_EnterPrivateChannel_Failure_Baned)
	{
		TestPlayerEnterChannel(m_pChannel);
		TestPlayerBan(m_pChannel);

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(true, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		auto_ptr<MCommand> cmd(MakeCmd_ENTER_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, m_strChannelName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_EnterPrivateChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_BANED_CHANNEL,		m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_LeavePrivateChannel)
	{
		TestPlayerEnterChannel(m_pChannel);

		auto_ptr<MCommand> cmd(MakeCmd_LEAVE_CHANNEL_REQ(m_uidGameServer, m_cidTestPlayer, m_pChannel->GetUID()));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_LeavePrivateChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());

		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,		m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL,				m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO,	m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_LeavePrivateChannel_LeaveOwner_ChangeOwner)
	{
		TestPlayerEnterChannel(m_pChannel);

		/// 테스트 채널에 ChangeHost 모듈 추가.
		m_pChannel->GetChatModules().AddModule(CMT_CHANGE_HOST, new PChatModule_ChangeHost(m_pChannel));

		/// 테스트 코드 시작
		CHECK_EQUAL(m_cidOwner, m_pChannel->GetOwnerCID());

		auto_ptr<MCommand> cmd(MakeCmd_LEAVE_CHANNEL_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID()));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_LeavePrivateChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(m_cidTestPlayer, m_pChannel->GetOwnerCID());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(4, m_pLinkGameServer->GetCommandCount());

		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,		m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL,				m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO,	m_pLinkGameServer->GetCommand(2).GetID());
		CHECK_EQUAL(MPC_MSG_CHANGE_HOST_INFO,			m_pLinkGameServer->GetCommand(3).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_LeaveChannel_Private_LeaveOwner_LazilyDeleteChannel)
	{
		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(false, m_pChannel->IsReserveDelete());

		auto_ptr<MCommand> cmd(MakeCmd_LEAVE_CHANNEL_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID()));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_LeavePrivateChannel(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(0, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(true, m_pChannel->IsReserveDelete());

		CHECK_EQUAL(2, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,		m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL,				m_pLinkGameServer->GetCommand(1).GetID());
	}

	TEST_FIXTURE(Fixture, TestChatModule_Kick_Success)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		auto_ptr<MCommand> cmd(MakeCmd_KICK_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strTestPlayerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Kick(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_KICK,					m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_KICK_PLAYER_INFO,		m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_Kick_Failure_RequestNotOwner)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		auto_ptr<MCommand> cmd(MakeCmd_KICK_REQ(m_uidGameServer, m_cidTestPlayer, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Kick(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Kick_Failure_TargetIsOwner)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		auto_ptr<MCommand> cmd(MakeCmd_KICK_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Kick(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Kick_Failure_TargetNameIsLong)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		wstring strLongTargetName = L"IHaveAVeryLongName12345678901234567890";	// 이름은 영문 10자 제한 이지만, 내부적으로는 32자까지 허용된다.

		auto_ptr<MCommand> cmd(MakeCmd_KICK_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), strLongTargetName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Kick(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Ban_Success)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(false, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		auto_ptr<MCommand> cmd(MakeCmd_BAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strTestPlayerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Ban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(true, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_BAN,					m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_BAN_PLAYER_INFO,		m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_Ban_Failure_TargetNameIsLong)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		wstring strLongTargetName = L"IHaveAVeryLongName12345678901234567890";

		auto_ptr<MCommand> cmd(MakeCmd_BAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), strLongTargetName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Ban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Ban_Failure_RequestNotOwner)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		auto_ptr<MCommand> cmd(MakeCmd_BAN_REQ(m_uidGameServer, m_cidTestPlayer, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Ban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(false, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,			m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Ban_Failure_NotExistTargetPlayer)
	{
		TestPlayerEnterChannel(m_pChannel);
		TestPlayerBan(m_pChannel);
		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(true, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		auto_ptr<MCommand> cmd(MakeCmd_BAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strTestPlayerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Ban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_EXIST_PLAYER,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Ban_Failure_TargetIsOwner)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		auto_ptr<MCommand> cmd(MakeCmd_BAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Ban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Unban_Success)
	{
		TestPlayerEnterChannel(m_pChannel);
		TestPlayerBan(m_pChannel);

		CHECK_EQUAL(1, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(true, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		auto_ptr<MCommand> cmd(MakeCmd_UNBAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strTestPlayerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Unban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(false, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		CHECK_EQUAL(1,					m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_UNBAN_RES,	m_pLinkGameServer->GetCommand(0).GetID());
	}
	TEST_FIXTURE(Fixture, TestChatModule_Unban_Failure_TargetNameIsLong)
	{
		TestPlayerEnterChannel(m_pChannel);
		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());

		wstring strLongTargetName = L"IHaveAVeryLongName12345678901234567890";

		auto_ptr<MCommand> cmd(MakeCmd_UNBAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), strLongTargetName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Unban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Unban_Failure_RequestNotOwner)
	{
		TestPlayerEnterChannel(m_pChannel);
		TestPlayerBan(m_pChannel);

		auto_ptr<MCommand> cmd(MakeCmd_UNBAN_REQ(m_uidGameServer, m_cidTestPlayer, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Unban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,			m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_Unban_Failure_TargetIsNotBanedPlayer)
	{
		TestPlayerEnterChannel(m_pChannel);

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(false, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		auto_ptr<MCommand> cmd(MakeCmd_UNBAN_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strTestPlayerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_Unban(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(2, m_pChannel->GetPlayerCount());
		CHECK_EQUAL(false, m_pChannel->IsBanedPlayer(m_strTestPlayerName));

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_BANED,			m_pLinkGameServer->GetParam<int>(0, 4));
	}

	TEST_FIXTURE(Fixture, TestChatModule_ChangeHost_Success)
	{
		TestPlayerEnterChannel(m_pChannel);

		CHECK_EQUAL(m_cidOwner, m_pChannel->GetOwnerCID());

		auto_ptr<MCommand> cmd(MakeCmd_CHANGE_HOST_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strTestPlayerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_ChangeHost(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(m_cidTestPlayer, m_pChannel->GetOwnerCID());

		CHECK_EQUAL(1,										m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_CHANGE_HOST_INFO,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_pChannel->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(0, wcscmp(m_strTestPlayerName.c_str(),	m_pLinkGameServer->GetParam<const wchar_t*>(0, 1)));
	}
	TEST_FIXTURE(Fixture, TestChatModule_ChangeHost_Failure_TargetNameIsLong)
	{
		TestPlayerEnterChannel(m_pChannel);

		wstring strLongTargetName = L"IHaveAVeryLongName12345678901234567890";

		auto_ptr<MCommand> cmd(MakeCmd_CHANGE_HOST_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), strLongTargetName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_ChangeHost(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_ChangeHost_Failure_RequestNotOwner)
	{
		TestPlayerEnterChannel(m_pChannel);

		CHECK_EQUAL(m_cidOwner, m_pChannel->GetOwnerCID());

		auto_ptr<MCommand> cmd(MakeCmd_CHANGE_HOST_REQ(m_uidGameServer, m_cidTestPlayer, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_ChangeHost(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(m_cidOwner, m_pChannel->GetOwnerCID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,			m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_ChangeHost_Failure_WrongTarget)
	{
		wstring strWrongTarget = L"WrongTarget";
		auto_ptr<MCommand> cmd(MakeCmd_CHANGE_HOST_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), strWrongTarget));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_ChangeHost(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(m_cidOwner, m_pChannel->GetOwnerCID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_NOT_EXIST_PLAYER,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestChatModule_ChangeHost_Failure_TargetIsOwnerSelf)
	{
		auto_ptr<MCommand> cmd(MakeCmd_CHANGE_HOST_REQ(m_uidGameServer, m_cidOwner, m_pChannel->GetUID(), m_strOwnerName));
		auto_ptr<PChatModuleBase> chatModule(new PChatModule_ChangeHost(m_pChannel));
		chatModule.get()->Run(cmd.get());

		CHECK_EQUAL(m_cidOwner, m_pChannel->GetOwnerCID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,				m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_MSG_WRONG_TARGET,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
}
