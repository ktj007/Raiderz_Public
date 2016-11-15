#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeConfirmer.h"
#include "GPlayerTrade.h"

SUITE(TradeConfirmer)
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

			m_pReqLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		MockLink*		m_pReqLink;
		MockLink*		m_pTarLink;

		GTradeConfirmer	m_TradeConfirmer;
	};

	TEST_FIXTURE(Fixture, Confirm)
	{
		CHECK_EQUAL(true, m_TradeConfirmer.Confirm(m_pReqPlayer, m_pTarPlayer));

		CHECK(true == m_pReqPlayer->GetPlayerTrade().IsConfirm());

		CHECK_EQUAL(1, m_pReqLink->GetCommandCount());
		CHECK_EQUAL(MC_TRADE_CONFIRM, m_pReqLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqPlayer->GetUID(), m_pReqLink->GetParam<MUID>(0, 0));

		CHECK_EQUAL(1, m_pTarLink->GetCommandCount());
		CHECK_EQUAL(MC_TRADE_CONFIRM, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqPlayer->GetUID(), m_pTarLink->GetParam<MUID>(0, 0));
	}
}
