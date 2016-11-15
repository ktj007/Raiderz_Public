#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZMoveServerCommandRouter.h"
#include "CCommandTable_Master.h"


SUITE(MoveServerCommandRouter)
{
	struct FMoveServerCommandRouter : public FBaseLoginServer, FBaseMockLink2
	{
		FMoveServerCommandRouter()
		{
			MUID uidLoginServer = Connected(TEST_LOGINSERVER_ID);
			m_pLSLink = NewLink(uidLoginServer);


			ZGameServerObject* pGameServerObject = AddGameServerObject(TEST_GAMESERVER_ID);
			MUID uidGameServer = pGameServerObject->GetUID();
			m_pGSLink = NewLink(uidGameServer);


			m_uidPlayer = ZTestHelper::NewUID();
		}
		~FMoveServerCommandRouter()
		{

		}

		static const int TEST_LOGINSERVER_ID = 10;
		static const int TEST_GAMESERVER_ID = 1;

		MockLink* m_pLSLink;
		MockLink* m_pGSLink;
		MUID m_uidPlayer;
	};

	TEST_FIXTURE(FMoveServerCommandRouter, TestFMoveServerCommandRouter_Fixture)
	{
		CHECK(m_pLSLink != NULL);
		CHECK(m_pGSLink != NULL);
		CHECK(m_uidPlayer.IsValid());
	}

	TEST_FIXTURE(FMoveServerCommandRouter, TestRouteReserveMoveLoginServerRes)
	{
		MUID uidConnectionKey = ZTestHelper::NewUID();

		ZMoveServerCommandRouter::RouteReserveMoveLoginServerRes(m_pGSLink->GetUID(), m_uidPlayer, CR_SUCCESS, uidConnectionKey);

		CHECK_EQUAL(1, m_pGSLink->GetCommandCount());
		CHECK_EQUAL(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES, m_pGSLink->GetCommandID(0));
		CHECK_EQUAL(m_uidPlayer, m_pGSLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(CR_SUCCESS, m_pGSLink->GetParam<int>(0, 1));
		CHECK_EQUAL(uidConnectionKey, m_pGSLink->GetParam<MUID>(0, 2));
	}

	TEST_FIXTURE(FMoveServerCommandRouter, TestRouteReadyEnterLoginServerReq)
	{
		MUID uidConnectionKey = ZTestHelper::NewUID();
		AID nAID = ZTestHelper::NewID();
		wstring strUserID = ZTestHelper::NewName();

		ZMoveServerCommandRouter::RouteReadyEnterLoginServerReq(m_pLSLink->GetUID(), m_pGSLink->GetUID(), m_uidPlayer, uidConnectionKey, nAID, strUserID);

		CHECK_EQUAL(1, m_pLSLink->GetCommandCount());
		CHECK_EQUAL(MLC_COMM_READY_ENTER_LOGINSERVER_REQ, m_pLSLink->GetCommandID(0));
		CHECK_EQUAL(m_pGSLink->GetUID(), m_pLSLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_uidPlayer, m_pLSLink->GetParam<MUID>(0, 1));
		CHECK_EQUAL(uidConnectionKey, m_pLSLink->GetParam<MUID>(0, 2));
		CHECK_EQUAL(nAID, m_pLSLink->GetParam<int>(0, 3));
	}

	TEST_FIXTURE(FMoveServerCommandRouter, TestRouteCancelMoveLoginServer)
	{
		MUID uidConnectionKey = ZTestHelper::NewUID();

		ZMoveServerCommandRouter::RouteCancelMoveLoginServer(m_pLSLink->GetUID(), uidConnectionKey);

		CHECK_EQUAL(1, m_pLSLink->GetCommandCount());
		CHECK_EQUAL(MLC_COMM_CANCEL_MOVE_LOGINSERVER, m_pLSLink->GetCommandID(0));
		CHECK_EQUAL(uidConnectionKey, m_pLSLink->GetParam<MUID>(0, 0));
	}

}