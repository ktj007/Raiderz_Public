#include "stdafx.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "PChatChannel.h"
#include "PChatChannelManager.h"
#include "MockServer.h"
#include "MTest.h"
#include "PFixtureHelper.h"
#include "MockLink.h"
#include "CCommandTable_App.h"
#include "PChatChannelFactory.h"
#include "SUnitTestUtil.h"


SUITE(ChatChannelManager)
{
	struct Fixture: public FBasePlayer, FBaseGameServer
	{
		Fixture()
		{
			//gsys.pServer->Create();
			m_pTestChatChannelManager = m_wrapperpChatChannelManager.Get();
			
			m_nGameServerID = 1;
			m_uidGameServer = AddGameServerObject(m_nGameServerID);
			m_pLinkGameServer = PTestHelper::NewLink(m_uidGameServer);
		}
		~Fixture()
		{
			//gsys.pServer->Destroy();
		}

		PChatChannel* CreateChatChannel()
		{
			int nOwnerCID = SUnitTestUtil::NewID();
			std::wstring strChannelName = L"Test";
		
			PChatChannelFactory factory;
			PChatChannel* pChatChannel = factory.NewChatChannel_Private(nOwnerCID, strChannelName);			
			m_pTestChatChannelManager->AddChannel(pChatChannel);

			return pChatChannel;
		}
		PChatChannel* CreateChatChannel_Private(int nOwnerCID, const std::wstring& strChannelName)
		{
			MUID uidNew = SUnitTestUtil::NewUID();
			PChatChannelFactory factory;
			PChatChannel* pChatChannel = factory.NewChatChannel_Private(nOwnerCID, strChannelName);
			m_pTestChatChannelManager->AddChannel(pChatChannel);
			return pChatChannel;
		}
		PChatChannel* CreateChatChannel_Party(MUID uidParty)
		{
			PChatChannelFactory factory;
			PChatChannel* pChatChannel = factory.NewChatChannel_Party(uidParty);

			m_pTestChatChannelManager->AddChannel(pChatChannel);
			return pChatChannel;
		}

		PChatChannel* CreateChatChannel_Field(int nFieldID)
		{
			PChatChannelFactory factory;
			PChatChannel* pChatChannel = factory.NewChatChannel_Field(nFieldID);

			m_pTestChatChannelManager->AddChannel(pChatChannel);
			return pChatChannel;
		}

		PChatChannel* CreateChatChannel_Auction(void)
		{
			PChatChannelFactory factory;
			PChatChannel* pChatChannel = factory.NewChatChannel_Auction();

			m_pTestChatChannelManager->AddChannel(pChatChannel);
			return pChatChannel;
		}

		//PTestSysWrapper2<PServer, MockServer>	m_wrapperServer;
		PTestMgrWrapper<PChatChannelManager>	m_wrapperpChatChannelManager;
		PChatChannelManager*	m_pTestChatChannelManager;
		MockLink*				m_pLinkGameServer;
		int						m_nGameServerID;
		MUID					m_uidGameServer;
	};

	TEST_FIXTURE(Fixture, TestChatChannelManager_AddChannel_Private)
	{
		CHECK_EQUAL(0, m_pTestChatChannelManager->GetChannelCount());

		MUID uidNew = SUnitTestUtil::NewUID();
		PChatChannel* pChatChannel = new PChatChannel(uidNew, CCT_PRIVATE);

		int nCID = SUnitTestUtil::NewID();
		std::wstring strChannelName = L"TestChannel";
		pChatChannel->Init(nCID, strChannelName);

		CHECK_EQUAL(true,	m_pTestChatChannelManager->AddChannel(pChatChannel));
		CHECK_EQUAL(1,		m_pTestChatChannelManager->GetChannelCount());
	}
	TEST_FIXTURE(Fixture, TestChatChannelManager_AddChannel_Party)
	{
		CHECK_EQUAL(0, m_pTestChatChannelManager->GetChannelCount());

		MUID uidNew = SUnitTestUtil::NewUID();
		PChatChannel* pChatChannel = new PChatChannel(uidNew, CCT_PARTY);

		MUID uidParty = SUnitTestUtil::NewUID();
		pChatChannel->Init(uidParty);

		CHECK_EQUAL(true,	m_pTestChatChannelManager->AddChannel(pChatChannel));
		CHECK_EQUAL(1,		m_pTestChatChannelManager->GetChannelCount());
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_DeleteChannel)
	{
		PChatChannel* pChatChannel = CreateChatChannel();

		CHECK_EQUAL(1, m_pTestChatChannelManager->GetChannelCount());

		m_pTestChatChannelManager->DeleteChannel(pChatChannel);

		CHECK_EQUAL(0, m_pTestChatChannelManager->GetChannelCount());
	}
	
	TEST_FIXTURE(Fixture, TestChatChannelManager_EnterChannel)	
	{		
		// arrange		
		int nPlayerCID = AddPlayerObject(1);		
		PChatChannel* pChannel = CreateChatChannel();		
		m_pLinkGameServer->ResetCommands();

		// act
		CHECK(m_pTestChatChannelManager->EnterChannel(pChannel, nPlayerCID, 1));
		
		// assert
		CHECK(pChannel->IsExistPlayer(nPlayerCID));
		CHECK(m_pTestChatChannelManager->FindPlayer(nPlayerCID));
		CHECK(MPC_MSG_ADDED_PLAYER_INFO == m_pLinkGameServer->GetCommandID(0));
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_LeaveChannel)	
	{
		// arrange		
		int nPlayerCID = AddPlayerObject(1);		
		PChatChannel* pChannel = CreateChatChannel();				
		CHECK(m_pTestChatChannelManager->EnterChannel(pChannel, nPlayerCID, m_nGameServerID));
		m_pLinkGameServer->ResetCommands();

		// act
		m_pTestChatChannelManager->LeaveChannel(pChannel, nPlayerCID);

		// assert
		CHECK(!pChannel->IsExistPlayer(nPlayerCID));
		CHECK(NULL == m_pTestChatChannelManager->FindPlayer(nPlayerCID));
		CHECK(MPC_MSG_DELETED_PLAYER_INFO == m_pLinkGameServer->GetCommandID(0));
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_LeaveAllChannel)	
	{
		// arrange		
		int nPlayerCID = AddPlayerObject(1);		
		PChatChannel* pChannel1 = CreateChatChannel();				
		PChatChannel* pChannel2 = CreateChatChannel();				

		CHECK(m_pTestChatChannelManager->EnterChannel(pChannel1, nPlayerCID, m_nGameServerID));
		CHECK(m_pTestChatChannelManager->EnterChannel(pChannel2, nPlayerCID, m_nGameServerID));
		m_pLinkGameServer->ResetCommands();

		// act
		m_pTestChatChannelManager->LeaveAllChannel(nPlayerCID);	
		
		// assert
		CHECK(!pChannel1->IsExistPlayer(nPlayerCID));
		CHECK(!pChannel2->IsExistPlayer(nPlayerCID));
		CHECK(NULL == m_pTestChatChannelManager->FindPlayer(nPlayerCID));
		CHECK(MPC_MSG_DELETED_PLAYER_INFO == m_pLinkGameServer->GetCommandID(0));
		CHECK(MPC_MSG_DELETED_PLAYER_INFO == m_pLinkGameServer->GetCommandID(1));
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_FindChannel_ByUID)
	{
		PChatChannel* pChatChannel = CreateChatChannel();

		CHECK_EQUAL(1, m_pTestChatChannelManager->GetChannelCount());

		PChatChannel* pFindedChatChannel = m_pTestChatChannelManager->FindChannel(pChatChannel->GetUID());

		CHECK_EQUAL(pFindedChatChannel,				pChatChannel);
		CHECK_EQUAL(pFindedChatChannel->GetUID(),	pChatChannel->GetUID());
	}
	TEST_FIXTURE(Fixture, TestChatChannelManager_FindChannel_ByInvalidUID)
	{
		CreateChatChannel();

		CHECK_EQUAL(1, m_pTestChatChannelManager->GetChannelCount());

		MUID uidInvalid = MUID::ZERO;
		PChatChannel* pFindedChatChannel = m_pTestChatChannelManager->FindChannel(uidInvalid);

		CHECK(NULL == pFindedChatChannel);
	}
	TEST_FIXTURE(Fixture, TestChatChannelManager_FindChannel_ByChannelName)
	{
		int nOwnerCID = SUnitTestUtil::NewID();
		std::wstring strChannelName = L"TestChatChannel";
		PChatChannel* pChatChannel = CreateChatChannel_Private(nOwnerCID, strChannelName);

		PChatChannel* pFindedChatChannel = m_pTestChatChannelManager->FindChannel(strChannelName);

		CHECK_EQUAL(pFindedChatChannel,				pChatChannel);
		CHECK(pFindedChatChannel->GetName() == pChatChannel->GetName());

		pFindedChatChannel = m_pTestChatChannelManager->FindChannel(L"");

		CHECK(NULL == pFindedChatChannel);
	}
	TEST_FIXTURE(Fixture, TestChatChannelManager_FindChannel_Party)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		PChatChannel* pChatChannel = CreateChatChannel_Party(uidParty);

		PChatChannel* pFindedChatChannel = m_pTestChatChannelManager->FindChannel_Party(uidParty);

		CHECK_EQUAL(pFindedChatChannel,					pChatChannel);
		CHECK_EQUAL(pFindedChatChannel->GetPartyUID(),	pChatChannel->GetPartyUID());
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_FindChannelField)	
	{
		int nFieldID = SUnitTestUtil::NewID();
		PChatChannel* pChatChannel = CreateChatChannel_Field(nFieldID);

		PChatChannel* pFindedChatChannel = m_pTestChatChannelManager->FindChannel_Field(nFieldID);

		CHECK_EQUAL(pFindedChatChannel,					pChatChannel);
		CHECK_EQUAL(pFindedChatChannel->GetFieldID(),	pChatChannel->GetFieldID());
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_FindChannelAuction)	
	{
		PChatChannel* pChatChannel = CreateChatChannel_Auction();

		PChatChannel* pFindedChatChannel = m_pTestChatChannelManager->FindChannel_Auction();
		
		CHECK_EQUAL(pFindedChatChannel,					pChatChannel);		
	}

	TEST_FIXTURE(Fixture, TestChatChannelManager_FindPlayer)	
	{
		int nPlayerCID = SUnitTestUtil::NewID();
		AddPlayerObject(1, nPlayerCID);

		PChatChannel* pChannel = CreateChatChannel();
		
		CHECK(m_pTestChatChannelManager->EnterChannel(pChannel, nPlayerCID, m_nGameServerID));

		PChatPlayer* pPlayer = m_pTestChatChannelManager->FindPlayer(nPlayerCID);
		ASSERT_CHECK(pPlayer != NULL);
		CHECK(pChannel->IsExistPlayer(nPlayerCID));

		m_pTestChatChannelManager->LeaveChannel(pChannel, nPlayerCID);
		CHECK(!pChannel->IsExistPlayer(nPlayerCID));

		CHECK(NULL == m_pTestChatChannelManager->FindPlayer(nPlayerCID));
	}


	TEST_FIXTURE(Fixture, TestChatChannelManager_LaziliyDeleteChannel)
	{
		int nPlayerCID = SUnitTestUtil::NewID();
		AddPlayerObject(1, nPlayerCID);

		PChatChannel* pChannel = CreateChatChannel();
		m_pLinkGameServer->ResetCommands();

		CHECK_EQUAL(false, pChannel->IsReserveDelete());
		CHECK_EQUAL(1, m_pTestChatChannelManager->GetChannelCount());

		PChatChannel* pFindedChannel = m_pTestChatChannelManager->FindChannel(pChannel->GetUID());
		CHECK(NULL != pFindedChannel);


		m_pTestChatChannelManager->LazilyDeleteChannel(pChannel);
		CHECK_EQUAL(true, pChannel->IsReserveDelete());
		CHECK_EQUAL(0, m_pTestChatChannelManager->GetChannelCount());

		pFindedChannel = m_pTestChatChannelManager->FindChannel(pChannel->GetUID());
		CHECK(NULL == pFindedChannel);

		CHECK_EQUAL(0, m_pLinkGameServer->GetCommandCount());
		m_pTestChatChannelManager->Update(0.f);
		CHECK_EQUAL(1, m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MPC_MSG_DELETED_CHANNEL_INFO,	m_pLinkGameServer->GetCommandID(0));
	}
}
