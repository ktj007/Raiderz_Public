#include "StdAfx.h"
#include "PServableChecker.h"
#include "PServerInfo.h"
#include "PGlobal.h"
#include "MockServer.h"
#include "PFixtureHelper.h"
#include "PTestHelper.h"

#include "PMasterServerFacade.h"
#include "PGameServerObjectManager.h"

SUITE(ServableChecker)
{
	struct FServableChecker
	{
		PServableChecker m_ServableChecker;

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

		CHECK_EQUAL(false, gsys.pMasterServerFacade->GetNetClient()->IsConnected());

		m_ServableChecker.Update(0.0f);
		CheckFailure();

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CheckFailure();

		/// MasterServer 와 연결 설정
		gsys.pMasterServerFacade->GetNetClient()->Connect("", 0);
		CHECK_EQUAL(true, gsys.pMasterServerFacade->GetNetClient()->IsConnected());


		m_ServableChecker.Update(0.0f);
		CheckFailure();

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CheckSuccess();
	}
}
