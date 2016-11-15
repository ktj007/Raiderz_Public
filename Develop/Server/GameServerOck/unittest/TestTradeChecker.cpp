#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeChecker.h"
#include "GPlayerTrade.h"
#include "GTradeSystem.h"
#include "GTradeCanceler.h"

SUITE(TradeChecker)
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

		GTradeChecker	m_TradeChecker;
	};

	// 거래자가 아닌 경우
	TEST_FIXTURE(Fixture, CheckTrader)
	{
		gsys.pTradeSystem->GetCanceler().Cancel(m_pReqPlayer);

		CHECK_EQUAL(false, m_TradeChecker.CheckTrader(m_pReqPlayer, m_pTarPlayer));
	}

	// 거리가 먼 경우
	TEST_FIXTURE(Fixture, CheckDistance)
	{
		test.interaction.SetUninteractableDistacne(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(false, m_TradeChecker.CheckDistance(m_pReqPlayer, m_pTarPlayer));
	}

	// 적대 상태인 경우
	TEST_FIXTURE(Fixture, CheckEnemy)
	{
		test.duel.NewMockDuel_Fighting(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(false, m_TradeChecker.CheckEnemy(m_pReqPlayer, m_pTarPlayer));
	}

	// 잘못된 거래중인 경우 거래 취소
	TEST_FIXTURE(Fixture, CancelInvalidTrade)
	{
		test.duel.NewMockDuel_Fighting(m_pReqPlayer, m_pTarPlayer);
		m_pReqLink->ResetCommands();
		m_pTarLink->ResetCommands();

		CHECK_EQUAL(false, m_TradeChecker.CancelInvalidTrade(m_pReqPlayer, m_pTarPlayer));
		CHECK_EQUAL(false, gsys.pTradeSystem->GetChecker().CheckTrader(m_pReqPlayer, m_pTarPlayer));

		ASSERT_EQUAL(1, m_pReqLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_CANCEL, m_pReqLink->GetCommand(0).GetID());

		ASSERT_EQUAL(1, m_pTarLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_CANCEL, m_pTarLink->GetCommand(0).GetID());
	}
}
