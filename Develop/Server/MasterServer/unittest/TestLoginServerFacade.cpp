#include "stdafx.h"
#include "ZFieldManager.h"
#include "ZFixtureHelper.h"


SUITE(LoginServerFacade)
{
	struct FLoginServerFacade : public FBaseLoginServer
	{
		FLoginServerFacade() : m_nLoginServerID(1), m_LoginServerUID(0, 1)
		{
			SetupServerInfo();
		}
		~FLoginServerFacade()
		{
		}

		void SetupServerInfo()
		{
			m_pServerInfo = AddLoginServerInfo(m_nLoginServerID);
		}

		// const
		MUID					m_LoginServerUID;
		int						m_nLoginServerID;

		ZLoginServerInfo*		m_pServerInfo;
	};

	TEST_FIXTURE(FLoginServerFacade, TestLoginServerObjectManager_Connect_Success)
	{
		CHECK_EQUAL(false, gmgr.pLoginServerFacade->IsConnected());

		gmgr.pLoginServerFacade->Connected(m_LoginServerUID, m_pServerInfo);

		CHECK_EQUAL(true, gmgr.pLoginServerFacade->IsConnected());
	}

	TEST_FIXTURE(FLoginServerFacade, TestLoginServerObjectManager_Disconnect_Success)
	{
		gmgr.pLoginServerFacade->Connected(m_LoginServerUID, m_pServerInfo);

		CHECK_EQUAL(true, gmgr.pLoginServerFacade->IsConnected());

		gmgr.pLoginServerFacade->Disconnect();

		CHECK_EQUAL(false, gmgr.pLoginServerFacade->IsConnected());

	}

}
