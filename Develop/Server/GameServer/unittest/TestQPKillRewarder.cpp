#include "stdafx.h"
#include "FBaseGame.h"
#include "GParty.h"
#include "GTestForward.h"
#include "GQPKillRewardOwner.h"
#include "GQPKillRewarder.h"
#include "GNPCLoot.h"
#include "GDropList.h"

SUITE(QPKillRewardr)
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

			m_pVictim->GetQPKillRewardOwner().OnDamage(m_pVictim, m_pPlayer->GetUID(), 1);

			GLootInfo* pLootInfo = test.loot.NewLootInfo();
			LOOT_ITEM_INFO* pLootItemInfo = test.loot.NewLootItemInfo(pLootInfo);
			GNPCInfo* pNPCInfo = test.npc.NewNPCInfo();
			pNPCInfo->m_pLootInfo = pLootInfo;

			GFieldInfo* pFieldInfo = test.field.NewFieldInfo();
			pFieldInfo->m_nQPVPNPCID1 = pNPCInfo->nID;
			pFieldInfo->m_nQPVPNPCID2 = pNPCInfo->nID;

			m_pField->SetFieldInfo(pFieldInfo);
		}

		GEntityPlayer*	m_pVictim;
		GEntityPlayer*	m_pMember;
		GParty*			m_pParty;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;
	};

	TEST_FIXTURE(Fixture, RewardToEnemy_NotLocateAndBelongTeam)
	{
		test.player.ResetMockPlayerPVPArea(m_pVictim);

		CHECK_EQUAL((GEntityNPC*)NULL, m_pVictim->GetQPKillRewarder().Reward());
	}

	TEST_FIXTURE(Fixture, RewardToEnemy_EmptyRewardOwner)
	{
		m_pVictim->GetQPKillRewardOwner().Clear();

		CHECK_EQUAL((GEntityNPC*)NULL, m_pVictim->GetQPKillRewarder().Reward());
	}

	TEST_FIXTURE(Fixture, RewardToEnemy_NowDisableQuestPVPDrop)
	{
		GBuffInfo* pBuffInfo = test.buff.NewBuffInfo();
		pBuffInfo->m_nPassiveExtraAttrib = BUFPEA_DISABLE_QUESTPVP_DROP;
		test.buff.GainBuffSelf(m_pVictim, pBuffInfo);

		CHECK_EQUAL((GEntityNPC*)NULL, m_pVictim->GetQPKillRewarder().Reward());
	}

	TEST_FIXTURE(Fixture, Reward)
	{
		GEntityNPC* pNPC = m_pVictim->GetQPKillRewarder().Reward();
		ASSERT_CHECK(NULL != pNPC);

		CHECK_EQUAL(true, pNPC->IsDead());

		pNPC->GetNPCLoot().GetDropList().IsAuthorizedCID(m_pPlayer->GetCID());
		pNPC->GetNPCLoot().GetDropList().IsAuthorizedCID(m_pMember->GetCID());
	}
}
