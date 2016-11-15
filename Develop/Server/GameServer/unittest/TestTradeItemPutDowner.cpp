#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeItemPutDowner.h"
#include "GPlayerTrade.h"
#include "GItemHolder.h"
#include "GTradeSystem.h"
#include "GTradeConfirmer.h"
#include "GTradeItemPutUper.h"

SUITE(TradeItemPutDowner)
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

			m_pItem = test.item.GiveNewItem(m_pReqPlayer, INVALID_ID, 10);
			test.trade.SetTradeState(m_pReqPlayer, m_pTarPlayer);
			gsys.pTradeSystem->GetItemPutUper().PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount());

			m_pReqLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}

		bool IsEqual(GItem* pItem, const TD_TRADE_ITEM_PUTDOWN& tdItemPutDown)
		{
			if (NULL == pItem) return false;

			if (tdItemPutDown.m_nSlotID != m_pItem->m_nSlotID) return false;
			if (tdItemPutDown.m_nAmount != m_pItem->GetAmount()) return false;

			return true;
		}


		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		MockLink*		m_pReqLink;
		MockLink*		m_pTarLink;

		GItem*			m_pItem;

		GTradeItemPutDowner	m_TradeItemPutDowner;
	};

	// 아이템 수량이 잘못된 경우
	TEST_FIXTURE(Fixture, CheckItemAmount)
	{
		int nAmount;

		nAmount = 0;
		CHECK_EQUAL(false, m_TradeItemPutDowner.PutDown(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, nAmount));

		nAmount = m_pItem->GetAmount() + 1;
		CHECK_EQUAL(false, m_TradeItemPutDowner.PutDown(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, nAmount));
	}

	// 거래창에 존재하지 않는 아이템을 내리려는 경우
	TEST_FIXTURE(Fixture, PutDown_NotExistItem)
	{
		int nEmptySlot = m_pReqPlayer->GetItemHolder()->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);

		CHECK_EQUAL(false, m_TradeItemPutDowner.PutDown(m_pReqPlayer, m_pTarPlayer, nEmptySlot, 2));
	}

	// 아이템을 내리면 확인 해제
	TEST_FIXTURE(Fixture, PutDown_ConfirmRelease)
	{
		gsys.pTradeSystem->GetConfirmer().Confirm(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(true, m_TradeItemPutDowner.PutDown(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));
		CHECK(false == m_pReqPlayer->GetPlayerTrade().IsConfirm());
		CHECK(false == m_pTarPlayer->GetPlayerTrade().IsConfirm());
	}

	TEST_FIXTURE(Fixture, PutDown)
	{
		CHECK_EQUAL(true, m_TradeItemPutDowner.PutDown(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));
		CHECK_EQUAL(0, m_pReqPlayer->GetPlayerTrade().GetItemAmount(m_pItem->m_nSlotID));

		ASSERT_EQUAL(1, m_pReqLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_PUTDOWN_ITEM, m_pReqLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqLink->GetUID(), m_pReqLink->GetParam<MUID>(0, 0));
		{
			TD_TRADE_ITEM_PUTDOWN tdItemPutDown;
			ASSERT_CHECK(m_pReqLink->GetCommand(0).GetSingleBlob(tdItemPutDown, 1));
			CHECK_EQUAL(true, IsEqual(m_pItem, tdItemPutDown));
		}

		ASSERT_EQUAL(1, m_pTarLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_PUTDOWN_ITEM, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqLink->GetUID(), m_pTarLink->GetParam<MUID>(0, 0));
		{
			TD_TRADE_ITEM_PUTDOWN tdItemPutDown;
			ASSERT_CHECK(m_pTarLink->GetCommand(0).GetSingleBlob(tdItemPutDown, 1));
			CHECK_EQUAL(true, IsEqual(m_pItem, tdItemPutDown));
		}		
	}
}

