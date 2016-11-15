#include "stdafx.h"
#include "GUnitTestUtil.h"
#include "FBaseNetClient.h"
#include "STransData.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "SCommandTable.h"

SUITE(ProxyPlayer)
{
	struct Fixture: public FBaseNetClient
	{
		SProxyPlayerManager	m_manager;

		Fixture(): m_manager(m_pNetClient)	{}
		~Fixture()	{}

		TD_PROXY_PLAYER_INFO MakeProxyPlayerInfo(void)
		{
			TD_PROXY_PLAYER_INFO info;
			
			info.nAID = GUnitTestUtil::NewAID();
			info.nCID = GUnitTestUtil::NewCID();
			info.UID = GUnitTestUtil::NewUID();
			wcsncpy_s(info.szName, GUnitTestUtil::NewName().c_str(), _TRUNCATE);
			info.nGameServerID = GUnitTestUtil::NewID();
			info.uidField = GUnitTestUtil::NewUID();
			info.uidParty = GUnitTestUtil::NewUID();
			info.nFieldID = GUnitTestUtil::NewID();
			info.nChannelID = GUnitTestUtil::NewID();

			return info;
		}
	};

	TEST_FIXTURE(Fixture, TestAddPlayer)	
	{
		// arrange		
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();
			
		// act
		CHECK(m_manager.AddPlayer(info));

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		ASSERT_CHECK(pPlayer != NULL);
		CHECK_EQUAL(pPlayer->GetAID(), info.nAID);
		CHECK_EQUAL(pPlayer->GetCID(), info.nCID);
		CHECK_EQUAL(pPlayer->GetUID(), info.UID);
		CHECK(pPlayer->GetName() == info.szName);
		CHECK_EQUAL(pPlayer->GetGameServerID(), info.nGameServerID);
		CHECK_EQUAL(pPlayer->GetFieldUID(), info.uidField);
		CHECK_EQUAL(pPlayer->GetPartyUID(), info.uidParty);
		CHECK_EQUAL(pPlayer->GetFieldID(), info.nFieldID);
		CHECK_EQUAL(pPlayer->GetChannelID(), info.nChannelID);
	}

	TEST_FIXTURE(Fixture, TestRemovePlayer)	
	{
		// arrange
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();
		CHECK(m_manager.AddPlayer(info));
		
		// act
		CHECK(m_manager.RemovePlayer(info.nCID));

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		CHECK(pPlayer == NULL);
	}

	TEST_FIXTURE(Fixture, TestUpdatePlayer)	
	{
		// arrange
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();
		CHECK(m_manager.AddPlayer(info));

		CID cid = info.nCID;
		info = MakeProxyPlayerInfo();
		info.nCID = cid;		

		// act
		CHECK(m_manager.UpdatePlayer(info));

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		ASSERT_CHECK(pPlayer != NULL);
		CHECK_EQUAL(pPlayer->GetAID(), info.nAID);
		CHECK_EQUAL(pPlayer->GetCID(), info.nCID);
		CHECK_EQUAL(pPlayer->GetUID(), info.UID);
		CHECK(pPlayer->GetName() == info.szName);
		CHECK_EQUAL(pPlayer->GetGameServerID(), info.nGameServerID);
		CHECK_EQUAL(pPlayer->GetFieldUID(), info.uidField);
		CHECK_EQUAL(pPlayer->GetPartyUID(), info.uidParty);
		CHECK_EQUAL(pPlayer->GetFieldID(), info.nFieldID);
		CHECK_EQUAL(pPlayer->GetChannelID(), info.nChannelID);
	}

	TEST_FIXTURE(Fixture, TestClear)		
	{
		// arrange
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();
		CHECK(m_manager.AddPlayer(info));

		// act
		m_manager.Clear();

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		CHECK(pPlayer == NULL);
	}
	
	TEST_FIXTURE(Fixture, TestCmdHandler_ProxyPlayerAdd)		
	{
		// arrange
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();

		// act
		m_pNetAgent->OnRecv(MSC_PROXY_PLAYER_ADD, 1, NEW_SINGLE_BLOB(&info, sizeof(info)));

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		CHECK(pPlayer != NULL);
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_ProxyPlayerRemove)		
	{
		// arrange
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();
		CHECK(m_manager.AddPlayer(info));

		// act
		m_pNetAgent->OnRecv(MSC_PROXY_PLAYER_REMOVE, 1, NEW_INT(info.nCID));

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		CHECK(pPlayer == NULL);
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_ProxyPlayerUpdate)		
	{
		// arrange
		TD_PROXY_PLAYER_INFO info = MakeProxyPlayerInfo();
		CHECK(m_manager.AddPlayer(info));

		CID cid = info.nCID;
		info = MakeProxyPlayerInfo();
		info.nCID = cid;

		// act
		m_pNetAgent->OnRecv(MSC_PROXY_PLAYER_UPDATE, 1, NEW_SINGLE_BLOB(&info, sizeof(info)));

		// assert
		SProxyPlayer* pPlayer = m_manager.FindPlayer(info.nCID);
		ASSERT_CHECK(pPlayer != NULL);
		CHECK_EQUAL(pPlayer->GetAID(), info.nAID);
		CHECK_EQUAL(pPlayer->GetCID(), info.nCID);
		CHECK_EQUAL(pPlayer->GetUID(), info.UID);
		CHECK(pPlayer->GetName() == info.szName);
		CHECK_EQUAL(pPlayer->GetGameServerID(), info.nGameServerID);
		CHECK_EQUAL(pPlayer->GetFieldUID(), info.uidField);
		CHECK_EQUAL(pPlayer->GetPartyUID(), info.uidParty);
		CHECK_EQUAL(pPlayer->GetFieldID(), info.nFieldID);
		CHECK_EQUAL(pPlayer->GetChannelID(), info.nChannelID);
	}

	TEST_FIXTURE(Fixture, TestCmdHandler_AllProxyPlayerInfo)
	{
		// arrange
		const int MaxPlayer = 3;
		TD_PROXY_PLAYER_INFO info[MaxPlayer];
		for(int i = 0; i < MaxPlayer; i++)
		{
			info[i] = MakeProxyPlayerInfo();
		}

		// act
		m_pNetAgent->OnRecv(MSC_PROXY_ALL_PLAYER_INFO, 1, NEW_BLOB(info, sizeof(TD_PROXY_PLAYER_INFO), MaxPlayer));

		// assert
		for(int i = 0; i < MaxPlayer; i++)
		{
			SProxyPlayer* pPlayer = m_manager.FindPlayer(info[i].nCID);
			ASSERT_CHECK(pPlayer != NULL);
			CHECK_EQUAL(pPlayer->GetAID(), info[i].nAID);
			CHECK_EQUAL(pPlayer->GetCID(), info[i].nCID);
			CHECK_EQUAL(pPlayer->GetUID(), info[i].UID);
			CHECK(pPlayer->GetName() == info[i].szName);
			CHECK_EQUAL(pPlayer->GetGameServerID(), info[i].nGameServerID);
			CHECK_EQUAL(pPlayer->GetFieldUID(), info[i].uidField);
			CHECK_EQUAL(pPlayer->GetPartyUID(), info[i].uidParty);
			CHECK_EQUAL(pPlayer->GetFieldID(), info[i].nFieldID);
			CHECK_EQUAL(pPlayer->GetChannelID(), info[i].nChannelID);
		}
	}
}
