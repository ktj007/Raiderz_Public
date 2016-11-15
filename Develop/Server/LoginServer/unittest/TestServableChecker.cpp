#include "StdAfx.h"
#include "LServableChecker.h"
#include "LServerInfo.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "STransData.h"
#include "MockNetServer.h"
#include "MockDBManager.h"
#include "LCmdHandlerGroup.h"
#include "MockServer.h"
#include "MockMasterNetClient.h"
#include "LTestWrapper.h"


SUITE(ServableChecker)
{
	class FakeDBManager : public MockDBManager
	{
	public:
		bool m_bCallGetServerStatusList;

		FakeDBManager() : m_bCallGetServerStatusList(false) {}

		bool GetServerStatusList(const int nWorldID, const std::vector<int>& vecServerID) override
		{
			m_bCallGetServerStatusList = true;
			for (size_t i = 0; i < vecServerID.size(); ++i)
			{
				gsys.pServer->GetServableChecker().UpdateServerStatusInfoByDB(vecServerID[i], true, true);
			}
			return true;
		}
	};

	struct FServableChecker
	{
		FServableChecker()
		{
			LServableChecker& refServableChecker = gsys.pServer->GetServableChecker();
			m_pServableChecker = &refServableChecker;
		}
		~FServableChecker()
		{
			m_pServableChecker->Reset();
		}

		void CheckFailure()
		{
			CHECK_EQUAL(false, m_pServableChecker->IsServable());
			CHECK_EQUAL(SERVER_STAT_INACTIVE, gsys.pServer->GetServerInfo().nStat);
		}

		void CheckSuccess()
		{
			CHECK_EQUAL(true, m_pServableChecker->IsServable());
			CHECK_EQUAL(SERVER_STAT_ACTIVE, gsys.pServer->GetServerInfo().nStat);
		}

		LServableChecker*	m_pServableChecker;

		LTestSysWrapper2<LDBManager, FakeDBManager>	m_wrapperDBManager;
	};

	TEST_FIXTURE(FServableChecker, TestServableChecker_Fixture)
	{
		CHECK_EQUAL(LServableChecker::SCP_WAIT, m_pServableChecker->GetPhase());
	}

	TEST_FIXTURE(FServableChecker, TestServableChecker_Start)
	{
		/// Fixture 설치
		std::vector<TD_SERVER_LIST_NODE> vecServerList;

		TD_SERVER_LIST_NODE tdNode;
		tdNode.nServerID = 1;
		tdNode.nServerType = SERVER_LOGIN;
		vecServerList.push_back(tdNode);

		tdNode.nServerID = 2;
		tdNode.nServerType = SERVER_GAME;
		vecServerList.push_back(tdNode);

		/// SUT 실행
		bool bRet = m_pServableChecker->Start(vecServerList);

		/// 검증
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(LServableChecker::SCP_STARTING, m_pServableChecker->GetPhase());
	}

	TEST_FIXTURE(FServableChecker, TestServableChecker_Start_Failure)
	{
		/// Fixture 설치
		std::vector<TD_SERVER_LIST_NODE> vecServerList;

		/// SUT 실행
		bool bRet = m_pServableChecker->Start(vecServerList);

		/// 검증
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(FServableChecker, TestServableChecker_Running)
	{
		/// Fixture 설치
		std::vector<TD_SERVER_LIST_NODE> vecServerList;
		TD_SERVER_LIST_NODE tdNode;
		tdNode.nServerID = 1;
		tdNode.nServerType = SERVER_LOGIN;
		vecServerList.push_back(tdNode);

		tdNode.nServerID = 2;
		tdNode.nServerType = SERVER_GAME;
		vecServerList.push_back(tdNode);

		m_pServableChecker->Start(vecServerList);

		m_pServableChecker->Update(SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC);
		
		CHECK_EQUAL(true, static_cast<FakeDBManager*>(gsys.pDBManager)->m_bCallGetServerStatusList);

		/// SUT 실행
		m_pServableChecker->Update(SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC);

		/// 검증
		CHECK_EQUAL(LServableChecker::SCP_RUNNING, m_pServableChecker->GetPhase());
	}

	struct FServableChecker_PhaseRunning : public FServableChecker
	{
		FServableChecker_PhaseRunning()
		{
			std::vector<TD_SERVER_LIST_NODE> vecServerList;
			TD_SERVER_LIST_NODE tdNode;
			tdNode.nServerID = 1;
			tdNode.nServerType = SERVER_LOGIN;
			vecServerList.push_back(tdNode);

			tdNode.nServerID = 2;
			tdNode.nServerType = SERVER_GAME;
			vecServerList.push_back(tdNode);
			m_pServableChecker->Start(vecServerList);

			m_pServableChecker->Update(SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC);
			m_pServableChecker->Update(SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC);
		}
	};

	TEST_FIXTURE(FServableChecker_PhaseRunning, TestServableChecker_PhaseRunning_Fixture)
	{
		CHECK_EQUAL(LServableChecker::SCP_RUNNING, m_pServableChecker->GetPhase());
	}

	TEST_FIXTURE(FServableChecker_PhaseRunning, TestServableChecker_PhaseRunning_Update)
	{
		const float TIME_CHECK_REGULATOR = SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC;

		CheckFailure();

		CHECK_EQUAL(false, gsys.pServer->GetMasterClient()->GetNetClient()->IsConnected());

		m_pServableChecker->Update(0.0f);
		CheckFailure();

		m_pServableChecker->Update(TIME_CHECK_REGULATOR);
		CheckFailure();

		/// 마스터서버 연결 설정
		gsys.pServer->GetMasterClient()->GetNetClient()->Connect(NULL, 0);
		CHECK_EQUAL(true, gsys.pServer->GetMasterClient()->GetNetClient()->IsConnected());

		m_pServableChecker->Update(0.0f);
		CheckFailure();

		m_pServableChecker->Update(TIME_CHECK_REGULATOR);
		CheckSuccess();

		/// MasterServer 연결 설정을 되돌립니다.
		gsys.pServer->GetMasterClient()->GetNetClient()->Disconnect();
	}

}
