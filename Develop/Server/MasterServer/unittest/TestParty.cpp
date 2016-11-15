#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZParty.h"
#include "ZPartyManager.h"
#include "ZPartyLogic.h"
#include "STransData_M2G.h"
#include "CCommandTable_Master.h"
#include "CTransData.h"
#include "ZPartyMember.h"

SUITE(Party)
{
	struct Fixture
	{
		MUID m_uidParty;
		MUID m_uidLeader;
		wstring m_strLeaderName;
		int m_nLeaderCID;
		ZParty* m_pParty;
		ZPartyManager m_partyManager;

		MUID m_uidMember1;
		wstring m_strMemberName1;
		int m_nMemberCID1;
		MUID m_uidMember2;
		wstring m_strMemberName2;
		int m_nMemberCID2;

		Fixture()
		{
			m_uidParty = ZTestHelper::NewUID();
			m_uidLeader = ZTestHelper::NewUID();
			m_strLeaderName = L"TestLeaderName";
			m_nLeaderCID = ZTestHelper::NewCID();

			m_pParty = new ZParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		}

		~Fixture()
		{
			SAFE_DELETE(m_pParty);
		}

		void AddMoreMember()
		{
			m_uidMember1 = ZTestHelper::NewUID();
			m_strMemberName1 = L"TestMemberName1";
			m_nMemberCID1 = ZTestHelper::NewCID();
			m_uidMember2 = ZTestHelper::NewUID();
			m_strMemberName2 = L"TestMemberName2";
			m_nMemberCID2 = ZTestHelper::NewCID();

			m_pParty->AddMember(m_uidMember1, m_strMemberName1, 1, m_nMemberCID1);
			m_pParty->AddMember(m_uidMember2, m_strMemberName2, 1, m_nMemberCID2);
		}
	};


	// ZParty ////////////////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestParty_Creator)	
	{
		ZParty party(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		
		ZPartyMember* pPartyMember = party.GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);

		CHECK_EQUAL(party.GetUID(), m_uidParty);
		CHECK_EQUAL(party.GetLeader(), m_uidLeader);
		CHECK_EQUAL(party.GetCount(), 1);

		CHECK(pPartyMember->GetName() == m_strLeaderName);
	}

	TEST_FIXTURE(Fixture, TestParty_AddMember)
	{
		CHECK_EQUAL(m_pParty->GetCount(), 1);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);

		CHECK_EQUAL(m_pParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestParty_AddMember_AlreadyAddedMemberFailure)
	{
		CHECK_EQUAL(m_pParty->GetCount(), 1);
				
		m_pParty->AddMember(m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);

		CHECK_EQUAL(m_pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveMember)	
	{
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);
		CHECK_EQUAL(m_pParty->GetCount(), 2);

		m_pParty->RemoveMember(uidMember);
		CHECK_EQUAL(m_pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveMember_RemoveLeaderFailure)
	{
		CHECK_EQUAL(m_pParty->GetCount(), 1);
		
		m_pParty->RemoveMember(m_uidLeader);
		
		CHECK_EQUAL(m_pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveMember_UnknownMemberFailure)
	{
		CHECK_EQUAL(m_pParty->GetCount(), 1);
		
		MUID uidMember = ZTestHelper::NewUID();
		m_pParty->RemoveMember(uidMember);
		
		CHECK_EQUAL(m_pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestParty_ChangeLeader)	
	{
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);
		CHECK_EQUAL(m_pParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(m_pParty->GetCount(), 2);

		m_pParty->ChangeLeader(uidMember);
		CHECK_EQUAL(m_pParty->GetLeader(), uidMember);
		CHECK_EQUAL(m_pParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestParty_ChangeLeader_UnknownMemberFailure)
	{
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);
		CHECK_EQUAL(m_pParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(m_pParty->GetCount(), 2);		

		MUID uidUnknownMember = ZTestHelper::NewUID();
		m_pParty->ChangeLeader(uidUnknownMember);
		CHECK_EQUAL(m_pParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(m_pParty->GetCount(), 2);	
	}

	TEST_FIXTURE(Fixture, TestParty_UpdateMember)	
	{
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);

		TD_PARTY_MEMBER tdPartyMember;
		tdPartyMember.m_uidPlayer = uidMember;
		tdPartyMember.nStatusFlag = 0;
		tdPartyMember.nHP = 100;
		tdPartyMember.nEN = 100;
		tdPartyMember.nSTA = 100;
		wcsncpy_s(tdPartyMember.szName, strMemberName.c_str(), _TRUNCATE);

		vector<int> vecBuff;
		vecBuff.push_back(100);
		vecBuff.push_back(200);
		
		ZPartyMember partyMember;
		partyMember.Assign(tdPartyMember, vecBuff);
		
		ZPartyMember* pPartyMember = m_pParty->GetMember(uidMember);
		ASSERT_CHECK(pPartyMember != NULL);


		pPartyMember->Update(&partyMember);
		
		
		TD_PARTY_MEMBER tdUpdatedPartyMember;
		vector<int> vecUpdateBuff;

		pPartyMember->Export(&tdUpdatedPartyMember, &vecUpdateBuff);
		CHECK_EQUAL(tdUpdatedPartyMember.m_uidPlayer, tdPartyMember.m_uidPlayer);
		CHECK_EQUAL(tdUpdatedPartyMember.nHP, tdPartyMember.nHP);
		CHECK_EQUAL(tdUpdatedPartyMember.nEN, tdPartyMember.nEN);
		CHECK_EQUAL(tdUpdatedPartyMember.nSTA, tdPartyMember.nSTA);
		CHECK_EQUAL(tdUpdatedPartyMember.nStatusFlag, tdPartyMember.nStatusFlag);
		CHECK(0 == wcscmp(tdUpdatedPartyMember.szName, tdPartyMember.szName));
		
		CHECK(vecUpdateBuff == vecBuff);
	}

	TEST_FIXTURE(Fixture, TestParty_AddQuest_New)		
	{
		int nQuestID = 100;
		
		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);

		CHECK(!pPartyMember->IsExistQuest(nQuestID));
		

		pPartyMember->AddQuest(nQuestID);
		
	
		CHECK(pPartyMember->IsExistQuest(nQuestID));
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveQuest)	
	{
		// arrange
		int nQuestID1 = 100;
		int nQuestID2 = 101;
		
		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);

		pPartyMember->AddQuest(nQuestID1);
		pPartyMember->AddQuest(nQuestID2);
		CHECK(pPartyMember->IsExistQuest(nQuestID1));
		CHECK(pPartyMember->IsExistQuest(nQuestID2));


		// act
		pPartyMember->RemoveQuest(nQuestID1);


		// assert
		CHECK(!pPartyMember->IsExistQuest(nQuestID1));
		CHECK(pPartyMember->IsExistQuest(nQuestID2));
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveQuest_LastQuest)
	{
		// arrange
		int nQuestID = 100;
		
		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);

		pPartyMember->AddQuest(nQuestID);
		

		// act
		pPartyMember->RemoveQuest(nQuestID);


		// assert
		CHECK(pPartyMember->IsEmptyQuest());
	}

	TEST_FIXTURE(Fixture, TestParty_AddField_MakeNew)		
	{
		// arrange
		int nFieldGroupID = 100;
		MUID uidFieldGroup = ZTestHelper::NewUID();		

		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);
		
		
		// act
		pPartyMember->SetFieldGroup(nFieldGroupID, uidFieldGroup);

		
		// assert	
		CHECK_EQUAL(m_pParty->GetFieldGroupUID(nFieldGroupID), uidFieldGroup);
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveField)		
	{
		// arrange
		int nFieldGroupID = 100;
		MUID uidFieldGroup = ZTestHelper::NewUID();

		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);
		
		pPartyMember->SetFieldGroup(nFieldGroupID, uidFieldGroup);
		

		// act
		pPartyMember->ClearFieldGroup();
		

		// assert
		CHECK_EQUAL(m_pParty->GetFieldGroupUID(nFieldGroupID), MUID::Invalid());
	}

	TEST_FIXTURE(Fixture, TestParty_IsMember)
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		CHECK(!m_pParty->IsExistMember(uidMember));

		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);


		// act/assert
		CHECK(m_pParty->IsExistMember(uidMember));
	}

	TEST_FIXTURE(Fixture, TestParty_GetAloneMember)	
	{
		// act
		MUID uidAlone = m_pParty->GetAloneMember();
		
		// assert
		CHECK_EQUAL(uidAlone, m_uidLeader);
	}

	TEST_FIXTURE(Fixture, TestParty_GetAloneMember_NotAlone)	
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);

		// act
		MUID uidAlone = m_pParty->GetAloneMember();

		// assert
		CHECK_EQUAL(uidAlone, MUID::Invalid());
	}

	TEST_FIXTURE(Fixture, TestParty_FindCandidateForLeader)	
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);
		
		// act
		MUID uidCandidate = m_pParty->FindCandidateForLeader();

		// assert
		CHECK_EQUAL(uidCandidate, uidMember);
	}

	TEST_FIXTURE(Fixture, TestParty_FindCandidateForLeader_OnlyLeader)	
	{
		// act
		MUID uidCandidate = m_pParty->FindCandidateForLeader();

		// assert
		CHECK_EQUAL(uidCandidate, MUID::Invalid());
	}

	TEST_FIXTURE(Fixture, TestParty_AddOfflineMember)	
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);
		

		// act
		ZPartyMember* pPartyMember = m_pParty->GetMember(uidMember);
		ASSERT_CHECK(pPartyMember != NULL);
		
		pPartyMember->SetOffline(true);


		// assert
		CHECK_EQUAL(m_pParty->GetCount(), 2);
		CHECK_EQUAL(m_pParty->GetOnlineMemberCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestParty_RemoveOfflineMember)	
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);

		ZPartyMember* pPartyMember = m_pParty->GetMember(uidMember);
		ASSERT_CHECK(pPartyMember != NULL);
		
		pPartyMember->SetOffline(true);


		// act
		pPartyMember->SetOffline(false);


		// assert
		CHECK_EQUAL(m_pParty->GetCount(), 2);
		CHECK_EQUAL(m_pParty->GetOnlineMemberCount(), 2);
	}

	// ZPartyManager /////////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestPartyManager_AddParty)	
	{		
		CHECK_EQUAL(m_partyManager.GetCount(), 0);

		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		ZPartyMember* pPartyMember = pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);

		CHECK_EQUAL(pParty->GetUID(), m_uidParty);
		CHECK_EQUAL(pParty->GetLeader(), m_uidLeader);		
		CHECK_EQUAL(pParty->GetCount(), 1);
		CHECK(pPartyMember->GetName() == m_strLeaderName);

		CHECK_EQUAL(m_partyManager.GetCount(), 1);		
	}

	TEST_FIXTURE(Fixture, TestPartyManager_AddParty_AlreadyAddedPartyFailure)
	{
		CHECK_EQUAL(m_partyManager.GetCount(), 0);
		m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		CHECK_EQUAL(m_partyManager.GetCount(), 1);

		m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		CHECK_EQUAL(m_partyManager.GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_RemoveParty)	
	{
		m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		CHECK_EQUAL(m_partyManager.GetCount(), 1);

		m_partyManager.RemoveParty(m_uidParty);
		CHECK_EQUAL(m_partyManager.GetCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_RemoveParty_UnknownPartyFailure)
	{
		m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		CHECK_EQUAL(m_partyManager.GetCount(), 1);

		MUID uidUnknownParty = ZTestHelper::NewUID();
		m_partyManager.RemoveParty(uidUnknownParty);
		CHECK_EQUAL(m_partyManager.GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_AddMember)
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 1);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_partyManager.AddMember(m_uidParty, uidMember, strMemberName, 1, nMemberCID);
		CHECK_EQUAL(pParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_AddMember_AlreadyAddedMemberFailure)
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 1);

		m_partyManager.AddMember(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		CHECK_EQUAL(pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_RemoveMember)	
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_partyManager.AddMember(m_uidParty, uidMember, strMemberName, 1, nMemberCID);
		CHECK_EQUAL(pParty->GetCount(), 2);

		m_partyManager.RemoveMember(m_uidParty, uidMember);
		CHECK_EQUAL(pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestPartyManger_RemoveMember_RemoveLeaderFailure)
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 1);

		m_partyManager.RemoveMember(m_uidParty, m_uidLeader);
		CHECK_EQUAL(pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_RemoveMember_UnknownMemberFailure)
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 1);

		MUID uidMember = ZTestHelper::NewUID();
		m_partyManager.RemoveMember(m_uidParty, uidMember);
		CHECK_EQUAL(pParty->GetCount(), 1);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_ChangeLeader)	
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_partyManager.AddMember(m_uidParty, uidMember, strMemberName, 1, nMemberCID);

		CHECK_EQUAL(pParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(pParty->GetCount(), 2);

		m_partyManager.ChangeLeader(m_uidParty, uidMember);
		CHECK_EQUAL(pParty->GetLeader(), uidMember);
		CHECK_EQUAL(pParty->GetCount(), 2);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_ChangeLeader_UnknownMemberFailure)
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_partyManager.AddMember(m_uidParty, uidMember, strMemberName, 1, nMemberCID);
		CHECK_EQUAL(pParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(pParty->GetCount(), 2);

		MUID uidUnknownMember = ZTestHelper::NewUID();
		m_partyManager.ChangeLeader(m_uidParty, uidUnknownMember);
		CHECK_EQUAL(pParty->GetLeader(), m_uidLeader);
		CHECK_EQUAL(pParty->GetCount(), 2);		
	}

	TEST_FIXTURE(Fixture, TestPartyManager_Find)	
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		ZParty* pFoundParty = m_partyManager.Find(m_uidParty);
		ASSERT_CHECK(pFoundParty != NULL);

		CHECK_EQUAL(pParty, pFoundParty);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_Find_UnknownPartyFailure)
	{
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidUnknownParty = ZTestHelper::NewUID();
		ZParty* pFoundParty = m_partyManager.Find(uidUnknownParty);
		CHECK(pFoundParty == NULL);
	}

	TEST_FIXTURE(Fixture, TestPartyManager_Clear)	
	{
		CHECK_EQUAL(m_partyManager.GetCount(), 0);
		ZParty* pParty = m_partyManager.AddParty(m_uidParty, m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		CHECK_EQUAL(m_partyManager.GetCount(), 1);

		m_partyManager.Clear();
		CHECK_EQUAL(m_partyManager.GetCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestAddMoveServer)	
	{
		// arrange
		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);
		
		CHECK(!pPartyMember->IsMoveServer());


		// act
		pPartyMember->SetMoveServer(true);


		// assert
		CHECK(pPartyMember->IsMoveServer());
	}

	TEST_FIXTURE(Fixture, TestRemoveMoveServer)	
	{
		// arrange
		ZPartyMember* pPartyMember = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyMember != NULL);

		pPartyMember->SetMoveServer(true);
		CHECK(pPartyMember->IsMoveServer());

		
		// act
		pPartyMember->SetMoveServer(false);


		// assert
		CHECK(!pPartyMember->IsMoveServer());
	}

	TEST_FIXTURE(Fixture, TestChangeMemberUID)		
	{
		// arrange
		MUID uidNew = ZTestHelper::NewUID();

		// act
		m_pParty->ChangeMemberUID(m_uidLeader, uidNew);

		// assert
		CHECK_EQUAL(m_pParty->GetCount(), 1);
		CHECK(!m_pParty->IsExistMember(m_uidLeader));
		CHECK(m_pParty->IsExistMember(uidNew));
		CHECK_EQUAL(m_pParty->GetLeader(), uidNew);
	}

	TEST_FIXTURE(Fixture, TestIsSyncReady_NotReady)
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);

		// act/assert
		CHECK(!m_pParty->IsSyncReady());	
	}

	TEST_FIXTURE(Fixture, TestIsSyncReady_Readed)
	{
		// arrange
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		m_pParty->AddMember(uidMember, strMemberName, 1, nMemberCID);

		ZPartyMember partyLeader(m_uidLeader, m_strLeaderName, 1, m_nLeaderCID);
		ZPartyMember* pPartyLeader = m_pParty->GetMember(m_uidLeader);
		ASSERT_CHECK(pPartyLeader != NULL);
		pPartyLeader->Update(&partyLeader);

		ZPartyMember partyMember(uidMember, strMemberName, 1, nMemberCID);
		ZPartyMember* pPartyMember = m_pParty->GetMember(uidMember);
		ASSERT_CHECK(pPartyMember != NULL);
		pPartyMember->Update(&partyMember);

		// act/assert
		CHECK(m_pParty->IsSyncReady());	
	}
}
