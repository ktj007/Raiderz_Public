#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeRequester.h"
#include "GInteractionSystem.h"
#include "GPlayerDoing.h"
#include "GPlayerTrade.h"

SUITE(TradeRequester)
{
	struct Fixture 
	{
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pReqPlayer = test.player.NewPlayer(m_pField);
			m_pTarPlayer = test.player.NewPlayer(m_pField);
			m_pOtherPlayer = test.player.NewPlayer(m_pField);

			m_pTarLink = test.network.NewLink(m_pTarPlayer);
			m_pTarLink->ResetCommands();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		GEntityPlayer*	m_pOtherPlayer;		
		MockLink*		m_pTarLink;

		GTradeRequester	m_TradeRequester;
	};

	// 요청자가 자기에게 요청하는 경우
	TEST_FIXTURE(Fixture, RequestStart_SelfRequest)
	{				
		CHECK_EQUAL(false, m_TradeRequester.Request(m_pReqPlayer, m_pReqPlayer));
	}

	// 너무 멀리 떨어저 있는 경우
	TEST_FIXTURE(Fixture, RequestStart_Distance)
	{
		test.interaction.SetUninteractableDistacne(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(false, m_TradeRequester.Request(m_pReqPlayer, m_pTarPlayer));
	}

	// 적대상태인경우
	TEST_FIXTURE(Fixture, RequestStart_Enemy)
	{
		test.duel.NewMockDuel_Fighting(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(false, m_TradeRequester.Request(m_pReqPlayer, m_pTarPlayer));
	}

	// 대상이 거래 중인 경우
	TEST_FIXTURE(Fixture, RequestStart_TargetIsTrading)
	{
		test.trade.SetTradeState(m_pTarPlayer, m_pOtherPlayer);

		CHECK_EQUAL(false, m_TradeRequester.Request(m_pReqPlayer, m_pTarPlayer));
	}	

	// 대상이 다른 사람과 듀얼중인 경우
	TEST_FIXTURE(Fixture, RequestStart_TargetIsDueling)
	{
		test.duel.NewMockDuel_Fighting(m_pTarPlayer, m_pOtherPlayer);

		CHECK_EQUAL(false, m_TradeRequester.Request(m_pReqPlayer, m_pTarPlayer));
	}
	
	// 대상이 다른행위 중인경우
	TEST_FIXTURE(Fixture, RequestStart_TartetIsBusy)
	{
		m_pTarPlayer->GetDoing().ChangeDoing(CD_INTERACTING);

		CHECK_EQUAL(false, m_TradeRequester.Request(m_pReqPlayer, m_pTarPlayer));
	}

	TEST_FIXTURE(Fixture, RequestStart)
	{
		CHECK_EQUAL(true, m_TradeRequester.Request(m_pReqPlayer, m_pTarPlayer));
		CHECK_EQUAL(true, m_pTarPlayer->GetPlayerTrade().IsTradeRequester(m_pReqPlayer->GetUID()));

		ASSERT_EQUAL(1, m_pTarLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_START_REQ_RELAY, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqPlayer->GetUID(), m_pTarLink->GetParam<MUID>(0, 0));
	}
}