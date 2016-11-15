#include "StdAfx.h"
#include "ZServableChecker.h"
#include "ZServerInfo.h"
#include "ZGlobal.h"
#include "ZServer.h"
#include "ZFixtureHelper.h"


SUITE(ServableChecker)
{
	struct FServableChecker : public FBaseLoginServer, FBaseAppServer, FBaseGameServer
	{
		ZServableChecker m_ServableChecker;

		void CheckFailure()
		{
			CHECK_EQUAL(false, m_ServableChecker.IsServable());
			CHECK_EQUAL(SERVER_STAT_INACTIVE, gsys.pServer->GetServerInfo().nStat);
		}

		void CheckSuccess()
		{
			CHECK_EQUAL(true, m_ServableChecker.IsServable());
			CHECK_EQUAL(SERVER_STAT_ACTIVE, gsys.pServer->GetServerInfo().nStat);
		}
	};

	TEST_FIXTURE(FServableChecker, TestServableChecker)
	{
		const float TIME_CHECK_REGULATOR = SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC;

		CheckFailure();

		m_ServableChecker.Update(0.0f);
		CheckFailure();

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CheckFailure();


		MUID uidLoginServer = ZTestHelper::NewUID();
		int nLoginServerID = 1;
		ZLoginServerInfo* pLoginServerInfo = AddLoginServerInfo(nLoginServerID);
		gmgr.pLoginServerFacade->Connected(uidLoginServer, pLoginServerInfo);
		int nAppServerID = 1;	AddAppServerObject(nAppServerID);
		int nGameServerID = 1;	AddGameServerObject(nGameServerID, ZTestHelper::NewID());

		CHECK_EQUAL(true, gmgr.pLoginServerFacade->IsConnected());

		CHECK_EQUAL(1, gmgr.pAppServerInfoManager->GetSize());
		CHECK_EQUAL(1, gmgr.pAppServerObjectManager->GetClientsCount());

		CHECK_EQUAL(1, gmgr.pGameServerInfoManager->GetSize());
		CHECK_EQUAL(1, gmgr.pGameServerObjectManager->GetClientsCount());


		m_ServableChecker.Update(0.0f);
		CheckFailure();

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CheckSuccess();
	}
}
