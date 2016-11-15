#include "stdafx.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GPlayerAcceptManager.h"
#include "SUnitTestUtil.h"
#include "GPlayerGameData.h"
#include "GEntityPlayer.h"

SUITE(PlayerAcceptor)
{
	struct FPlayerAcceptor
	{
		FPlayerAcceptor()
		{

		}
		~FPlayerAcceptor()
		{

		}
		TD_LOGIN_ACCOUNT_INFO MakeParam()
		{
			TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo;

			wcsncpy_s(tdLoginAccountInfo.szUserID, L"birdkr", _TRUNCATE);
			tdLoginAccountInfo.nAID = 1; 
			tdLoginAccountInfo.nCID = 2;
			tdLoginAccountInfo.nEnterKey = 500;

			return tdLoginAccountInfo;
		}
		void CHECK_Param(GPlayerAcceptor_FromLoginServer& acceptor)
		{
			CHECK_EQUAL(1, acceptor.GetAID());
			CHECK_EQUAL(2, acceptor.GetCID());
			CHECK_EQUAL(500, acceptor.GetEnterKey());
		}

		MUIDGenerator UidGenerator;
	};

	TEST_FIXTURE(FPlayerAcceptor, TestPlayerAcceptorFromLoginServer_ReserveLogin)
	{
		GPlayerAcceptor_FromLoginServer acceptor;

		const MUID uidConnectionKey = SUnitTestUtil::NewUID();
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam();

		acceptor.ReserveLogin(&tdLoginAccountInfo, uidConnectionKey);

		CHECK_Param(acceptor);
		CHECK_EQUAL(uidConnectionKey, acceptor.GetConnectionKey());
	}


	TEST_FIXTURE(FPlayerAcceptor, TestPlayerAcceptorFromLoginServer_MakeFakeReserveLogin)
	{
		GPlayerAcceptor_FromLoginServer acceptor;

		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam();

		acceptor.MakeFakeReserveLogin(UidGenerator.Generate(), &tdLoginAccountInfo);

		CHECK_Param(acceptor);
		CHECK(MUID::ZERO != acceptor.GetConnectionKey());
	}

	TEST_FIXTURE(FPlayerAcceptor, TestPlayerAcceptorManager)
	{
		GPlayerAcceptManager mgr;
		GPlayerAcceptor_FromLoginServer* pAcceptor = new GPlayerAcceptor_FromLoginServer();

		const MUID uidConnectionKey = SUnitTestUtil::NewUID();
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam();
		pAcceptor->ReserveLogin(&tdLoginAccountInfo, uidConnectionKey);

		CHECK_Param(*pAcceptor);
		CHECK(MUID::ZERO != pAcceptor->GetConnectionKey());

		mgr.Add(pAcceptor, GPlayerAcceptor::FROM_LOGIN_SERVER);
		CHECK_EQUAL(1, mgr.GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));

		GPlayerAcceptor* pFoundAcceptor = mgr.FindPlayerAcceptor_FromLoginServer(uidConnectionKey);
		CHECK(pFoundAcceptor != NULL);

		CHECK_Param(*pAcceptor);
	}

	class FPlayerAcceptor_FromGameServer : public FPlayerAcceptor
	{
	public:
		FPlayerAcceptor_FromGameServer() : FPlayerAcceptor() {}
		virtual ~FPlayerAcceptor_FromGameServer() {}

		MCommand* NewCommand_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER(GEntityPlayer* pEntityPlayer)
		{
			GPlayerGameData playerGameData;
			MCommand* pMoveGameServerCommand = playerGameData.MakeMoveGameServerCommand(pEntityPlayer, MUID::ZERO, MUID::ZERO);



			return NULL;
		}

		GEntityPlayer		m_EntityPlayer;
	};

	TEST_FIXTURE(FPlayerAcceptor_FromGameServer, TestPlayerAcceptor_FromGameServer_ReserveLogin)
	{

		//
	}
}