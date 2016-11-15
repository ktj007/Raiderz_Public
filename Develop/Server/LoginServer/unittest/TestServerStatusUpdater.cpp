#include "StdAfx.h"
#include "LServerStatusUpdater.h"
#include "MockDBManager.h"

SUITE(ServerStatusUpdater)
{
	namespace testserverstatusupdater
	{
		class TestDBManager : public MockDBManager
		{
		public:
			bool bCall_ServerStatusStart;
			bool bCall_ServerStatusUpdate;

			TestDBManager()
			{
				bCall_ServerStatusStart = false;
				bCall_ServerStatusUpdate = false;
			}

			virtual bool ServerStatusStart(const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm) override
			{
				bCall_ServerStatusStart = true;
				return true;
			}
			
			virtual bool ServerStatusUpdate(const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable) override
			{
				bCall_ServerStatusUpdate = true;
				return true;
			}
		};
	}

	struct FServerStatusUpdater
	{
		FServerStatusUpdater()
		{
			pServerStatusUpdater = new LServerStatusUpdater(&m_testDBManager);
		}

		~FServerStatusUpdater()
		{
			delete pServerStatusUpdater;
		}

		testserverstatusupdater::TestDBManager m_testDBManager;
		LServerStatusUpdater* pServerStatusUpdater;
	};

	TEST_FIXTURE(FServerStatusUpdater, TestServerStatusUpdater_Start)
	{
		pServerStatusUpdater->Start(L"TestVersion");

		CHECK_EQUAL(true, m_testDBManager.bCall_ServerStatusStart);
		CHECK_EQUAL(false, m_testDBManager.bCall_ServerStatusUpdate);
	}

	TEST_FIXTURE(FServerStatusUpdater, TestServerStatusUpdater_Update)
	{
		pServerStatusUpdater->Update(10);
		CHECK_EQUAL(false, m_testDBManager.bCall_ServerStatusUpdate);

		pServerStatusUpdater->Update(10);
		CHECK_EQUAL(false, m_testDBManager.bCall_ServerStatusUpdate);

		pServerStatusUpdater->Update(10);
		CHECK_EQUAL(true, m_testDBManager.bCall_ServerStatusUpdate);


		m_testDBManager.bCall_ServerStatusUpdate = false;

		pServerStatusUpdater->Update(0);
		CHECK_EQUAL(false, m_testDBManager.bCall_ServerStatusUpdate);

		pServerStatusUpdater->Update(MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC);
		CHECK_EQUAL(true, m_testDBManager.bCall_ServerStatusUpdate);
	}

}