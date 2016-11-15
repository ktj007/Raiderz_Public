#include "stdafx.h"
#include "GProxyChatChannel.h"
#include "SUnitTestUtil.h"
#include "GUTHelper_Player.h"
#include "GConfig.h"
#include "FBasePlayer.h"
#include "FBaseAppServerFacade.h"
#include "MockLink.h"
#include "FBaseMockLink.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"


SUITE(ProxyChatChannelManager)
{
	struct Fixture : FBaseAppServerFacade, FBasePlayer, FBaseMockLink
	{
		Fixture()
		{
			m_pProxyChatChannelManager = gsys.pAppServerFacade->GetChatChannelManager();

			AddPlayer();
		}
		~Fixture()
		{

		}
		void AddPlayer()
		{
			GUTHelper_Player helperPlayer;
			m_pPlayer1 = helperPlayer.NewPlayer();
			m_pLinkPlayer1 = NewLink(m_pPlayer1);

			m_pPlayer2 = helperPlayer.NewPlayer();
			m_pLinkPlayer2 = NewLink(m_pPlayer2);
		}

		GEntityPlayer* m_pPlayer1;
		MockLink* m_pLinkPlayer1;

		GEntityPlayer* m_pPlayer2;
		MockLink* m_pLinkPlayer2;

		GProxyChatChannelManager* m_pProxyChatChannelManager;
	};

	TEST_FIXTURE(Fixture, TestGProxyChatChannelManager_DisconnectAppServer)
	{
		MUID uidChannel = SUnitTestUtil::NewUID();
		wstring strName = SUnitTestUtil::NewName();
		m_pProxyChatChannelManager->AddChannel(uidChannel, strName);

		m_pProxyChatChannelManager->AddPlayer(uidChannel, m_pPlayer1->GetPlayerInfo()->nCID, GConfig::m_nMyServerID, m_pPlayer1->GetUID());
		m_pProxyChatChannelManager->AddPlayer(uidChannel, m_pPlayer2->GetPlayerInfo()->nCID, GConfig::m_nMyServerID, m_pPlayer2->GetUID());

		CHECK_EQUAL(1, m_pProxyChatChannelManager->GetChannelCount());
		CHECK_EQUAL(2, m_pProxyChatChannelManager->GetPlayerCount());

		/// SUT 실행
		m_pProxyChatChannelManager->DisconnectAppServer();

		/// 검증
		CHECK_EQUAL(0, m_pProxyChatChannelManager->GetChannelCount());
		CHECK_EQUAL(0, m_pProxyChatChannelManager->GetPlayerCount());

		CHECK_EQUAL(1,						m_pLinkPlayer1->GetCommandCount());
		CHECK_EQUAL(MC_MSG_LEAVE_CHANNEL,	m_pLinkPlayer1->GetCommandID(0));

		CHECK_EQUAL(1,						m_pLinkPlayer2->GetCommandCount());
		CHECK_EQUAL(MC_MSG_LEAVE_CHANNEL,	m_pLinkPlayer2->GetCommandID(0));
	}

}