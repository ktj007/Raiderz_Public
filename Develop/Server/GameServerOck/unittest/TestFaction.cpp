#include "stdafx.h"
#include "GTestForward.h"
#include "CSFactionCalculator.h"
#include "GFactionInfo.h"
#include "GPlayerFactions.h"
#include "GHateTable.h"
#include "GNPCRewarder.h"

SUITE(Faction)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC = m_Helper.NewEntityNPC(m_pField);
			m_pFactionInfo =  test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
			m_pFactionInfo2 = test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
		}

		virtual ~Fixture()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GEntityNPC*		m_pNPC;
		GFactionInfo*	m_pFactionInfo;
		GFactionInfo*	m_pFactionInfo2;
		DECLWRAPPER_NPCInfoMgr;
		DECLWRAPPER_FieldInfoMgr;
	};

	// NPC°¡ Á×¾úÀ»¶§ ÆÑ¼Ç Áõ°¨
	TEST_FIXTURE(Fixture, DieNPC)
	{		
		m_pNPC->GetNPCInfo()->m_nFactionGainID = m_pFactionInfo->m_nID;
		m_pNPC->GetNPCInfo()->m_nFactionGainQuantity = 10;

		m_pNPC->GetNPCInfo()->m_nFactionLossID = m_pFactionInfo2->m_nID;
		m_pNPC->GetNPCInfo()->m_nFactionLossQuantity = 10;

		uint16 nBeforeGainQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pNPC->GetNPCInfo()->m_nFactionGainID);
		uint16 nBeforeLossQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pNPC->GetNPCInfo()->m_nFactionLossID);

		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->GetNPCRewarder().RewardByDie(m_pNPC);

		CHECK_EQUAL(nBeforeGainQuantity + m_pNPC->GetNPCInfo()->m_nFactionGainQuantity, m_pPlayer->GetPlayerFactions().GetQuantity(m_pNPC->GetNPCInfo()->m_nFactionGainID));
		CHECK_EQUAL(nBeforeLossQuantity - m_pNPC->GetNPCInfo()->m_nFactionLossQuantity, m_pPlayer->GetPlayerFactions().GetQuantity(m_pNPC->GetNPCInfo()->m_nFactionLossID));
	}
}