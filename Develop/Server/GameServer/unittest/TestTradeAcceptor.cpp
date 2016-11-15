#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeAcceptor.h"
#include "GTradeSystem.h"
#include "GTradeChecker.h"

SUITE(TradeAcceptor)
{
	struct Fixture 
	{
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pAcceptPlayer = test.player.NewPlayer(m_pField);
			m_pTarPlayer = test.player.NewPlayer(m_pField);
			m_pOtherPlayer = test.player.NewPlayer(m_pField);

			m_AcceptLink = test.network.NewLink(m_pAcceptPlayer);
			m_pTarLink = test.network.NewLink(m_pTarPlayer);

			test.trade.SetRequestState(m_pAcceptPlayer, m_pTarPlayer);

			m_AcceptLink->AddIgnoreID(MC_MSG_RES);
			m_AcceptLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pAcceptPlayer;
		GEntityPlayer*	m_pTarPlayer;
		GEntityPlayer*	m_pOtherPlayer;
		MockLink*		m_AcceptLink;
		MockLink*		m_pTarLink;

		GTradeAcceptor	m_TradeAcceptor;
	};


	// 너무 멀리 떨어저 있는 경우
	TEST_FIXTURE(Fixture, AcceptStart_Distance)
	{
		test.interaction.SetUninteractableDistacne(m_pAcceptPlayer, m_pTarPlayer);

		CHECK_EQUAL(false, m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, true));
	}

	// 적대상태인경우
	TEST_FIXTURE(Fixture, AcceptStart_Enemy)
	{
		test.duel.NewMockDuel_Fighting(m_pAcceptPlayer, m_pTarPlayer);

		CHECK_EQUAL(false, m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, true));
	}

	// 요청자가 거래중인 경우
	TEST_FIXTURE(Fixture, AcceptStart_TargetIsTrading)
	{
		test.trade.SetTradeState(m_pOtherPlayer, m_pAcceptPlayer);

		CHECK_EQUAL(false, m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, true));
	}

	// 수락자가 거래중인 경우
	TEST_FIXTURE(Fixture, AcceptStart_AcceptorIsTrading)
	{	
		m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, true);
		CHECK_EQUAL(false, m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, true));
	}
	
	
	// 수락자가 거절한 경우
	TEST_FIXTURE(Fixture, AcceptStart_Reject)
	{
		CHECK_EQUAL(true, m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, false));
		CHECK(false == gsys.pTradeSystem->GetChecker().CheckTrader(m_pAcceptPlayer, m_pTarPlayer));

		CHECK_EQUAL(1, m_AcceptLink->GetCommandCount());
		CHECK_EQUAL(MC_TRADE_START_REJECT, m_AcceptLink->GetCommand(0).GetID());
	}

	TEST_FIXTURE(Fixture, AcceptStart)
	{
		CHECK_EQUAL(true, m_TradeAcceptor.Accept(m_pTarPlayer, m_pAcceptPlayer, true));
		CHECK(true == gsys.pTradeSystem->GetChecker().CheckTrader(m_pAcceptPlayer, m_pTarPlayer));

		CHECK_EQUAL(1, m_AcceptLink->GetCommandCount());
		CHECK_EQUAL(MC_TRADE_START, m_AcceptLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pTarPlayer->GetUID(), m_AcceptLink->GetParam<MUID>(0, 0));

		CHECK_EQUAL(1, m_pTarLink->GetCommandCount());
		CHECK_EQUAL(MC_TRADE_START, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pAcceptPlayer->GetUID(), m_pTarLink->GetParam<MUID>(0, 0));
	}
}