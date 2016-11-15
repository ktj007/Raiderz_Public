#include "stdafx.h"
#include "FBaseGame.h"
#include "GParty.h"
#include "GTestForward.h"
#include "GQPKillRewardOwner.h"
#include "GEntityPlayer.h"

SUITE(QPKillRewardOwner)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pVictim	= NewEntityPlayer(m_pField);
			m_pMember	= NewEntityPlayer(m_pField);
			m_pParty	= test.party.CreateParty(m_pPlayer, m_pMember);

			test.player.SetMockPlayerPVPArea_LocateAndTeamOne(m_pVictim);
			test.player.SetMockPlayerPVPArea_LocateAndTeamTwo(m_pPlayer);
			test.player.SetMockPlayerPVPArea_LocateAndTeamTwo(m_pMember);			
		}

		GEntityPlayer*	m_pVictim;
		GEntityPlayer*	m_pMember;
		GParty*			m_pParty;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;
	};


	TEST_FIXTURE(Fixture, OnDamage_NotLocateAndBelongTeam)
	{
		test.player.ResetMockPlayerPVPArea(m_pVictim);

		m_pVictim->GetQPKillRewardOwner().OnDamage(m_pVictim, m_pPlayer->GetUID(), 1);

		CHECK_EQUAL(false, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pMember->GetCID()));
	}

	TEST_FIXTURE(Fixture, OnDamage_FarDistanceMember)
	{
		vec3 vPos = m_pPlayer->GetPos();
		vec3 vFarDistancePos = vec3(vPos.x+GConst::QUESTPVP_REWARDOWNER_DISTANCE+1.0f, vPos.y, vPos.z);
		m_pMember->SetPos(vFarDistancePos);

		m_pVictim->GetQPKillRewardOwner().OnDamage(m_pVictim, m_pPlayer->GetUID(), 1);

		CHECK_EQUAL(true, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pMember->GetCID()));
	}

	TEST_FIXTURE(Fixture, OnDamage)
	{
		m_pVictim->GetQPKillRewardOwner().OnDamage(m_pVictim, m_pPlayer->GetUID(), 1);

		CHECK_EQUAL(true, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(true, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pMember->GetCID()));
	}

	TEST_FIXTURE(Fixture, ClearPastRewardOwner)
	{
		GEntityPlayer* pOther = test.player.NewPlayer(m_pField);
		test.player.SetMockPlayerPVPArea_LocateAndTeamTwo(pOther);

		m_pVictim->GetQPKillRewardOwner().OnDamage(m_pVictim, m_pPlayer->GetUID(), 1);

		gsys.pSystem->Sleep(GConst::QUESTPVP_REWARDOWNER_RELEASETIME + 0.1f);
		m_pVictim->GetQPKillRewardOwner().OnDamage(m_pVictim, pOther->GetUID(), 1);

		CHECK_EQUAL(false, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, m_pVictim->GetQPKillRewardOwner().IsCandidate(m_pMember->GetCID()));
	}	
}