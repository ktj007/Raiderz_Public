#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "MockLink.h"
#include "GEntityPlayer.h"
#include "GUTHelper_Field.h"
#include "GPlayerDoing.h"
#include "CCommandTable.h"
#include "GPlayerSit.h"

SUITE(Sit)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::NewMockField();
			m_pSitPlayer = m_Helper.NewEntityPlayer(m_pField);
			m_pNeighborPlayer = m_Helper.NewEntityPlayer(m_pField);
			m_pSitPlayerLink = GUTHelper::NewLink(m_pSitPlayer);
			m_pNeighborPlayerLink = GUTHelper::NewLink(m_pNeighborPlayer);
		}

		~Fixture()
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}

		GUTHelper			m_Helper;
		MockField*			m_pField;
		GEntityPlayer*		m_pSitPlayer;
		GEntityPlayer*		m_pNeighborPlayer;
		
		MockLink*			m_pSitPlayerLink;
		MockLink*			m_pNeighborPlayerLink;
	};

	namespace Sit
	{
		TEST_FIXTURE(Fixture, AlreadySitting)
		{
			m_pSitPlayer->GetSit().Sit();

			m_pSitPlayerLink->OnRecv(MC_ACTION_SIT_REQ, 0, NULL);

			CHECK_EQUAL(true, m_pSitPlayer->GetSit().IsNowSit());
			CHECK_EQUAL(0, m_pSitPlayerLink->GetCommandCount());
			CHECK_EQUAL(0, m_pNeighborPlayerLink->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, Sit)
		{
			m_pSitPlayerLink->OnRecv(MC_ACTION_SIT_REQ, 0, NULL);

			CHECK_EQUAL(true, m_pSitPlayer->GetSit().IsNowSit());
			CHECK_EQUAL(1, m_pSitPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_SIT, m_pSitPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pSitPlayer->GetUIID(), m_pSitPlayerLink->GetParam<UIID>(0, 0));

			CHECK_EQUAL(1, m_pNeighborPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_SIT, m_pNeighborPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pSitPlayer->GetUIID(), m_pNeighborPlayerLink->GetParam<UIID>(0, 0));
		}
	}

	namespace
	{
		TEST_FIXTURE(Fixture, NotSitting)
		{
			m_pSitPlayerLink->OnRecv(MC_ACTION_SIT_RISE_REQ, 0, NULL);

			CHECK_EQUAL(false, m_pSitPlayer->GetSit().IsNowSit());
			CHECK_EQUAL(0, m_pSitPlayerLink->GetCommandCount());
			CHECK_EQUAL(0, m_pNeighborPlayerLink->GetCommandCount());
		}

		TEST_FIXTURE(Fixture, SitRise)
		{
			m_pSitPlayer->GetSit().Sit();

			m_pSitPlayerLink->OnRecv(MC_ACTION_SIT_RISE_REQ, 0, NULL);

			CHECK_EQUAL(false, m_pSitPlayer->GetSit().IsNowSit());
			CHECK_EQUAL(1, m_pSitPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_SIT_RISE, m_pSitPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pSitPlayer->GetUIID(), m_pSitPlayerLink->GetParam<UIID>(0, 0));

			CHECK_EQUAL(1, m_pNeighborPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_SIT_RISE, m_pNeighborPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pSitPlayer->GetUIID(), m_pNeighborPlayerLink->GetParam<UIID>(0, 0));
		}
	}	

	TEST_FIXTURE(Fixture, HitWhenSitting)
	{		
		m_pSitPlayer->GetSit().Sit();

		GHitInfo hitInfo;
		hitInfo.pTalentInfo = m_Helper.NewTalentInfo();
		hitInfo.pAttacker = m_pNeighborPlayer;
		m_pSitPlayer->OnHit(hitInfo, false);

		CHECK_EQUAL(false, m_pSitPlayer->GetSit().IsNowSit());
	}
}

