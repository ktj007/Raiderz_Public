#include "StdAfx.h"
#include "GTradeExchangerForDBTask.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBTaskDataTrade.h"
#include "GItemSystem.h"
#include "GEntityPlayer.h"
#include "GMoneySystem.h"
#include "GItemHolder.h"
#include "GQuestSystem.h"
#include "GPlayerTrade.h"
#include "GDBCacheSystem.h"
#include "GQObjUpdater.h"
#include "GPlayerDoing.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GCmdHandler_Trade.h"

void GTradeExchangerForDBTask::Exchange(GDBT_TRADE_DATA& data)
{
	GEntityPlayer* pPlayer1 = gmgr.pPlayerObjectManager->GetEntity(data.vTradeChar[0].m_uidPlayer);
	GEntityPlayer* pPlayer2 = gmgr.pPlayerObjectManager->GetEntity(data.vTradeChar[1].m_uidPlayer);
	VALID(pPlayer1);
	VALID(pPlayer2);
	
	UpdateMoney(pPlayer1, pPlayer2
		, data.vTradeChar[0].m_nMoney - pPlayer1->GetMoney()
		, data.vTradeChar[1].m_nMoney - pPlayer2->GetMoney());


	pPlayer1->GetPlayerTrade().Reset();
	pPlayer2->GetPlayerTrade().Reset();
	pPlayer1->GetDoing().ChangeDoing(CD_NORMAL);
	pPlayer2->GetDoing().ChangeDoing(CD_NORMAL);


	GDBT_TRADE_RESULT	Res1;
	GDBT_TRADE_RESULT	Res2;

	InsertItemBySplit(pPlayer1, pPlayer2, data.vecDBInsertItemBySplit, Res1);
	InsertItemBySplit(pPlayer2, pPlayer1, data.vecDBInsertItemBySplit, Res2);

	TradeServerItem(pPlayer1, pPlayer2, data.vTradeChar[0], Res1, Res2);
	TradeServerItem(pPlayer2, pPlayer1, data.vTradeChar[1], Res2, Res1);

	gsys.pItemSystem->GetAdder().Route(pPlayer1, Res1.vecAddItem);
	gsys.pItemSystem->GetAdder().Route(pPlayer2, Res2.vecAddItem);
	gsys.pItemSystem->GetRemover().Route(pPlayer1, Res1.vecRemoveItem);
	gsys.pItemSystem->GetRemover().Route(pPlayer2, Res2.vecRemoveItem);

	MCommand* pCmd1 = MakeNewCommand(MC_TRADE_COMPLETE, 0, NULL);		
	MCommand* pCmd2 = MakeNewCommand(MC_TRADE_COMPLETE, 0, NULL);
	pPlayer1->RouteToMe(pCmd1);		
	pPlayer2->RouteToMe(pCmd2);
	

	pPlayer1->GetNPCIconSender().SendByPlayerConditionChange(CT_ITEM);
	pPlayer2->GetNPCIconSender().SendByPlayerConditionChange(CT_ITEM);
}

void GTradeExchangerForDBTask::UpdateMoney(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2, int nMoney1, int nMoney2)
{
	VALID(pPlayer1);
	VALID(pPlayer2);

	if (nMoney1 < 0)
		gsys.pMoneySystem->DecreaseForDBTask(pPlayer1, abs(nMoney1));
	else if (nMoney1 > 0)
		gsys.pMoneySystem->IncreaseForDBTask(pPlayer1, nMoney1);

	if (nMoney2 < 0)
		gsys.pMoneySystem->DecreaseForDBTask(pPlayer2, abs(nMoney2));
	else if (nMoney2 > 0)
		gsys.pMoneySystem->IncreaseForDBTask(pPlayer2, nMoney2);
}

void GTradeExchangerForDBTask::TradeServerItem(GEntityPlayer* pPlayer, GEntityPlayer* pToPlayer, GTRADE_CHAR& TradeChar, GDBT_TRADE_RESULT& Res, GDBT_TRADE_RESULT& ToRes)
{
	VALID(pPlayer);
	VALID(pToPlayer);
	
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	
	for (int i = 0; i < MAX_TRADE_ITEM_SPACE; ++i)
	{
		GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, TradeChar.m_vItem[i].m_nIUID);
		if (NULL == pItem) continue;

		if (TradeChar.m_vItem[i].m_nStackAmt == 0)
		{
			if (TradeChar.m_vItem[i].m_nSlotIDTo == INVALID_ID)	
				continue;

			ChangeOwner(pPlayer, pToPlayer, pItem, TradeChar.m_vItem[i].m_nSlotIDTo, Res, ToRes);			
		}
		else if (TradeChar.m_vItem[i].m_nStackAmt > 0)
		{
			DecreaseItemBySplit(pPlayer, pItem, pItem->GetAmount() - TradeChar.m_vItem[i].m_nStackAmt, Res);
		}
	}
}

// 아이템을 통째로 줌
void GTradeExchangerForDBTask::ChangeOwner( GEntityPlayer* pPlayer, GEntityPlayer* pToPlayer, GItem* pItem, int nSlotID, GDBT_TRADE_RESULT& Res, GDBT_TRADE_RESULT& ToRes )
{
	GItemHolder* pPlayerItemHolder = pPlayer->GetItemHolder();
	// Pop하고 나면 슬롯 타입과 슬롯 아이디가 초기화되서 미리 REMOVE_ITEM를 생성한다.
	REMOVE_ITEM removeItem = REMOVE_ITEM(SLOTTYPE_INVENTORY, pItem->m_nSlotID, 0, pItem->GetAmount());
	if (pPlayerItemHolder->PopItem(SLOTTYPE_INVENTORY, pItem->m_nSlotID))
	{
		Res.vecRemoveItem.push_back(removeItem);
		gsys.pQuestSystem->GetQObjUpdater().DecCollectQObj(pPlayer, pItem->m_pItemData->m_nID, pItem->GetAmount());
		gsys.pDBCacheSystem->DeleteItem(pPlayer->GetUID(), pItem->m_nIUID);
	}

	GItemHolder* pToPlayerItemHolder = pToPlayer->GetItemHolder();	
	if (pToPlayerItemHolder->PushItem(SLOTTYPE_INVENTORY, nSlotID, pItem))
	{
		ToRes.vecAddItem.push_back(ADD_ITEM(SLOTTYPE_INVENTORY, pItem->m_nSlotID, pItem->GetAmount(), pItem->GetAmount(), 
			pItem->m_nIUID, pItem->m_pItemData->m_nID, -1, 0, pItem->m_nDurability, pItem->m_nDyedColor));
		gsys.pQuestSystem->GetQObjUpdater().IncCollectQObj(pToPlayer, pItem->m_pItemData->m_nID, pItem->GetAmount());
	}
}

// 아이템을 쪼개서 준것 감소
void GTradeExchangerForDBTask::DecreaseItemBySplit(GEntityPlayer* pPlayer, GItem* pItem, const int16 nTradeAmt, GDBT_TRADE_RESULT& Res )
{
	gsys.pItemSystem->GetRemover().Server_Decrease(pPlayer, SLOTTYPE_INVENTORY, pItem->m_nSlotID, nTradeAmt);
	Res.vecRemoveItem.push_back(REMOVE_ITEM(pItem->m_nSlotType, pItem->m_nSlotID, pItem->GetAmount(), (uint8) nTradeAmt));	
}

// 아이템을 쪼개서 받은것 추가
void GTradeExchangerForDBTask::InsertItemBySplit(GEntityPlayer* pPlayer, GEntityPlayer* pSender, const vector<GDBT_TRADE_ITEM>& vecDBInsertItemBySplit, GDBT_TRADE_RESULT& Res)
{
	for (size_t i = 0; i < vecDBInsertItemBySplit.size(); ++i)
	{
		// 내가 나한테 처리한것. 즉 분리되는 경우에 내 수량이 줄어든 정보이다.
		if (vecDBInsertItemBySplit[i].nToCID == vecDBInsertItemBySplit[i].nCID)
		{
			// 내가 가지고 있는 아이템의 수량이 변한것.
			continue;
		}

		if (pPlayer->GetCID() != vecDBInsertItemBySplit[i].nToCID)
		{
			continue;
		}

		const GDBT_TRADE_ITEM& ti = vecDBInsertItemBySplit[i];

		GItem* pItem = pSender->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, ti.nSlotID);
		if (NULL == pItem)
			continue;

		gsys.pItemSystem->GetAdder().Server_Insert(pPlayer, SLOTTYPE_INVENTORY, ti.nToSlotID, ti.nStackAmt, ti.nIUID, pItem->m_pItemData->m_nID, -1, ti.nRemainUsagePeriod, pItem->m_nDurability, pItem->m_nDyedColor);
		Res.vecAddItem.push_back(ADD_ITEM(SLOTTYPE_INVENTORY, ti.nToSlotID, ti.nStackAmt, ti.nStackAmt,	ti.nIUID, pItem->m_pItemData->m_nID, -1, ti.nRemainUsagePeriod, pItem->m_nDurability, pItem->m_nDyedColor));
	}
}