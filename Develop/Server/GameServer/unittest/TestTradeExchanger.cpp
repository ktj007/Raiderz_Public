#include "stdafx.h"
#include "GTestForward.h"
#include "GTradeExchanger.h"
#include "GPlayerTrade.h"
#include "GItemHolder.h"
#include "GDBTaskTrade.h"
#include "GPlayerDoing.h"

SUITE(TradeExchanger)
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

			m_pItem = test.item.GiveNewItem(m_pReqPlayer, INVALID_ID, 20);
			m_nItemID = m_pItem->m_pItemData->m_nID;

			test.trade.SetTradeState(m_pReqPlayer, m_pTarPlayer);
			test.trade.SetConfirmState(m_pReqPlayer, m_pTarPlayer);

			m_pReqLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			m_pReqLink->AddIgnoreID(MC_MSG_RES);
			m_pTarLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			m_pTarLink->AddIgnoreID(MC_MSG_RES);

			m_pReqLink->ResetCommands();
			m_pTarLink->ResetCommands();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pReqPlayer;
		GEntityPlayer*	m_pTarPlayer;
		MockLink*		m_pReqLink;
		MockLink*		m_pTarLink;

		GItem*			m_pItem;
		int				m_nItemID;

		GTradeExchanger	m_TradeExchanger;
	};

	// 사용시간만큼 다 사용 한경우(접속했을때만 시간 지남)
	TEST_FIXTURE(Fixture, Exchange_InvalidUsagePeriod)
	{		
		m_pReqPlayer->GetPlayerInfo()->nPlayTimeSec = 3000;
		m_pItem->m_UsagePeriod.Set(true, true, 3001 + 1, 0);
		test.trade.AddTradeItem(m_pReqPlayer, m_pItem);
		
		CHECK_EQUAL(false, m_TradeExchanger.Exchange(m_pReqPlayer, m_pTarPlayer));

		CHECK_EQUAL(0, m_pTarPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, m_nItemID));
	}

	// 만료 시간이 된 경우(접속하지 않아도 시간 지남)
	TEST_FIXTURE(Fixture, Exchange_InvalidExpirationPeriod)
	{
		m_pReqPlayer->GetPlayerInfo()->nPlayTimeSec = 3000;
		m_pItem->m_ExpirationPeriod.Set(true, true, 3001, 0);
		test.trade.AddTradeItem(m_pReqPlayer, m_pItem);

		CHECK_EQUAL(false, m_TradeExchanger.Exchange(m_pReqPlayer, m_pTarPlayer));

		CHECK_EQUAL(0, m_pTarPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, m_nItemID));
	}

	// 인벤토리 공간이 모자름
	TEST_FIXTURE(Fixture, Exchange_NotEnoughInventory)
	{
		test.item.FullItemToInven(m_pTarPlayer, 1);
		GItem* pItem2 = test.item.GiveNewItem(m_pReqPlayer);
		test.trade.AddTradeItem(m_pReqPlayer, m_pItem);
		test.trade.AddTradeItem(m_pReqPlayer, pItem2);

		CHECK_EQUAL(false, m_TradeExchanger.Exchange(m_pReqPlayer, m_pTarPlayer));

		CHECK_EQUAL(0, m_pTarPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, m_nItemID));
		CHECK_EQUAL(0, m_pTarPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItem2->m_pItemData->m_nID));
	}

	// 아이템을 쪼개서 준 경우
	TEST_FIXTURE(Fixture, Exchange_Item_DividedQuantity)
	{		
		class MockDBManager : public GDBManager
		{
		public:
			virtual bool Trade(GTRADE_CHAR_VEC& vTradeChar, const uint8 nItemInstnaceCnt) override
			{
				GDBTaskTrade t;
				GDBT_TRADE_ITEM i;
				i.nCID		= vTradeChar[0].m_nCID;
				//i.nItemID	= m_nItemID;
				i.nSlotID	= vTradeChar[0].m_vItem[0].m_nSlotID;
				i.nToCID = vTradeChar[1].m_nCID;
				i.nToSlotID = vTradeChar[0].m_vItem[0].m_nSlotIDTo;
				i.nStackAmt = 10;

				t.GetData().vecDBInsertItemBySplit.push_back(i);
				t.Input(vTradeChar);
				t.SetTaskSuccess();
				t.OnPrePost();
				t.OnCompleted();
				
				return true;							
			}

			int m_nItemID;
		};
		GTestSysWrapper2<GDBManager, MockDBManager> dbManager;
		dbManager.Get()->m_nItemID = m_nItemID;

		int nP1GiveItemQuantity = 10;
		test.trade.AddTradeItem(m_pReqPlayer, m_pItem, nP1GiveItemQuantity);

		int nBeforeItemQuantity = m_pItem->GetAmount();
		CHECK_EQUAL(true, m_TradeExchanger.Exchange(m_pReqPlayer, m_pTarPlayer));
		CHECK_EQUAL(false, m_pReqPlayer->GetDoing().IsNowTrading());
		CHECK_EQUAL(false, m_pTarPlayer->GetDoing().IsNowTrading());

		vector<GItem*> vecP2InsertItem;
		m_pTarPlayer->GetItemHolder()->CollectItemByID(SLOTTYPE_INVENTORY, m_nItemID, vecP2InsertItem);
		ASSERT_EQUAL(1, vecP2InsertItem.size());
		CHECK_EQUAL(nBeforeItemQuantity - nP1GiveItemQuantity, m_pItem->GetAmount());
		CHECK_EQUAL(nP1GiveItemQuantity, vecP2InsertItem.front()->GetAmount());

		CHECK_EQUAL(3, m_pReqLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_ADD, m_pReqLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_ITEM_REMOVE, m_pReqLink->GetCommand(1).GetID());	
		CHECK_EQUAL(MC_TRADE_COMPLETE, m_pReqLink->GetCommand(2).GetID());	

		CHECK_EQUAL(3, m_pTarLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_ADD, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_ITEM_REMOVE, m_pTarLink->GetCommand(1).GetID());	
		CHECK_EQUAL(MC_TRADE_COMPLETE, m_pTarLink->GetCommand(2).GetID());
	}

	// 아이템을 전부 준 경우
	TEST_FIXTURE(Fixture, Exchange_Item_AllQuantity)
	{		
		test.trade.AddTradeItem(m_pReqPlayer, m_pItem);

		CHECK_EQUAL(true, m_TradeExchanger.Exchange(m_pReqPlayer, m_pTarPlayer));

		CHECK_EQUAL(false, m_pReqPlayer->GetDoing().IsNowTrading());
		CHECK_EQUAL(false, m_pTarPlayer->GetDoing().IsNowTrading());		
		CHECK_EQUAL(0, m_pReqPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, m_nItemID));

		vector<GItem*> vecP2InsertItem;
		m_pTarPlayer->GetItemHolder()->CollectItemByID(SLOTTYPE_INVENTORY, m_nItemID, vecP2InsertItem);		
		ASSERT_EQUAL(1, vecP2InsertItem.size());
		CHECK_EQUAL(m_pItem, vecP2InsertItem.front());

		CHECK_EQUAL(3, m_pReqLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_ADD, m_pReqLink->GetCommand(0).GetID());	
		CHECK_EQUAL(MC_ITEM_REMOVE, m_pReqLink->GetCommand(1).GetID());
		CHECK_EQUAL(MC_TRADE_COMPLETE, m_pReqLink->GetCommand(2).GetID());	

		vector<TD_ITEM_DECREASE> vecTDItemDecrease;
		ASSERT_CHECK(m_pReqLink->GetCommand(1).GetBlob(vecTDItemDecrease, 0));
		ASSERT_EQUAL(vecTDItemDecrease.size(), 1);
		CHECK_EQUAL(vecTDItemDecrease.front().m_nSlotID, 0);

		CHECK_EQUAL(3, m_pTarLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_ADD, m_pTarLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_ITEM_REMOVE, m_pTarLink->GetCommand(1).GetID());	
		CHECK_EQUAL(MC_TRADE_COMPLETE, m_pTarLink->GetCommand(2).GetID());				

		vector<TD_ITEM_INSERT> vecTDItemInsert;
		ASSERT_CHECK(m_pTarLink->GetCommand(0).GetBlob(vecTDItemInsert, 0));
		ASSERT_EQUAL(vecTDItemInsert.size(), 1);
	}

	// 돈
	TEST_FIXTURE(Fixture, Exchange_Money)
	{				
		m_pReqPlayer->GetPlayerInfo()->SetMoney(1000);
		m_pTarPlayer->GetPlayerInfo()->SetMoney(1000);

		int nP1GiveMoney = 200;
		int nP2GiveMoney = 300;
		m_pReqPlayer->GetPlayerTrade().SetMoney(nP1GiveMoney);
		m_pTarPlayer->GetPlayerTrade().SetMoney(nP2GiveMoney);

		int nP1BeforeMoney = m_pReqPlayer->GetPlayerInfo()->GetMoney();
		int nP2BeforeMoney = m_pTarPlayer->GetPlayerInfo()->GetMoney();

		CHECK_EQUAL(true, m_TradeExchanger.Exchange(m_pReqPlayer, m_pTarPlayer));

		CHECK_EQUAL(false, m_pReqPlayer->GetDoing().IsNowTrading());
		CHECK_EQUAL(false, m_pTarPlayer->GetDoing().IsNowTrading());

		CHECK_EQUAL(4, m_pReqLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_UPDATE_MONEY, m_pReqLink->GetCommand(0).GetID());	
		CHECK_EQUAL(MC_ITEM_ADD, m_pReqLink->GetCommand(1).GetID());	
		CHECK_EQUAL(MC_ITEM_REMOVE, m_pReqLink->GetCommand(2).GetID());	
		CHECK_EQUAL(MC_TRADE_COMPLETE, m_pReqLink->GetCommand(3).GetID());	

		CHECK_EQUAL(4, m_pTarLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_UPDATE_MONEY, m_pTarLink->GetCommand(0).GetID());				
		CHECK_EQUAL(MC_ITEM_ADD, m_pTarLink->GetCommand(1).GetID());
		CHECK_EQUAL(MC_ITEM_REMOVE, m_pTarLink->GetCommand(2).GetID());	
		CHECK_EQUAL(MC_TRADE_COMPLETE, m_pTarLink->GetCommand(3).GetID());				

		int nP1AfterMoney = m_pReqPlayer->GetPlayerInfo()->GetMoney();
		int nP2AfterMoney = m_pTarPlayer->GetPlayerInfo()->GetMoney();

		CHECK_EQUAL(nP1BeforeMoney - nP1GiveMoney + nP2GiveMoney, nP1AfterMoney);
		CHECK_EQUAL(nP2BeforeMoney - nP2GiveMoney + nP1GiveMoney, nP2AfterMoney);				
	}

}
