#include "stdafx.h"
#include "FNotStandAlone.h"
#include "FBaseNetClient.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "FBasePlayer.h"
#include "FBaseMockLink.h"
#include "GUTHelper.h"
#include "GUTHelper_MasterServer.h"
#include "GSharedField.h"
#include "GFieldSystem.h"
#include "CCommandTable_Master.h"
#include "GServerGateRouter.h"
#include "GStandAloneModeGateRouter.h"
#include "GPlayerField.h"
#include "GPlayerFieldShared.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GTestForward.h"


SUITE(Channel_MasterServer)
{
	struct Fixture : public FNotStandAlone, FBaseNetClient, FBaseFieldInfo, FBaseFieldMgr, FBasePlayer, FBaseMockLink
	{
		enum TEST_CHANNEL_ID
		{
			TEST_CHANNEL_A = 0,
			TEST_CHANNEL_B = 1,
			TEST_CHANNEL_COUNT,
		};

		Fixture()
		{
			ChangeMasterServerRouter();
			InitFieldInfo();
			InitFields();
			InitPlayer();

			m_pNetAgent->ResetSendCommands();
			m_pTestPlayerLink->ResetCommands();
		}
		~Fixture()
		{
			RecoveryMasterServerRouter();
		}
		void ChangeMasterServerRouter()
		{
			gsys.pMasterServerFacade->SetGateRouter(new GServerGateRouter());
		}
		void RecoveryMasterServerRouter()
		{
			gsys.pMasterServerFacade->SetGateRouter(new GStandAloneModeGateRouter());
		}
		void InitFieldInfo()
		{
			m_pFieldInfo = AddFieldInfo(TEST_FIELD_ID);
			m_pFieldInfo->m_nDefaultChannelCount = 0;
			m_pFieldInfo->m_bDynamic = false;
		}
		void InitFields()
		{
			m_thelperMasterServer.AddNewSharedField(SUnitTestUtil::NewUID(), TEST_FIELD_ID, 1);
			m_thelperMasterServer.AddNewSharedField(SUnitTestUtil::NewUID(), TEST_FIELD_ID, 2);

			m_pFields[TEST_CHANNEL_A] = gmgr.pFieldMgr->GetSharedField(TEST_FIELD_ID, 1);
			m_pFields[TEST_CHANNEL_B] = gmgr.pFieldMgr->GetSharedField(TEST_FIELD_ID, 2);
		}
		void InitPlayer()
		{
			m_pPlayer			= GUTHelper::NewEntityPlayer((GField*)m_pFields[TEST_CHANNEL_A]);
			GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pPlayer->GetUID());
			pPlayerObject->OnStartGame();

			m_pTestPlayerLink	= NewLink(m_pPlayer);
		}

		MCommandResult OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES(MUID uidPlayer, int nResult, int nChannelID)
		{
			return m_pNetAgent->OnRecv(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES
				, 3
				, NEW_UID(uidPlayer)
				, NEW_INT(nResult)
				, NEW_INT(nChannelID));
		}

		GUTHelper_MasterServer	m_thelperMasterServer;

		static const int	TEST_FIELD_ID = 1;
		GFieldInfo*			m_pFieldInfo;
		GSharedField*		m_pFields[TEST_CHANNEL_COUNT];

		GEntityPlayer*		m_pPlayer;
		MockLink*			m_pTestPlayerLink;

		DECLWRAPPER_FakeSystem;
	};

	TEST_FIXTURE(Fixture, TestChannelMasterServer_Fixture)
	{
		ASSERT_CHECK(NULL != m_pFields[TEST_CHANNEL_A]);
		ASSERT_CHECK(NULL != m_pFields[TEST_CHANNEL_A]);

		CHECK_EQUAL(1,	m_pFields[TEST_CHANNEL_A]->GetPlayerQty());
		CHECK_EQUAL(0,	m_pFields[TEST_CHANNEL_B]->GetPlayerQty());

		CHECK_EQUAL(m_pPlayer->GetFieldUID(),	m_pFields[TEST_CHANNEL_A]->GetUID());
	}

	TEST_FIXTURE(Fixture, TestChannelMasterServer_PrepareChangeChannelReq_Success)
	{
		/// SUT 실행
		bool bRet = gsys.pFieldSystem->PrepareChangeChannel(m_pPlayer, m_pFields[TEST_CHANNEL_B]->GetChannelID());

		/// 검증
		CHECK_EQUAL(true, bRet);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		ASSERT_EQUAL(MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ,		m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetUID(),						m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(m_pFields[TEST_CHANNEL_B]->GetChannelID(),	m_pNetAgent->GetSendParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestChannelMasterServer_Recv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES_Success)
	{
		/// Fixture 설치
		gsys.pFieldSystem->PrepareChangeChannel(m_pPlayer, m_pFields[TEST_CHANNEL_B]->GetChannelID());
		m_pNetAgent->ResetSendCommands();

		/// SUT 실행
		OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES(m_pPlayer->GetUID(), CR_SUCCESS, m_pFields[TEST_CHANNEL_B]->GetChannelID());

		/// 검증
		CHECK_EQUAL(true, m_pPlayer->GetPlayerField().GetFieldShared().IsPreparedChangeChannel());

		ASSERT_EQUAL(1, m_pTestPlayerLink->GetCommandCount());
		ASSERT_EQUAL(MC_FIELD_PREPARE_CHANGE_CHANNEL_RES, m_pTestPlayerLink->GetCommandID(0));
	}

	TEST_FIXTURE(Fixture, TestChannelMasterServer_Recv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES_Failure)
	{
		/// Fixture 설치
		gsys.pFieldSystem->PrepareChangeChannel(m_pPlayer, m_pFields[TEST_CHANNEL_B]->GetChannelID());
		m_pNetAgent->ResetSendCommands();

		/// SUT 실행
		OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES(m_pPlayer->GetUID(), CR_FAIL, m_pFields[TEST_CHANNEL_B]->GetChannelID());

		/// 검증
		CHECK_EQUAL(false, m_pPlayer->GetPlayerField().GetFieldShared().IsPreparedChangeChannel());

		ASSERT_EQUAL(1,				m_pTestPlayerLink->GetCommandCount());
		ASSERT_EQUAL(MC_MSG_RES,	m_pTestPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL,		m_pTestPlayerLink->GetParam<int>(0, 4));
	}

	TEST_FIXTURE(Fixture, TestChannelMasterServer_CancelChangeChannel_Success)
	{
		/// Fixture 설치
		gsys.pFieldSystem->PrepareChangeChannel(m_pPlayer, m_pFields[TEST_CHANNEL_B]->GetChannelID());
		OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES(m_pPlayer->GetUID(), CR_SUCCESS, m_pFields[TEST_CHANNEL_B]->GetChannelID());
		m_pNetAgent->ResetSendCommands();

		/// SUT 실행
		bool bRet = gsys.pFieldSystem->CancelChangeChannel(m_pPlayer);

		/// 검증
		CHECK_EQUAL(true, bRet);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		ASSERT_EQUAL(MMC_GATE_CANCEL_CHANGE_CHANNEL,	m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetUID(),				m_pNetAgent->GetSendParam<MUID>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestChannelMasterServer_CancelChangeChannel_Failure_NotReservingChangeChannel)
	{
		// 예약 되어있지 않음
		CHECK_EQUAL(false, m_pPlayer->GetPlayerField().GetFieldShared().IsPreparedChangeChannel());

		/// SUT 실행
		bool bRet = gsys.pFieldSystem->CancelChangeChannel(m_pPlayer);

		/// 검증
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(Fixture, TestChannelMasterServer_ChangeChannel)
	{
		/// Fixture 설치
		gsys.pFieldSystem->PrepareChangeChannel(m_pPlayer, m_pFields[TEST_CHANNEL_B]->GetChannelID());
		OnRecv_MMC_GATE_PREPARE_CHANGE_CHANNEL_RES(m_pPlayer->GetUID(), CR_SUCCESS, m_pFields[TEST_CHANNEL_B]->GetChannelID());
		m_pNetAgent->ResetSendCommands();

		gsys.pSystem->Sleep(CHANNEL_CHANGE_PREPARE_TIME); ///< 채널 이동 대기시간 강제 스킵

		/// SUT 실행
		bool bRet = gsys.pFieldSystem->ChangeChannel(m_pPlayer);

		/// 검증
		CHECK_EQUAL(true, bRet);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		ASSERT_EQUAL(MMC_GATE_ASK_CHANGE_CHANNEL,	m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetUID(),			m_pNetAgent->GetSendParam<MUID>(0, 0));

		TD_GATE_INFO* pGateInfo;
		m_pNetAgent->GetSendCommand(0).GetSingleBlob(pGateInfo, 1);
		CHECK_EQUAL(pGateInfo->nFieldID,	TEST_FIELD_ID);
		CHECK_EQUAL(pGateInfo->nChannelID,	m_pFields[TEST_CHANNEL_B]->GetChannelID());
	}
}