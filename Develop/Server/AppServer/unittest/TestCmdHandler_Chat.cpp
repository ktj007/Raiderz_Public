#include "stdafx.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "PFixtureHelper.h"
#include "SUnitTestUtil.h"
#include "MockServer.h"
#include "MockLink.h"
#include "CCommandTable_App.h"
#include "PChatChannel.h"
#include "PChatModules_PrivateChannel.h"
#include "CCommandResultTable.h"


SUITE(CmdHandler_Chat)
{
	struct Fixture : public FBaseGameServer, FBaseChatChannel, FBasePlayer
	{
		Fixture()
		{
			InitGameServer();
			InitPlayers();

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
		PChatChannel* CreateTestChannel(CID cidPlayer)
		{
			wstring strTestChannel = L"TestChannel";
			OnRecv_CREATE_CHANNEL_PRIVATE_REQ(cidPlayer, strTestChannel);
			m_pLinkGameServer->ResetCommands();
			return gmgr.pChatChannelManager->FindChannel(strTestChannel);
		}
		PChatChannel* InitTestChannelEnteredTestPlayer()
		{
			PChatChannel* pChannel = CreateTestChannel(m_cidOwner);
			OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());
			m_pLinkGameServer->ResetCommands();
			return pChannel;
		}

		MCommandResult OnRecv_CREATE_CHANNEL_PRIVATE_REQ(CID cidPlayer, wstring& strChannelName)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_CREATE_PRIVATE_CHANNEL_REQ
				, 2
				, NEW_INT(cidPlayer)
				, NEW_WSTR(strChannelName.c_str())
				);
		}
		MCommandResult OnRecv_ENTER_CHANNEL_REQ(CID cidPlayer, wstring& strChannelName)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_ENTER_CHANNEL_REQ
				, 2
				, NEW_INT(cidPlayer)
				, NEW_WSTR(strChannelName.c_str())
				);
		}
		MCommandResult OnRecv_LEAVE_CHANNEL_REQ(CID cidPlayer, MUID uidChannel)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_LEAVE_CHANNEL_REQ
				, 2
				, NEW_INT(cidPlayer)
				, NEW_UID(uidChannel)
				);
		}

		MCommandResult OnRecv_KICK_REQ(CID cidPlayer, MUID uidChannel, wstring strTargetPlayer)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_KICK_REQ
				, 3
				, NEW_INT(cidPlayer)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayer.c_str())
				);
		}
		MCommandResult OnRecv_BAN_REQ(CID cidPlayer, MUID uidChannel, wstring strTargetPlayer)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_BAN_REQ
				, 3
				, NEW_INT(cidPlayer)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayer.c_str())
				);
		}
		MCommandResult OnRecv_UNBAN_REQ(CID cidPlayer, MUID uidChannel, wstring strTargetPlayer)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_UNBAN_REQ
				, 3
				, NEW_INT(cidPlayer)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayer.c_str())
				);
		}
		MCommandResult OnRecv_CHANGE_HOST_REQ(CID cidPlayer, MUID uidChannel, wstring strTargetPlayer)
		{
			return m_pLinkGameServer->OnRecv(MPC_MSG_CHANGE_HOST_REQ
				, 3
				, NEW_INT(cidPlayer)
				, NEW_UID(uidChannel)
				, NEW_WSTR(strTargetPlayer.c_str())
				);
		}

		int m_nGameServerID;
		MUID m_uidGameServer;
		MockLink* m_pLinkGameServer;

		CID m_cidOwner;
		wstring m_strOwnerName;

		CID m_cidTestPlayer;
		wstring m_strTestPlayerName;

	};

	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_CREATE_CHANNEL_PRIVATE_REQ_Success)
	{
		wstring strTestChannelName = L"MyTestChannel";

		CHECK_EQUAL(0, gmgr.pChatChannelManager->GetChannelCount());
		CHECK(NULL == gmgr.pChatChannelManager->FindChannel(strTestChannelName));

		OnRecv_CREATE_CHANNEL_PRIVATE_REQ(m_cidOwner, strTestChannelName);

		CHECK_EQUAL(1, gmgr.pChatChannelManager->GetChannelCount());
		CHECK(NULL != gmgr.pChatChannelManager->FindChannel(strTestChannelName));

		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_CHANNEL_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL,		m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_CREATE_CHANNEL_PRIVATE_REQ_Failure_AlreadyExistChannel)
	{
		wstring strTestChannelName = L"MyTestChannel";
		OnRecv_CREATE_CHANNEL_PRIVATE_REQ(m_cidOwner, strTestChannelName);
		CHECK_EQUAL(1, gmgr.pChatChannelManager->GetChannelCount());
		m_pLinkGameServer->ResetCommands();

		OnRecv_CREATE_CHANNEL_PRIVATE_REQ(m_cidOwner, strTestChannelName);
		CHECK_EQUAL(1, gmgr.pChatChannelManager->GetChannelCount());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,							m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,								m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_ALREADY_EXIST_CHANNEL_NAME,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_ENTER_CHANNEL_REQ_Success)
	{
		PChatChannel* pChannel = CreateTestChannel(m_cidOwner);
		CHECK_EQUAL(1, pChannel->GetPlayerCount());

		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_ADDED_PLAYER_INFO,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL,				m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_ENTER_CHANNEL_PLAYER_INFO,	m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_ENTER_CHANNEL_REQ_Failure_Not_ExistChannel)
	{
		PChatChannel* pChannel = CreateTestChannel(m_cidOwner);
		CHECK_EQUAL(1, pChannel->GetPlayerCount());

		wstring strNotExistChannel = L"NotExistChannel";
		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, strNotExistChannel);	///< 존재하지 않는 ChannelName 으로 입장 시도 - 실패
		CHECK_EQUAL(1, pChannel->GetPlayerCount());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,						m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_EXIST_CHANNEL,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_LEAVE_CHANNEL_REQ)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		OnRecv_LEAVE_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetUID());
		CHECK_EQUAL(1, pChannel->GetPlayerCount());

		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,		m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL,				m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_LEAVE_CHANNEL_PLAYER_INFO,	m_pLinkGameServer->GetCommand(2).GetID());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_KICK_REQ_Success)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		OnRecv_KICK_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);	///< TestPlayer 내쫓음
		CHECK_EQUAL(1, pChannel->GetPlayerCount());

		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_KICK,					m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_KICK_PLAYER_INFO,		m_pLinkGameServer->GetCommand(2).GetID());

		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());		///< TestPlayer 재입장 시도 - 성공
		CHECK_EQUAL(2, pChannel->GetPlayerCount());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_KICK_REQ_Failure_NotOwner)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		OnRecv_KICK_REQ(m_cidTestPlayer, pChannel->GetUID(), m_strOwnerName);	///< Owner 가 아닌 TestPlayer 로 Kick 요청 - 실패
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,				m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_KICK_REQ_Failure_NotExistTargetPlayer)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		wstring strNotExistPlayer = L"NotExistPlayer";
		OnRecv_KICK_REQ(m_cidOwner, pChannel->GetUID(), strNotExistPlayer);	///< 채널에 존재하지 않는 플레이어를 Kick 요청 - 실패
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,						m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_EXIST_PLAYER,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_BAN_REQ_Success)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		OnRecv_BAN_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);	///< TestPlayer 내쫓음
		CHECK_EQUAL(1, pChannel->GetPlayerCount());

		CHECK_EQUAL(3, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_PLAYER_INFO,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MPC_MSG_BAN,					m_pLinkGameServer->GetCommand(1).GetID());
		CHECK_EQUAL(MPC_MSG_BAN_PLAYER_INFO,		m_pLinkGameServer->GetCommand(2).GetID());

		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());		///< TestPlayer 재입장 시도 - 실패
		CHECK_EQUAL(1, pChannel->GetPlayerCount());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_BAN_REQ_Failure_NotOwner)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		OnRecv_BAN_REQ(m_cidTestPlayer, pChannel->GetUID(), m_strOwnerName);	///< Owner 가 아닌 TestPlayer 로 Ban 요청 - 실패
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,				m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_BAN_REQ_Failure_NotExistTargetPlayer)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		wstring strNotExistPlayer = L"NotExistPlayer";
		OnRecv_BAN_REQ(m_cidOwner, pChannel->GetUID(), strNotExistPlayer);	///< 채널에 존재하지 않는 플레이어를 Ban 요청
		CHECK_EQUAL(2, pChannel->GetPlayerCount());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,						m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_EXIST_PLAYER,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_UNBAN_REQ_Success)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		OnRecv_BAN_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);
		CHECK_EQUAL(1, pChannel->GetPlayerCount());							///< TestPlayer 는 Ban 당한 상태.

		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());		///< TestPlayer 는 Ban 당한 상태. 채널에 입장할 수 없다
		CHECK_EQUAL(1, pChannel->GetPlayerCount());
		m_pLinkGameServer->ResetCommands();

		OnRecv_UNBAN_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);	///< Owner 가 TestPlayer 의 Ban 을 해제.

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_UNBAN_RES,	m_pLinkGameServer->GetCommand(0).GetID());

		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());		///< TestPlayer 재입장 시도 - 성공
		CHECK_EQUAL(2, pChannel->GetPlayerCount());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_UNBAN_REQ_Failure_NotOwner)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		OnRecv_BAN_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);
		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());				///< TestPlayer 는 Ban 당한 상태. 채널에 입장할 수 없다
		CHECK_EQUAL(1, pChannel->GetPlayerCount());
		m_pLinkGameServer->ResetCommands();

		OnRecv_UNBAN_REQ(m_cidTestPlayer, pChannel->GetUID(), m_strTestPlayerName);	///< Owner 가 아닌 TestPlayer 가 자신의 Ban 을 해제 - 실패

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,				m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,	m_pLinkGameServer->GetParam<int>(0, 4));

		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());				///< TestPlayer 재입장 시도 - 실패
		CHECK_EQUAL(1, pChannel->GetPlayerCount());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_UNBAN_REQ_Failure_NotExistTargetPlayer)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		OnRecv_BAN_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);
		OnRecv_ENTER_CHANNEL_REQ(m_cidTestPlayer, pChannel->GetName());				///< TestPlayer 는 Ban 당한 상태. 채널에 입장할 수 없다
		CHECK_EQUAL(1, pChannel->GetPlayerCount());
		m_pLinkGameServer->ResetCommands();

		wstring strNotExistPlayer = L"NotExistPlayer";
		OnRecv_UNBAN_REQ(m_cidOwner, pChannel->GetUID(), strNotExistPlayer);

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,				m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_BANED,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_CHANGE_HOST_REQ_Success)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(m_cidOwner, pChannel->GetOwnerCID());

		OnRecv_CHANGE_HOST_REQ(m_cidOwner, pChannel->GetUID(), m_strTestPlayerName);	///< Owner 가 TestPlayer 에게 방장을 줌.

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_CHANGE_HOST_INFO,	m_pLinkGameServer->GetCommand(0).GetID());

		CHECK_EQUAL(m_cidTestPlayer, pChannel->GetOwnerCID());
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_CHANGE_HOST_REQ_Failure_NotOwner)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(m_cidOwner, pChannel->GetOwnerCID());

		OnRecv_CHANGE_HOST_REQ(m_cidTestPlayer, pChannel->GetUID(), m_strTestPlayerName);	///< Owner 가 아닌 TestPlayer 가 자신에게 방장을 줌 - 실패

		CHECK_EQUAL(m_cidOwner, pChannel->GetOwnerCID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,				m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_HOST,	m_pLinkGameServer->GetParam<int>(0, 4));
	}
	TEST_FIXTURE(Fixture, TestCmdHandler_Chat_CHANGE_HOST_REQ_Failure_NotExistTargetPlayer)
	{
		PChatChannel* pChannel = InitTestChannelEnteredTestPlayer();
		CHECK_EQUAL(m_cidOwner, pChannel->GetOwnerCID());

		wstring strNotExistPlayer = L"NotExistPlayer";
		OnRecv_CHANGE_HOST_REQ(m_cidOwner, pChannel->GetUID(), strNotExistPlayer);

		CHECK_EQUAL(m_cidOwner, pChannel->GetOwnerCID());

		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_RES,					m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(MT_SYSTEM,						m_pLinkGameServer->GetParam<int>(0, 0));
		CHECK_EQUAL(CR_FAIL_MSG_NOT_EXIST_PLAYER,	m_pLinkGameServer->GetParam<int>(0, 4));
	}

}