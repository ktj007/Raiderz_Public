#include "stdafx.h"
#include "GProxyParty.h"
#include "FBaseNetClient.h"
#include "STransData_M2G.h"
#include "GCommandTable.h"
#include "GMasterServerFacade.h"
#include "SUnitTestUtil.h"

SUITE(ProxyParty)
{
	struct Fixture: public FBaseNetClient
	{
		MUID m_uidParty;
		MUID m_uidLeader;
		wstring m_strLeaderName;
		int	m_nLeaderCID;
		GProxyParty* m_pProxyParty;		
		GProxyPartyManager* m_pProxyPartyManager;

		Fixture()
		{
			m_uidParty = SUnitTestUtil::NewUID();
			m_uidLeader = SUnitTestUtil::NewUID();
			m_strLeaderName = L"TestLeaderName";
			
			m_pProxyParty = new GProxyParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
			m_pProxyPartyManager = gsys.pMasterServerFacade->GetProxyPartyManger();
		}
		
		~Fixture()
		{
			SAFE_DELETE(m_pProxyParty);
		}
	};

	// ProxyParty ////////////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestProxyParty_Creator)
	{
		GProxyParty party(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);

		CHECK_EQUAL(party.GetUID(), m_uidParty);
		CHECK_EQUAL(party.GetLeader(), m_uidLeader);
		CHECK(party.GetName(m_uidLeader) == m_strLeaderName);
		CHECK_EQUAL(party.GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyParty_AddMember)
	{
		CHECK_EQUAL(m_pProxyParty->GetCount(), 1);
		
		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		m_pProxyParty->AddMember(uidMember, strMemberName, nMemberCID);
		CHECK(m_pProxyParty->GetName(uidMember) == strMemberName);
		CHECK_EQUAL(m_pProxyParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestProxyParty_ChangeLeader)
	{
		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		m_pProxyParty->AddMember(uidMember, strMemberName, nMemberCID);
		
		CHECK_EQUAL(m_pProxyParty->GetLeader(), m_uidLeader);		
		CHECK_EQUAL(m_pProxyParty->GetCount(), 2);
		
		m_pProxyParty->ChangeLeader(uidMember);
		
		CHECK_EQUAL(m_pProxyParty->GetLeader(), uidMember);
		CHECK_EQUAL(m_pProxyParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestProxyParty_ChangeLeader_UnknownMemberFailure)
	{
		MUID uidNotMember = SUnitTestUtil::NewUID();
		
		CHECK_EQUAL(m_pProxyParty->GetLeader(), m_uidLeader);		
		CHECK_EQUAL(m_pProxyParty->GetCount(), 1);
		
		m_pProxyParty->ChangeLeader(uidNotMember);
		
		CHECK_EQUAL(m_pProxyParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(m_pProxyParty->GetCount(), 1);
	}

	// ProxyPartyManager /////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestProxyPartyManager_AddParty)
	{
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);

		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK(pProxyParty != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);

		MUID uidParty1 = SUnitTestUtil::NewUID();
		MUID uidLeader1 = SUnitTestUtil::NewUID();
		wstring strLeaderName1 = L"TestLeaderName1";
		int nLeaderCID1 = SUnitTestUtil::NewCID();
		CHECK(m_pProxyPartyManager->AddParty(uidParty1, uidLeader1, strLeaderName1, nLeaderCID1) != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 2);

		MUID uidParty2 = SUnitTestUtil::NewUID();
		MUID uidLeader2 = SUnitTestUtil::NewUID();
		wstring strLeaderName2 = L"TestLeaderName2";
		int nLeaderCID2 = SUnitTestUtil::NewCID();
		CHECK(m_pProxyPartyManager->AddParty(uidParty2, uidLeader2, strLeaderName2, nLeaderCID2) != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 3);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_AddParty_AlreadyAddedPartyFailure)
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK(pProxyParty != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);

		CHECK(m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID) != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_RemoveParty)
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK(pProxyParty != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);

		m_pProxyPartyManager->RemoveParty(m_uidParty);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);

		m_pProxyPartyManager->RemoveParty(m_uidParty);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_RemoveParty_UnknownPartyFailure)
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK(pProxyParty != NULL);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);

		MUID uidUnknown = SUnitTestUtil::NewUID();

		m_pProxyPartyManager->RemoveParty(uidUnknown);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_AddMember)
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);

		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();

		m_pProxyPartyManager->AddMember(m_uidParty, uidMember, strMemberName, nMemberCID);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_AddMember_AlreadyAddedMemberFailure)
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);

		m_pProxyPartyManager->AddMember(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_AddMember_UnknownPartyFailure)
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);

		MUID uidUnkownParty = SUnitTestUtil::NewUID();
		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		m_pProxyPartyManager->AddMember(uidUnkownParty, uidMember, strMemberName, nMemberCID);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_RemoveMember)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);

		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		m_pProxyPartyManager->AddMember(m_uidParty, uidMember, strMemberName, nMemberCID);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);

		m_pProxyPartyManager->RemoveMember(m_uidParty, uidMember);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_RemoveMember_OnlyLeaderParty)	
	{
		const GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);

		m_pProxyPartyManager->RemoveMember(m_uidParty, m_uidLeader);

		CHECK_EQUAL(pProxyParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_RemoveMember_RemoveLeaderFailure)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);
		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		pProxyParty->AddMember(uidMember, strMemberName, nMemberCID);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);

		m_pProxyPartyManager->RemoveMember(m_uidParty, m_uidLeader);
		CHECK_EQUAL(pProxyParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_ChangeLeader)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty);

		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		m_pProxyPartyManager->AddMember(m_uidParty, uidMember, strMemberName, nMemberCID);
		CHECK_EQUAL(pProxyParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);

		m_pProxyPartyManager->ChangeLeader(m_uidParty, uidMember);
		CHECK_EQUAL(pProxyParty->GetLeader(), uidMember);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_Clear)	
	{
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);

		m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);

		m_pProxyPartyManager->Clear();
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_Find)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty != NULL);

		const GProxyParty* pFindedProxyParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pFindedProxyParty != NULL);

		CHECK_EQUAL(pProxyParty, pFindedProxyParty);
	}

	TEST_FIXTURE(Fixture, TestProxyPartyManager_Find_UnknownPartyFailure)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty != NULL);

		MUID uidParty = SUnitTestUtil::NewUID();
		const GProxyParty* pFindedProxyParty = m_pProxyPartyManager->Find(uidParty);
		CHECK(pFindedProxyParty == NULL);
	}

	// PartyCommandHandler ///////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAdd)	
	{
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);
		MCommandResult nResult = m_pNetAgent->OnRecv(MMC_PARTY_ADD, 4, NEW_UID(m_uidParty), NEW_UID(m_uidLeader), NEW_WSTR(m_strLeaderName.c_str()), NEW_INT(m_nLeaderCID));

		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);		
		
		const GProxyParty* pProxyParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pProxyParty != NULL);
		CHECK_EQUAL(pProxyParty->GetUID(), m_uidParty);
		CHECK_EQUAL(pProxyParty->GetLeader(), m_uidLeader);
		CHECK(pProxyParty->GetName(m_uidLeader) == m_strLeaderName);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyRemove)	
	{
		m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 1);

		MCommandResult nResult = m_pNetAgent->OnRecv(MMC_PARTY_REMOVE, 1, NEW_UID(m_uidParty));
		
		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(m_pProxyPartyManager->GetCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAddMember)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty != NULL);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);

		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		MCommandResult nResult = m_pNetAgent->OnRecv(MMC_PARTY_ADD_MEMBER, 4, NEW_UID(m_uidParty), NEW_UID(uidMember), NEW_WSTR(strMemberName.c_str()), NEW_INT(nMemberCID));
		
		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyRemoveMember)	
	{
		GProxyParty* pProxyParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidLeader, m_strLeaderName, m_nLeaderCID);
		ASSERT_CHECK(pProxyParty != NULL);

		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		pProxyParty->AddMember(uidMember, strMemberName, nMemberCID);
		CHECK_EQUAL(pProxyParty->GetCount(), 2);
		
		MCommandResult nResult = m_pNetAgent->OnRecv(MMC_PARTY_REMOVE_MEMBER, 2, NEW_UID(m_uidParty), NEW_UID(uidMember));

		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(pProxyParty->GetCount(), 1);
	}
}
