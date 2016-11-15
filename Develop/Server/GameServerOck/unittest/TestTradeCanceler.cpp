#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeCanceler.h"
#include "GPlayerTrade.h"
#include "GTradeSystem.h"
#include "GTradeChecker.h"
#include "GPlayerDoing.h"

SUITE(TradeCanceler)
{
	struct Fixture 
	{
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pReqPlayer = test.player.NewPlayer(m_pField);
			m_pTarPlayer = test.player.NewPlayer(m_pField);

			m_pReqLink = test.network.NewLink(m_pReqPlayer);
			m_pTarLink = test.network.NewLink(m_pTarPlayer);

			test.trade.SetTradeState(m_pReqPlayer, m_pTarPlayer);

			m_pReqLink->AddIgnoreID(MC_MSG_RES);
			m_pTarLink->AddIgnoreID(MC_MSG_RES);
			m_pReqLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		MockLink*		m_pReqLink;
		MockLink*		m_pTarLink;

		GTradeCanceler	m_TradeCanceler;
	};

	TEST_FIXTURE(Fixture, Cancel)
	{
		CHECK_EQUAL(true, gsys.pTradeSystem->GetCanceler().Cancel(m_pReqPlayer));
		CHECK_EQUAL(false, gsys.pTradeSystem->GetChecker().CheckTrader(m_pReqPlayer, m_pTarPlayer));
		CHECK_EQUAL(false, m_pReqPlayer->GetDoing().IsNowTrading());
		CHECK_EQUAL(false, m_pTarPlayer->GetDoing().IsNowTrading());

		ASSERT_EQUAL(1, m_pReqLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_CANCEL, m_pReqLink->GetCommand(0).GetID());

		ASSERT_EQUAL(1, m_pTarLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_CANCEL, m_pTarLink->GetCommand(0).GetID());
	}
}