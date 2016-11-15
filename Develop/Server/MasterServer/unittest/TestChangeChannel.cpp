#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZTestHelper_CmdHandler.h"
#include "ZFieldLogic.h"
#include "CCommandTable_Master.h"
#include "ZGameServerObject.h"
#include "ZPlayerLogic.h"
#include "ZSharedField.h"

SUITE(ChangeChannel)
{
	struct Fixture : public FBaseMockLink2, FBasePlayer
	{
		Fixture()
		{
			InitFieldInfo();
			InitGameServer();
			InitSharedField();
			InitPlayer();

			m_pLinkGameServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);
			m_pLinkGameServer->ResetCommands();
		}
		~Fixture()
		{
		}
		void InitFieldInfo()
		{
			m_nFieldID = 1;
			AddFieldInfo(m_nFieldID);
		}
		void InitGameServer()
		{
			InitDefaultGameServer();
			m_pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_nServerID);
			m_pLinkGameServer = NewLink(m_uidClient);

			/// 3번 채널만 담고있을 추가 게임서버 설정
			int nAnotherGameServerID = m_pGameServer->GetID() + 1;
			ZGameServerInfo* pServerInfo = AddGameServerInfo(nAnotherGameServerID, m_nFieldID, 1, 3);
			AddGameServerObject(pServerInfo);
		}
		void InitSharedField()
		{
			int nChannelID1 = 1;
			m_pChannel1 = AddSharedField(m_nFieldID, nChannelID1, m_pGameServer->GetID());

			int nChannelID2 = 2;
			m_pChannel2 = AddSharedField(m_nFieldID, nChannelID2, m_pGameServer->GetID());

			int nChannelID3 = 3;
			m_pChannel3_AnotherGameServer = gmgr.pFieldManager->FindSharedField(m_nFieldID, nChannelID3);
		}
		void InitPlayer()
		{
			m_pPlayer = AddNewPlayerInWorld(m_pGameServer->GetID());

			ZPlayerLogic playerLogic;
			playerLogic.InField(m_pGameServer->GetUID(), m_pPlayer->GetCID(), m_pChannel1->GetUID(), m_pChannel1->GetID());
		}

		ZGameServerObject*	m_pGameServer;
		int					m_nFieldID;
		ZSharedField*		m_pChannel1;
		ZSharedField*		m_pChannel2;
		ZSharedField*		m_pChannel3_AnotherGameServer;
		ZPlayer*			m_pPlayer;
		MockLink*			m_pLinkGameServer;

		ZFieldLogic			m_fieldLogic;
	};

	namespace PrepareChangeChannel
	{
		TEST_FIXTURE(Fixture, TestChangeChannel_PrepareChangeChannel_Success)
		{
			CHECK_EQUAL(MUID::ZERO,	m_pPlayer->GetReservedFieldUID());
			CHECK_EQUAL(0,			m_pChannel2->GetPlayerCount());

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());

			/// 검증
			CHECK_EQUAL(m_pChannel2->GetUID(),	m_pPlayer->GetReservedFieldUID());
			CHECK_EQUAL(1,						m_pChannel2->GetPlayerCount());

			CHECK_EQUAL(1,										m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),		m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_SUCCESS,					m_pLinkGameServer->GetParam<int>(0, 1));
			CHECK_EQUAL(m_pChannel2->GetChannelID(),m_pLinkGameServer->GetParam<int>(0, 2));
		}

		TEST_FIXTURE(Fixture, TestChangeChannel_PrepareChangeChannel_Failure_Duplicated)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());
			m_pLinkGameServer->ResetCommands();

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());

			/// 검증
			CHECK_EQUAL(m_pChannel2->GetUID(),	m_pPlayer->GetReservedFieldUID());
			CHECK_EQUAL(1,						m_pChannel2->GetPlayerCount());

			CHECK_EQUAL(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL_CHANGE_CHANNEL_PROCEED,			m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestChangeChannel_PrepareChangeChannel_Failure_RequestNotExistChannelID)
		{
			/// SUT 실행
			int nNotExistChannelID = 999;
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), nNotExistChannelID);

			/// 검증
			CHECK_EQUAL(MUID::ZERO,	m_pPlayer->GetReservedFieldUID());

			CHECK_EQUAL(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL_CHANGE_CHANNEL_IS_INVALID,		m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestChangeChannel_PrepareChangeChannel_Failure_DestChannelIsNotEnterable)
		{
			/// Fixture 설치
			for (int i = 0; i < m_pChannel2->GetPlayerLimit(); ++i)	m_pChannel2->IncreasePlayerCount();

			CHECK_EQUAL(false, m_pChannel2->IsEnterable());

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());

			/// 검증
			CHECK_EQUAL(MUID::ZERO,	m_pPlayer->GetReservedFieldUID());

			CHECK_EQUAL(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),					m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(CR_FAIL_CHANGE_CHANNEL_IS_FULL,			m_pLinkGameServer->GetParam<int>(0, 1));
		}
	}

	namespace CancelChangeChannel
	{
		TEST_FIXTURE(Fixture, TestChangeChannel_CancelChangeChannel_Success)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());
			m_pLinkGameServer->ResetCommands();

			CHECK_EQUAL(m_pChannel2->GetUID(),	m_pPlayer->GetReservedFieldUID());
			CHECK_EQUAL(1,						m_pChannel2->GetPlayerCount());

			/// SUT 실행
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_CANCEL_CHANGE_CHANNEL(m_pLinkGameServer, m_pPlayer->GetUID());

			/// 검증
			CHECK_EQUAL(MUID::ZERO,	m_pPlayer->GetReservedFieldUID());
			CHECK_EQUAL(0,			m_pChannel2->GetPlayerCount());
		}
	}

	namespace GateAskChangeChannel
	{
		TEST_FIXTURE(Fixture, TestChangeChannel_GateAskChangeChannel_Success_ShouldResponse_RGT_EXECUTE)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());
			m_pLinkGameServer->ResetCommands();

			/// SUT 실행
			TD_GATE_INFO gateInfo = ZTestHelper::MakeGateInfo_SharedField(m_pChannel2->GetID(), m_pChannel2->GetChannelID());
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK_CHANGE_CHANNEL(m_pLinkGameServer, m_pPlayer->GetUID(), gateInfo);

			/// 검증
			CHECK_EQUAL(1,						m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(RGT_EXECUTE,			m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestChangeChannel_GateAskChangeChannel_Success_ShouldResponse_MMC_GATE_MOVE_SERVER)
		{
			/// Fixture 설치
			CHECK(m_pPlayer->GetGameServerID() != m_pChannel3_AnotherGameServer->GetGameServerID());

			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel3_AnotherGameServer->GetChannelID());
			m_pLinkGameServer->ResetCommands();

			/// SUT 실행
			TD_GATE_INFO gateInfo = ZTestHelper::MakeGateInfo_SharedField(m_pChannel3_AnotherGameServer->GetID(), m_pChannel3_AnotherGameServer->GetChannelID());
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK_CHANGE_CHANNEL(m_pLinkGameServer, m_pPlayer->GetUID(), gateInfo);

			/// 검증
			CHECK_EQUAL(1,						m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_GATE_MOVE_SERVER,	m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(m_pChannel3_AnotherGameServer->GetGameServerID(), m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestChangeChannel_GateAskChangeChannel_Failure_NotReserved_ShouldResponse_RGT_FAIL)
		{
			CHECK_EQUAL(MUID::ZERO,				m_pPlayer->GetReservedFieldUID());

			/// SUT 실행
			TD_GATE_INFO gateInfo = ZTestHelper::MakeGateInfo_SharedField(m_pChannel2->GetID(), m_pChannel2->GetChannelID());
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK_CHANGE_CHANNEL(m_pLinkGameServer, m_pPlayer->GetUID(), gateInfo);

			/// 검증
			CHECK_EQUAL(1,						m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(RGT_FAIL,				m_pLinkGameServer->GetParam<int>(0, 1));
		}

		TEST_FIXTURE(Fixture, TestChangeChannel_GateAskChangeChannel_Failure_RequestNotExistChannelID_ShouldResponse_RGT_FAIL)
		{
			/// Fixture 설치
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ(m_pLinkGameServer, m_pPlayer->GetUID(), m_pChannel2->GetChannelID());
			m_pLinkGameServer->ResetCommands();

			/// SUT 실행
			int nRequestNotExistChannelID = 999;
			TD_GATE_INFO gateInfo = ZTestHelper::MakeGateInfo_SharedField(m_pChannel2->GetID(), nRequestNotExistChannelID);
			ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK_CHANGE_CHANNEL(m_pLinkGameServer, m_pPlayer->GetUID(), gateInfo);

			/// 검증
			CHECK_EQUAL(1,						m_pLinkGameServer->GetCommandCount());
			CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));
			CHECK_EQUAL(RGT_FAIL,				m_pLinkGameServer->GetParam<int>(0, 1));
		}
	}

}