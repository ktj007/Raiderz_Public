#include "stdafx.h"
#include "GMasterServerMoveServerRouter.h"
#include "FBaseNetClient.h"
#include "GUnitTestUtil.h"
#include "CCommandTable_Master.h"
#include "GClientMoveServerRouter.h"
#include "FBasePlayer.h"
#include "FBaseMockLink.h"
#include "GTestForward.h"
#include "CCommandTable_Login.h"


SUITE(MoveServerRouter)
{
	struct FMoveServerRouter :public FBaseNetClient
	{
		FMoveServerRouter()
		{

		}
		~FMoveServerRouter()
		{

		}
	};

	TEST_FIXTURE(FMoveServerRouter, TestMoveServerRouter_RouteReserveMoveLoginServerReq)
	{
		MUID uidPlayer = GUnitTestUtil::NewUID();

		GMasterServerMoveServerRouter router;
		router.RouteReserveMoveLoginServerReq(uidPlayer);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidPlayer, m_pNetAgent->GetSendParam<MUID>(0, 0));
	}

	TEST_FIXTURE(FMoveServerRouter, TestMoveServerRouter_RouteCancelMoveLoginServer)
	{
		MUID uidPlayer = GUnitTestUtil::NewUID();

		GMasterServerMoveServerRouter router;
		router.RouteCancelMoveLoginServer(uidPlayer);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_COMM_CANCEL_MOVE_LOGINSERVER, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidPlayer, m_pNetAgent->GetSendParam<MUID>(0, 0));
	}

	struct FClientMoveServerRouter: public FBasePlayer, FBaseMockLink
	{
		FClientMoveServerRouter()
		{
			//
			m_pMockField = test.field.NewMockField();
			m_pPlayer = NewEntityPlayer(m_pMockField);
			
			m_pPlayerLink = NewLink(m_pPlayer->GetUID());
		}
		~FClientMoveServerRouter()
		{
			m_pMockField->Destroy();
			SAFE_DELETE(m_pMockField);
		}

		GEntityPlayer*	m_pPlayer;
		MockField*		m_pMockField;
		MockLink*		m_pPlayerLink;
	};

	TEST_FIXTURE(FClientMoveServerRouter, TestClientMoveServerRouter_RouteStartMoveLoginServerRes)
	{
		int nResult = GUnitTestUtil::NewID();
		MUID uidConnectionKey = GUnitTestUtil::NewUID();

		GClientMoveServerRouter router;
		router.RouteStartMoveLoginServerRes(m_pPlayer->GetUID(), nResult, uidConnectionKey);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(nResult, m_pPlayerLink->GetParam<int>(0, 0));
		CHECK_EQUAL(uidConnectionKey, m_pPlayerLink->GetParam<MUID>(0, 1));
	}

}