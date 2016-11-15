#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GItemManager.h"
#include "GItemActableChecker.h"
#include "GTradeSystem.h"
#include "GTradeRequester.h"
#include "GTradeAcceptor.h"
#include "GTradeItemPutUper.h"
#include "GPlayerDoing.h"

SUITE(ItemActableChecker)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			pTrader = test.player.NewPlayer(m_pField);

			m_pEquipedItem = test.item.EquipNewItem(m_pPlayer, INVALID_ID, ITEMSLOT_BODY);
			m_pArrangedItem = test.item.EquipNewItem(m_pPlayer, INVALID_ID, ITEMSLOT_INN_BED);
			m_pArrangedItem->m_pItemData->m_ItemType = ITEMTYPE_HOUSING;
			m_pTradingItem = test.item.GiveNewItem(m_pPlayer);

			gsys.pTradeSystem->GetRequester().Request(m_pPlayer, pTrader);
			gsys.pTradeSystem->GetAcceptor().Accept(pTrader, m_pPlayer, true);
			gsys.pTradeSystem->GetItemPutUper().PutUp(m_pPlayer, pTrader, m_pTradingItem->m_nSlotID, m_pTradingItem->GetAmount());
		}

		DECLWRAPPER_ItemMgr;
		GItemActableChecker	m_ItemActableChecker;

		GItem* m_pEquipedItem;
		GItem* m_pArrangedItem;
		GItem* m_pTradingItem;

		GEntityPlayer* pTrader;
	};

	
	// 버릴수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsDropable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsDropable(m_pPlayer, m_pEquipedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsDropable(m_pPlayer, m_pArrangedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsDropable(m_pPlayer, m_pTradingItem));
	}

	// 팔수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsSellable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsSellable(m_pPlayer, m_pEquipedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsSellable(m_pPlayer, m_pArrangedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsSellable(m_pPlayer, m_pTradingItem));
	}

	// 거래 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsTradable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsTradable(m_pPlayer, m_pEquipedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsTradable(m_pPlayer, m_pArrangedItem));
	}

	// 착용 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsEquipable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsEquipable(m_pPlayer, m_pArrangedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsEquipable(m_pPlayer, m_pTradingItem));
	}

	// 착용 해재 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsUnequipable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsUnequipable(m_pPlayer, m_pTradingItem));
	}

	// 사용 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsUsable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsUsable(m_pPlayer, m_pEquipedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsUsable(m_pPlayer, m_pArrangedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsUsable(m_pPlayer, m_pTradingItem));
	}

	// 재료로 제작 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsCraftable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsCraftable(m_pPlayer, m_pArrangedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsCraftable(m_pPlayer, m_pTradingItem));
	}

	// 이동 시킬수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsMovable)
	{
		CHECK_EQUAL(false, m_ItemActableChecker.IsMovable(m_pPlayer, m_pEquipedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsMovable(m_pPlayer, m_pArrangedItem));
		CHECK_EQUAL(false, m_ItemActableChecker.IsMovable(m_pPlayer, m_pTradingItem));
	}

	// 염색 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsDyeable)
	{
		m_pPlayer->GetDoing().ChangeDoing(CD_NORMAL);
		CHECK_EQUAL(false, m_ItemActableChecker.IsDyeable(m_pPlayer, m_pTradingItem));
	}

	// 인챈트 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsEnchantable)
	{		
		m_pPlayer->GetDoing().ChangeDoing(CD_NORMAL);
		CHECK_EQUAL(false, m_ItemActableChecker.IsEnchantable(m_pPlayer, m_pTradingItem));
	}

	// 수리 할수 없는 아이템 상태
	TEST_FIXTURE(Fixture, IsRepairable)
	{		
		CHECK_EQUAL(false, m_ItemActableChecker.IsRepairable(m_pPlayer, m_pTradingItem));
	}
}