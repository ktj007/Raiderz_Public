#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeItemPutUper.h"
#include "GPlayerTrade.h"
#include "GTradeSystem.h"
#include "GTradeConfirmer.h"

SUITE(TradeItemPutUper)
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

			m_pReqLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}

		bool IsEqual(GItem* pItem, const TD_TRADE_ITEM_PUTUP& tdItemPutUp)
		{
			if (NULL == pItem) return false;

			if (tdItemPutUp.m_nSlotID != m_pItem->m_nSlotID) return false;
			if (tdItemPutUp.m_nID != m_pItem->m_pItemData->m_nID) return false;
			if (tdItemPutUp.m_nAmount != m_pItem->GetAmount()) return false;
			if (tdItemPutUp.m_nDurability != m_pItem->m_nDurability) return false;
			if (tdItemPutUp.m_nColor != m_pItem->m_nDyedColor) return false;

			return true;
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		MockLink*		m_pReqLink;
		MockLink*		m_pTarLink;

		GItem*			m_pItem;

		GTradeItemPutUper	m_TradeItemPutUper;
	};

	// 거래창이 꽉찬경우
	TEST_FIXTURE(Fixture, CheckTradeSpace)
	{
		test.trade.FullTradeItem(m_pReqPlayer);
		
		CHECK_EQUAL(false, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));
	}
	
	// 아이템 수량이 잘못된 경우
	TEST_FIXTURE(Fixture, CheckItemAmount)
	{
		int nAmount;

		nAmount = 0;
		CHECK_EQUAL(false, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, nAmount));

		nAmount = m_pItem->GetAmount() + 1;
		CHECK_EQUAL(false, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, nAmount));
	}

	// 거래할 수 없는 아이템
	TEST_FIXTURE(Fixture, CheckTradableItem)
	{
		// 귀속
		m_pItem->m_bBind = true;
		CHECK_EQUAL(false, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));

		// 장착
		GItem* pWeaponItem = test.item.EquipNewWeapon(m_pReqPlayer, WEAPON_1H_SLASH);
		CHECK_EQUAL(false, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, pWeaponItem->m_nSlotID, pWeaponItem->GetAmount()));
	}

	// 유니크 아이템
	TEST_FIXTURE(Fixture, CheckUnique)
	{
		test.item.GiveNewItem(m_pTarPlayer, m_pItem->m_pItemData->m_nID, 1);
		m_pItem->m_pItemData->m_bUnique = true;

		CHECK_EQUAL(false, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));
		CHECK(false == m_pReqPlayer->GetPlayerTrade().IsExistItemBySlotID(m_pItem->m_nSlotID));
	}

	// 아이템을 올리면 확인 해제
	TEST_FIXTURE(Fixture, PutUp_ConfirmRelease)
	{
		gsys.pTradeSystem->GetConfirmer().Confirm(m_pReqPlayer, m_pTarPlayer);

		CHECK_EQUAL(true, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));
		CHECK(false == m_pReqPlayer->GetPlayerTrade().IsConfirm());
		CHECK(false == m_pTarPlayer->GetPlayerTrade().IsConfirm());
	}

	TEST_FIXTURE(Fixture, PutUp)
	{
		CHECK_EQUAL(true, m_TradeItemPutUper.PutUp(m_pReqPlayer, m_pTarPlayer, m_pItem->m_nSlotID, m_pItem->GetAmount()));
		CHECK_EQUAL(m_pItem->GetAmount(), m_pReqPlayer->GetPlayerTrade().GetItemAmount(m_pItem->m_nSlotID));

		ASSERT_EQUAL(1, m_pReqLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_PUTUP_ITEM, m_pReqLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqLink->GetUID(), m_pReqLink->GetParam<MUID>(0, 0));
		{
			TD_TRADE_ITEM_PUTUP tdItemPutUp;
			ASSERT_CHECK(m_pReqLink->GetCommand(0).GetSingleBlob(tdItemPutUp, 1));
			CHECK_EQUAL(true, IsEqual(m_pItem, tdItemPutUp));
		}

		ASSERT_EQUAL(1, m_pTarLink->GetCommandCount());
		ASSERT_EQUAL(MC_TRADE_PUTUP_ITEM, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pReqLink->GetUID(), m_pTarLink->GetParam<MUID>(0, 0));
		{
			TD_TRADE_ITEM_PUTUP tdItemPutUp;
			ASSERT_CHECK(m_pTarLink->GetCommand(0).GetSingleBlob(tdItemPutUp, 1));
			CHECK_EQUAL(true, IsEqual(m_pItem, tdItemPutUp));
		}		
	}
}