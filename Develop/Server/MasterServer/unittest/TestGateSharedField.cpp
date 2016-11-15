#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "STransData_M2G.h"
#include "ZCommandTable.h"
#include "ZTestHelper_CmdHandler.h"
#include "ZSharedFieldMaster.h"
#include "ZSharedField.h"
#include "ZGameServerObject.h"
#include "ZPlayerLogic.h"

SUITE(TestGateSharedField)
{
	struct Fixture : public FBaseMockLink2, FBasePlayer
	{
		Fixture()
		{
			InitGameServer();
			InitFieldInfo();
			InitSharedField();
			InitPlayer();
		}
		~Fixture()
		{
			
		}
		void InitGameServer()
		{
			InitDefaultGameServer();
			m_nGameServerID = m_nServerID;
			m_pLinkGameServer = NewLink(m_uidClient);

			m_pLinkGameServer->ResetCommands();
			m_pLinkGameServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);
		}
		void InitFieldInfo()
		{
			m_nFieldID1 = 1;
			m_nFieldID2 = 2;
			AddFieldInfo(m_nFieldID1);
			AddFieldInfo(m_nFieldID2);
		}
		void InitSharedField()
		{
			m_nDefaultChannelID1 = 1;
			m_pField1 = AddSharedField(m_nFieldID1, m_nDefaultChannelID1, m_nGameServerID);
			m_pField2 = AddSharedField(m_nFieldID2, m_nDefaultChannelID1, m_nGameServerID);
		}
		void InitPlayer()
		{
			m_pPlayer = AddNewPlayerInWorld(m_nGameServerID);
		}
		
		int				m_nGameServerID;
		int				m_nFieldID1;
		int				m_nFieldID2;
		ZSharedField*	m_pField1;
		ZSharedField*	m_pField2;
		int				m_nDefaultChannelID1;
		ZPlayer*		m_pPlayer;
		MockLink*		m_pLinkGameServer;
		ZPlayerLogic	m_playerLogic;
	};

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK)
	{
		/// Fixture 설치
		CHECK_EQUAL(MUID::ZERO,	m_pPlayer->GetReservedFieldUID());
		CHECK_EQUAL(0,			m_pField1->GetPlayerCount());
		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFieldID1, m_nDefaultChannelID1);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(m_pField1->GetUID(),	m_pPlayer->GetReservedFieldUID());
		CHECK_EQUAL(1,						m_pField1->GetPlayerCount());

		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(RGT_EXECUTE,			m_pLinkGameServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK_NotExistChannel)
	{
		/// Fixture 설치
		int nNotExistChannelID = -1;
		ZSharedField* pNotExistChannel = gmgr.pFieldManager->FindSharedField(m_nFieldID1, nNotExistChannelID);
		CHECK(NULL == pNotExistChannel);
		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFieldID1, nNotExistChannelID);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(m_pField1->GetUID(),	m_pPlayer->GetReservedFieldUID());
		CHECK_EQUAL(1,						m_pField1->GetPlayerCount());

		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(),	m_pLinkGameServer->GetParam<MUID>(0, 0));
		CHECK_EQUAL(RGT_EXECUTE,			m_pLinkGameServer->GetParam<int>(0, 1));

		TD_GATE_INFO* pGateInfo;
		int nCount;
		m_pLinkGameServer->GetCommand(0).GetBlob(pGateInfo, nCount, 2);
		CHECK_EQUAL(m_pField1->GetID(),			pGateInfo->nFieldID);
		CHECK_EQUAL(m_pField1->GetChannelID(),	pGateInfo->nChannelID);
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK_Invalid_FieldID)
	{
		/// Fixture 설치
		int nInvalidFieldID = -1;
		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(nInvalidFieldID, m_nDefaultChannelID1);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(MUID::ZERO,				m_pPlayer->GetReservedFieldUID());

		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(RGT_FAIL,				m_pLinkGameServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK_Exist_ReservedFIeldUID)
	{
		/// Fixture 설치
		m_pPlayer->SetReservedFieldUID(m_pField1->GetUID());

		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFieldID1, m_nDefaultChannelID1);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(RGT_EXECUTE,			m_pLinkGameServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK_Invalid_ReservedFIeldUID)
	{
		/// Fixture 설치
		MUID uidInvalid = 9999999;
		m_pPlayer->SetReservedFieldUID(uidInvalid);

		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFieldID1, m_nDefaultChannelID1);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(m_pField1->GetUID(),	m_pPlayer->GetReservedFieldUID());

		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(RGT_EXECUTE,			m_pLinkGameServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK_Another_ReservedFIeld)
	{
		/// Fixture 설치
		m_playerLogic.PrepareGateField(m_pPlayer, m_pField2);

		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFieldID1, m_nDefaultChannelID1);
		
		CHECK_EQUAL(1, m_pField2->GetPlayerCount());
		CHECK_EQUAL(0, m_pField1->GetPlayerCount());

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(m_pField1->GetUID(),	m_pPlayer->GetReservedFieldUID());

		CHECK_EQUAL(0, m_pField2->GetPlayerCount());
		CHECK_EQUAL(1, m_pField1->GetPlayerCount());

		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(RGT_EXECUTE,			m_pLinkGameServer->GetParam<int>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_MMC_GATE_ASK_NotExistEnterableChannel_Response_RGT_RETRY)
	{
		/// Fixture 설치
		for (int i = 0; i < m_pField1->GetPlayerLimit(); ++i) m_pField1->IncreasePlayerCount();

		ZSharedFieldMaster* pFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(m_nFieldID1);
		ZSharedField* pEnterableField = pFieldMaster->GetEnterableChannel();
		CHECK(NULL == pEnterableField);

		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFieldID1, m_nDefaultChannelID1);

		/// SUT 실행
		ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkGameServer, m_pPlayer->GetUID(), tdGateInfo);

		/// 검증
		CHECK_EQUAL(MMC_GATE_RES,			m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(RGT_RETRY,				m_pLinkGameServer->GetParam<int>(0, 1));
	}

}