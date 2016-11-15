#include "stdafx.h"
#include "LClientConnectionKeeper.h"
#include "MockServer.h"
#include "LTestWrapper.h"
#include "LMasterClient.h"
#include "MockMasterNetClient.h"
#include "LCommandTable.h"
#include "LSystem.h"

SUITE(ClientConnectionKeeper)
{
	class FClientConnectionKeeper
	{
	public:
		FClientConnectionKeeper()
		{
			m_pNetClient = gsys.pServer->GetMasterClient()->GetNetClient();
			m_pNetAgent = new minet::MTestNetAgent(m_pNetClient);
		}
		virtual ~FClientConnectionKeeper()
		{
			SAFE_DELETE(m_pNetAgent);
		}

		MNetClient*				m_pNetClient;
		minet::MTestNetAgent*	m_pNetAgent;
	};

	TEST_FIXTURE(FClientConnectionKeeper, ClientConnectionKeeper_OnNetError)
	{
		LClientConnectionKeeper* pKeeper = new LClientConnectionKeeper(m_pNetClient, L"", 0, gsys.pSystem);

		pKeeper->OnNetError(10061);
		gsys.pServer->Update(0.1f);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MC_LOCAL_NET_CONNECT_WITH_NIC, m_pNetAgent->GetSendCommandID(0));		// 끊어졌으면 연결 시도 커맨드 보냄

		delete pKeeper;
	}

}