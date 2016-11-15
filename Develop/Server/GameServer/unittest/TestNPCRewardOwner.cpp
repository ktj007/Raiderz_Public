#include "stdafx.h"
#include "FBaseGame.h"
#include "GNPCRewardOwner.h"
#include "GUnitTestUtil.h"
#include "GTestForward.h"
#include "GHateTable.h"



SUITE(NPCRewardOwner)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pNPC = NewNPC(m_pField);
			m_pMember = NewEntityPlayer(m_pField);
			m_pParty = test.party.CreateParty(m_pPlayer, m_pMember);
		}

		GEntityNPC*		m_pNPC;
		GEntityPlayer*	m_pMember;
		GParty*		   	m_pParty;
	};

	// 플레이어가 어그로 획득하면 소유권자 후보로 등록하고, 첫등록이면 소유권자로 선정
	TEST_FIXTURE(Fixture, OnAddHateEntry)
	{
		m_pNPC->GetNPCRewardOwner().OnAddHateEntry(m_pPlayer);
	
		CHECK_EQUAL(true, m_pNPC->GetNPCRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(m_pPlayer->GetCID(), m_pNPC->GetNPCRewardOwner().GetRewardOwnerCID());	
	}

	// 먼저 어그로를 획득한 플레이어가 계속 소유권자로 유지
	TEST_FIXTURE(Fixture, OnAddHateEntry_MaintainRewardOwner)
	{
		m_pNPC->GetNPCRewardOwner().OnAddHateEntry(m_pPlayer);
		m_pNPC->GetNPCRewardOwner().OnAddHateEntry(m_pMember);

		CHECK_EQUAL(m_pPlayer->GetCID(), m_pNPC->GetNPCRewardOwner().GetRewardOwnerCID());	
	}

	// 플레이어의 파티 멤버가 어그로 획득하면 소유권자 후보로 등록
	TEST_FIXTURE(Fixture, OnAddHateEntryMember)
	{
		m_pNPC->GetNPCRewardOwner().OnAddHateEntryMember(m_pMember);

		CHECK_EQUAL(true, m_pNPC->GetNPCRewardOwner().IsCandidate(m_pMember->GetCID()));
	}	

	// 파티원중 일부만 어그로가 제거되면 소유권자 후보 유지
	TEST_FIXTURE(Fixture, OnLostHateEntry_PartPartyMember)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->GetHateTable().DropPoint(m_pPlayer->GetUID());

		CHECK_EQUAL(true, m_pNPC->GetNPCRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(true, m_pNPC->GetNPCRewardOwner().IsCandidate(m_pMember->GetCID()));
	}

	// 파티원 모두의 어그로가 제거되면 소유권자 후보에서 제거
	TEST_FIXTURE(Fixture, OnLostHateEntry_AllPartyMember)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->GetHateTable().DropPoint(m_pPlayer->GetUID());
		m_pNPC->GetHateTable().DropPoint(m_pMember->GetUID());

		CHECK_EQUAL(false, m_pNPC->GetNPCRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, m_pNPC->GetNPCRewardOwner().IsCandidate(m_pMember->GetCID()));
	}

	// 소유권자가 제거 되면 다음 소유권자 후보가 소유권자가 된다.
	TEST_FIXTURE(Fixture, OnLostHateEntry_ChangeRewardOwner)
	{
		GEntityPlayer* pOther = NewEntityPlayer(m_pField);

		m_pNPC->GetHateTable().AddPoint_FoundEnemy(pOther);
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);

		m_pNPC->GetHateTable().DropPoint(pOther->GetUID());

		int nRewardOwner = m_pNPC->GetNPCRewardOwner().GetRewardOwnerCID();
		CHECK(	m_pPlayer->GetCID() == nRewardOwner ||
				m_pMember->GetCID() == nRewardOwner);
	}

	// NPC의 어그로 테이블이 초기화되면 소유권자 후보 목록도 초기화
	TEST_FIXTURE(Fixture, OnEmptyHateTable)
	{
		m_pNPC->GetNPCRewardOwner().OnAddHateEntry(m_pPlayer);

		m_pNPC->GetNPCRewardOwner().OnEmptyHateTable();

		CHECK_EQUAL(true, m_pNPC->GetNPCRewardOwner().IsEmpty());
	}	
}
