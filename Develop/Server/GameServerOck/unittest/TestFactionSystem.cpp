#include "stdafx.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "GPlayerFactions.h"
#include "GFactionInfo.h"
#include "GFactionSystem.h"
#include "CSFactionCalculator.h"
#include "MockLink.h"
#include "CCommandTable.h"
#include "GDuel.h"
#include "GBattleArenaMgr.h"
#include "GMatchRule_SameTeamMember.h"
#include "GConst.h"
#include "GPlayerBattleArena.h"
#include "GEntityNPC.h"
#include "GEntityNPC.h"
#include "GTestForward.h"
#include "GHateTable.h"
#include "GConfig.h"


SUITE(FactionSystem)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pFactionInfo =  test.faction.NewFactionInfo(CSFactionCalculator::GetNormalMaxQuantity());
			GConfig::m_strAutoTestType = L"";
		}

		virtual ~Fixture()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GFactionInfo*	m_pFactionInfo;
		DECLWRAPPER_NPCInfoMgr;
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FactionInfoMgr;
		DECLWRAPPER_FactionRelationInfoMgr;
	};

	
	namespace Increase
	{
		// 이미 최대량인 경우
		TEST_FIXTURE(Fixture, CurrentQuantityIsMax)
		{
			m_pPlayer->GetPlayerFactions().Increase(m_pFactionInfo->m_nID, CSFactionCalculator::GetMaxQuantity() - m_pFactionInfo->m_nDefaultQuantity);

			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nIncreaseQuantity = 100;

			CHECK_EQUAL(true, gsys.pFactionSystem->Increase(m_pPlayer, m_pFactionInfo->m_nID, nIncreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity, nAfterFactionQuantity);
		}

		// 증가시키려는 량이 0인 경우
		TEST_FIXTURE(Fixture, IncreaseQuantityIsZero)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nIncreaseQuantity = 0;

			CHECK_EQUAL(true, gsys.pFactionSystem->Increase(m_pPlayer, m_pFactionInfo->m_nID, nIncreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity, nAfterFactionQuantity);
		}

		// 증가시키면 최대량보다 큰 경우
		TEST_FIXTURE(Fixture, BiggerThanMaxQuantity)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nIncreaseQuantity = CSFactionCalculator::GetMaxQuantity() - nBeforeFactionQuantity + 1;

			CHECK_EQUAL(true, gsys.pFactionSystem->Increase(m_pPlayer, m_pFactionInfo->m_nID, nIncreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(CSFactionCalculator::GetMaxQuantity(), nAfterFactionQuantity);
		}

		TEST_FIXTURE(Fixture, Normal)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nIncreaseQuantity = 100;

			CHECK_EQUAL(true, gsys.pFactionSystem->Increase(m_pPlayer, m_pFactionInfo->m_nID, nIncreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity + nIncreaseQuantity, nAfterFactionQuantity);
		}

		// 팩션의 수치의 퍼센트가 올라간 경우
		TEST_FIXTURE(Fixture, IncreaseFactionQuantityPercent)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nIncreaseQuantity = 1000;

			MockLink* pLink = m_Helper.NewLink(m_pPlayer);
			CHECK_EQUAL(true, gsys.pFactionSystem->Increase(m_pPlayer, m_pFactionInfo->m_nID, nIncreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity + nIncreaseQuantity, nAfterFactionQuantity);

			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_FACTION_INCREASE, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pFactionInfo->m_nID, pLink->GetParam<uint8>(0, 0));
			CHECK_EQUAL(m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID), pLink->GetParam<uint16>(0, 1));
		}
	}

	namespace Decrease
	{
		// 이미 최소량인 경우
		TEST_FIXTURE(Fixture, CurrentQuantityIsMin)
		{
			m_pPlayer->GetPlayerFactions().Decrease(m_pFactionInfo->m_nID, m_pFactionInfo->m_nDefaultQuantity);

			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nDecreaseQuantity = 100;

			CHECK_EQUAL(true, gsys.pFactionSystem->Decrease(m_pPlayer, m_pFactionInfo->m_nID, nDecreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity, nAfterFactionQuantity);
		}

		// 감소시키려는 량이 0인 경우
		TEST_FIXTURE(Fixture, DecreaseQuantityIsZero)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nDecreaseQuantity = 0;

			CHECK_EQUAL(true, gsys.pFactionSystem->Decrease(m_pPlayer, m_pFactionInfo->m_nID, nDecreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity, nAfterFactionQuantity);
		}

		// 감소시키면 최소량보다 작은 경우
		TEST_FIXTURE(Fixture, SmallerThanMinQuantity)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nDecreaseQuantity = nBeforeFactionQuantity - CSFactionCalculator::GetMinQuantity() + 1;

			CHECK_EQUAL(true, gsys.pFactionSystem->Decrease(m_pPlayer, m_pFactionInfo->m_nID, nDecreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(CSFactionCalculator::GetMinQuantity(), nAfterFactionQuantity);
		}

		TEST_FIXTURE(Fixture, Normal)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nDecreaseQuantity = 100;

			CHECK_EQUAL(true, gsys.pFactionSystem->Decrease(m_pPlayer, m_pFactionInfo->m_nID, nDecreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity - nDecreaseQuantity, nAfterFactionQuantity);
		}

		// 팩션의 수치의 퍼센트가 내려간 경우
		TEST_FIXTURE(Fixture, DecraseFactionQuantityPercent)
		{
			uint16 nBeforeFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			uint16 nDecreaseQuantity = 1000;

			MockLink* pLink = m_Helper.NewLink(m_pPlayer);
			CHECK_EQUAL(true, gsys.pFactionSystem->Decrease(m_pPlayer, m_pFactionInfo->m_nID, nDecreaseQuantity));

			uint16 nAfterFactionQuantity = m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID);
			CHECK_EQUAL(nBeforeFactionQuantity - nDecreaseQuantity, nAfterFactionQuantity);

			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_FACTION_DECREASE, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pFactionInfo->m_nID, pLink->GetParam<uint8>(0, 0));
			CHECK_EQUAL(m_pPlayer->GetPlayerFactions().GetQuantity(m_pFactionInfo->m_nID), pLink->GetParam<uint16>(0, 1));
		}
	}	
}



