#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeSilverPutUper.h"
#include "GPlayerTrade.h"
#include "GTradeSystem.h"
#include "GTradeConfirmer.h"

SUITE(TradeMoneyPutUper)
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

			m_pReqPlayer->GetPlayerInfo()->SetMoney(100);
			m_nPutUpMoney = m_pReqPlayer->GetPlayerInfo()->GetMoney();

			m_pReqLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}
		
		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		MockLink*		m_pReqLink;
		MockLink*		m_pTarLink;

		int m_nPutUpMoney;

		GTradeMoneyPutUper	m_TradeMoneyPutUper;
	};

	// 돈이 잘못된 경우
	TEST_FIXTURE(Fixture, CheckMoney)
	{
		m_nPutUpMoney = -1;
		CHECK_EQUAL(false, gsys.pTradeSystem->GetMoneyPutUper().PutUp(m_pReqPlayer, m_pTarPlayer, m_nPutUpMoney));

		m_nPutUpMoney = m_pReqPlayer->GetPlayerInfo()->GetMoney() + 1;
		CHECK_EQUAL(false, gsys.pTradeSystem->GetMoneyPutUper().PutUp(m_pReqPlayer, m_pTarPlayer, m_nPutUpMoney));
	}

	// 실버를 올리면 확인 해제
	TEST_FIXTURE(Fixture, PutUp_ConfirmRelease)
	{
		gsys.pTradeSystem->GetConfirmer().Confirm(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(true, gsys.pTradeSystem->GetMoneyPutUper().PutUp(m_pReqPlayer, m_pTarPlayer, m_nPutUpMoney));
		CHECK(false == m_pReqPlayer->GetPlayerTrade().IsConfirm());
		CHECK(false == m_pTarPlayer->GetPlayerTrade().IsConfirm());
	}

	TEST_FIXTURE(Fixture, PutUp)
	{
		CHECK_EQUAL(true, gsys.pTradeSystem->GetMoneyPutUper().PutUp(m_pReqPlayer, m_pTarPlayer, m_nPutUpMoney));
		CHECK_EQUAL(m_nPutUpMoney, m_pReqPlayer->GetPlayerTrade().GetMoney());

		ASSERT_EQUAL(1, m_pReqLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_PUTUP_MONEY, m_pReqLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqLink->GetUID(), m_pReqLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_nPutUpMoney, m_pReqLink->GetParam<int>(0, 1));

		ASSERT_EQUAL(1, m_pTarLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_PUTUP_MONEY, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqLink->GetUID(), m_pTarLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_nPutUpMoney, m_pTarLink->GetParam<int>(0, 1));
	}
}