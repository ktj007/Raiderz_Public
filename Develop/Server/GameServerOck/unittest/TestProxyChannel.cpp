#include "stdafx.h"
#include "GProxyChatChannel.h"
#include "GUnitTestUtil.h"

SUITE(ProxyChannel)
{
	struct Fixture
	{
		GProxyChatChannelManager m_manager;
		MUID m_uidChannel;
		wstring m_strChannelName;

		CID m_nCID;
		MUID m_uidPlayer;
		int m_nGameServerID;
		
		Fixture()	
		{
			m_uidChannel = GUnitTestUtil::NewUID();			
			m_strChannelName = L"TestChannelName";

			m_nCID = 1;
			m_uidPlayer = GUnitTestUtil::NewUID();
			m_nGameServerID = 2;
		}

		~Fixture()	{}
	};

	TEST_FIXTURE(Fixture, TestAddChannel)		
	{
		// arrange
		// act
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));

		// assert
		CHECK_EQUAL(m_manager.GetChannelCount(), 1);
		GProxyChatChannel* pChannel = m_manager.FindChannel(m_uidChannel);
		ASSERT_CHECK(pChannel != NULL);
		CHECK_EQUAL(pChannel->GetUID(), m_uidChannel);
		CHECK(pChannel->GetName() == m_strChannelName);
	}

	TEST_FIXTURE(Fixture, TestAddChannel_AleadyExistChannel)	
	{
		// arrange
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));

		// act/assert
		CHECK(!m_manager.AddChannel(m_uidChannel, m_strChannelName));
	}

	TEST_FIXTURE(Fixture, TestRemoveChannel)	
	{
		// arrange
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));

		// act
		CHECK(m_manager.RemoveChannel(m_uidChannel));

		// assert
		CHECK_EQUAL(m_manager.GetChannelCount(), 0);		
	}

	TEST_FIXTURE(Fixture, TestAddPlayer)		
	{
		// arrange
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));

		// act
		CHECK(m_manager.AddPlayer(m_uidChannel, m_nCID, m_nGameServerID, m_uidPlayer));

		// assert
		CHECK_EQUAL(m_manager.GetPlayerCount(), 1);
		CHECK_EQUAL(m_manager.GetPlayerCount(m_uidChannel), 1);
		GProxyChatPlayer* pPlayer = m_manager.FindPlayer(m_nCID);
		ASSERT_CHECK(pPlayer != NULL);
		CHECK_EQUAL(pPlayer->GetGameServerID(), m_nGameServerID);
		CHECK_EQUAL(pPlayer->GetUID(), m_uidPlayer);
	}

	TEST_FIXTURE(Fixture, TestRemovePlayer)		
	{
		// arrange
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));
		CHECK(m_manager.AddPlayer(m_uidChannel, m_nCID, m_nGameServerID, m_uidPlayer));
		
		// act
		CHECK(m_manager.RemovePlayer(m_uidChannel, m_nCID));

		// assert
		CHECK_EQUAL(m_manager.GetPlayerCount(), 0);
		GProxyChatChannel* pChannel = m_manager.FindChannel(m_uidChannel);
		ASSERT_CHECK(pChannel != NULL);
		CHECK_EQUAL(pChannel->GetPlayerCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestRemovePlayer_AllChannel)	
	{
		// arrange
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));		
		CHECK(m_manager.AddPlayer(m_uidChannel, m_nCID, m_nGameServerID, m_uidPlayer));

		// act
		CHECK(m_manager.RemovePlayer(m_nCID));

		// assert
		CHECK_EQUAL(m_manager.GetPlayerCount(), 0);
		GProxyChatChannel* pChannel = m_manager.FindChannel(m_uidChannel);
		ASSERT_CHECK(pChannel != NULL);
		CHECK_EQUAL(pChannel->GetPlayerCount(), 0);
	}
	
	TEST_FIXTURE(Fixture, TestRemoveChannel_InPlayer)	
	{
		// arrange
		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));		
		CHECK(m_manager.AddPlayer(m_uidChannel, m_nCID, m_nGameServerID, m_uidPlayer));

		// act
		CHECK(m_manager.RemoveChannel(m_uidChannel));

		// assert
		CHECK_EQUAL(m_manager.GetChannelCount(), 0);
		CHECK_EQUAL(m_manager.GetPlayerCount(), 0);		
	}

	TEST_FIXTURE(Fixture, TestUpdatePlayerInfoInLocal)	
	{
		// arrange
		int nUpdatedGameServerID = 3;
		MUID uidUpdatedUID = GUnitTestUtil::NewUID();

		CHECK(m_manager.AddChannel(m_uidChannel, m_strChannelName));		
		CHECK(m_manager.AddPlayer(m_uidChannel, m_nCID, m_nGameServerID, m_uidPlayer));

		// act
		CHECK(m_manager.UpdatePlayerInLocal(m_nCID, nUpdatedGameServerID, uidUpdatedUID));

		// assert
		GProxyChatPlayer* pPlayer = m_manager.FindPlayer(m_nCID);
		ASSERT_CHECK(pPlayer != NULL);
		CHECK_EQUAL(pPlayer->GetGameServerID(), nUpdatedGameServerID);
		CHECK_EQUAL(pPlayer->GetUID(), uidUpdatedUID);
	}
}
