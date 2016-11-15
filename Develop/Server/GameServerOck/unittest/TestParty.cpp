#include "stdafx.h"
#include "GParty.h"
#include "GPartySystem.h"
#include "MockEntityPlayer.h"
#include "MockPlayerObject.h"
#include "GConst.h"
#include "SUnitTestUtil.h"
#include "MockDBManager.h"
#include "GGlobal.h"
#include "GTestWrapper.h"
#include "GFieldSystem.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GPlayerObjectManager.h"
#include "GFieldMgr.h"
#include "GFieldInfoMgr.h"
#include "GPlayerField.h"
#include "MockPartySystem.h"
#include "GUTHelper_Duel.h"
#include "GPlayerBattleArena.h"

SUITE(Party)
{
	struct Fixture
	{
		Fixture(): party(MUID(1000, 0))
		{ 
			m_pPartySystem = m_PartySystemWrapper.Get();
			m_pPartyManager = m_pPartySystem->GetPartyManager();

			uidPlayer1.Value = 101;
			uidPlayer2.Value = 102;
			uidPlayer3.Value = 103;
			pPlayerObject1 = CreateTestPlayerObject(uidPlayer1);
			pPlayerObject2 = CreateTestPlayerObject(uidPlayer2);
			pPlayerObject3 = CreateTestPlayerObject(uidPlayer3);
			pPlayer1 = pPlayerObject1->GetMockEntity();
			pPlayer2 = pPlayerObject2->GetMockEntity();
			pPlayer3 = pPlayerObject3->GetMockEntity();			
			
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObject1);
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObject2);
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObject3);

			m_pPartyManager->Clear();
		}

		~Fixture() 
		{
			gmgr.pPlayerObjectManager->DeletePlayer(pPlayerObject3->GetUID());
			gmgr.pPlayerObjectManager->DeletePlayer(pPlayerObject2->GetUID());
			gmgr.pPlayerObjectManager->DeletePlayer(pPlayerObject1->GetUID());
		}

		MockPlayerObject* CreateTestPlayerObject(MUID& uid)
		{
			MockPlayerObject* p = new MockPlayerObject(uid);
			p->Create();
			return p;
		}

		void SetPlayerName__Player1_And_Player2_IsSame()
		{
			const wchar_t* pszPlayer1Name = L"pPlayer1";
			const wchar_t* pszPlayer3Name = L"OtherPlayer3";
			wcscpy_s(pPlayer1->GetPlayerInfo()->szName, pszPlayer1Name);
			wcscpy_s(pPlayer2->GetPlayerInfo()->szName, pszPlayer1Name);	// Reconnected Player #1
			wcscpy_s(pPlayer3->GetPlayerInfo()->szName, pszPlayer3Name);
		}
		
		GUTHelper		m_Helper;
		GParty party;
		GTestSysWrapper2<GPartySystem, MockPartySystem>		m_PartySystemWrapper;
		MockPartySystem*	m_pPartySystem;
		GPartyManager*		m_pPartyManager;

		MockEntityPlayer* pPlayer1; 
		MockEntityPlayer* pPlayer2;
		MockEntityPlayer* pPlayer3;
		MockPlayerObject* pPlayerObject1;
		MockPlayerObject* pPlayerObject2;
		MockPlayerObject* pPlayerObject3;
		MUID	uidPlayer1;
		MUID	uidPlayer2;
		MUID	uidPlayer3;
	};

	TEST_FIXTURE(Fixture, NewParty_CheckUID)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		GParty party(uidParty);

		CHECK_EQUAL(party.GetUID(), uidParty);
	}

	TEST_FIXTURE(Fixture, AddPartyMemberOverLimit)
	{	
		// arrange
		MUID uidMember;
		wstring strMemberName = L"TestMemberName";
		int nMemberCID;

		for(int i = 0; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			CHECK(!party.IsFull());

			uidMember = SUnitTestUtil::NewUID();
			nMemberCID = SUnitTestUtil::NewCID();
			party.AddMember(uidMember, strMemberName, nMemberCID);
		}

		CHECK(party.IsFull());
		CHECK_EQUAL(party.GetMemberCount(), MAX_PARTY_MEMBER_COUNT);


		// act
		CHECK(!party.AddMember(uidMember, strMemberName, nMemberCID));


		// assert
		CHECK(party.IsFull());
		CHECK_EQUAL(party.GetMemberCount(), MAX_PARTY_MEMBER_COUNT);
	}

	TEST_FIXTURE(Fixture, AddSamePartyMember)
	{
		CHECK(party.AddMember(uidPlayer1, pPlayer1->GetName(), pPlayer1->GetCID()));
		CHECK(party.IsExistMember(uidPlayer1));
		CHECK(!party.AddMember(uidPlayer1, pPlayer1->GetName(), pPlayer1->GetCID()));
		CHECK_EQUAL(party.GetMemberCount(), 1);		
	}

	TEST_FIXTURE(Fixture, DeletePartyMember)
	{	
		CHECK(party.AddMember(uidPlayer1, pPlayer1->GetName(), pPlayer1->GetCID()));
		CHECK_EQUAL(party.GetMemberCount(), 1);
		
		party.RemoveMember(uidPlayer1);
		CHECK_EQUAL(party.GetMemberCount(), 0);
	}

	TEST_FIXTURE(Fixture, DeleteNoPartyMember)
	{	
		CHECK(party.AddMember(uidPlayer1, pPlayer1->GetName(), pPlayer1->GetCID()));
		CHECK_EQUAL(party.GetMemberCount(), 1);

		party.RemoveMember(uidPlayer2);	
		CHECK_EQUAL(party.GetMemberCount(), 1);
	}

	TEST_FIXTURE(Fixture, CreatePartyAndSetToPlayer)
	{	
		MUID uidParty = pPlayer1->GetPartyUID();
		CHECK_EQUAL(uidParty.Value, MUID::Invalid().Value);
		
		MUID uidNewParty = SUnitTestUtil::NewUID();
		GParty* pParty = m_pPartyManager->NewParty(uidNewParty);
		CHECK(pParty != NULL);
		if (!pParty)	return;
		
		CHECK(pParty->GetUID().Value != MUID::Invalid().Value);

		pPlayer1->SetParty(pParty->GetUID());

		uidParty = pPlayer1->GetPartyUID();
		CHECK_EQUAL(uidParty.Value, pParty->GetUID().Value);

		delete pParty;
	}
	
	TEST_FIXTURE(Fixture, TestChangeMemberUID)
	{
		// arrange
		MUID uidMember = SUnitTestUtil::NewUID();		
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		party.AddMember(uidMember, strMemberName, nMemberCID);
		party.SetLeader(uidMember);

		MUID uidNewMember = SUnitTestUtil::NewUID();

		
		// act		
		party.ChangeMemberUID(uidMember, uidNewMember);


		// assert
		CHECK(!party.IsExistMember(uidMember));
		CHECK_EQUAL(party.GetLeader(), uidNewMember);
	}

	TEST_FIXTURE(Fixture, IsSamePartyMember)
	{
		party.AddMember(uidPlayer1, pPlayer1->GetName(), pPlayer1->GetCID());
		party.AddMember(uidPlayer2, pPlayer2->GetName(), pPlayer2->GetCID());
		m_pPartySystem->IsSamePartyMember(pPlayer1, pPlayer2);
	}
}
