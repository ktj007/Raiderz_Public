#include "stdafx.h"
#include "GDBTaskItemSellDelete.h"
#include "GDBTaskGlobal.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GPlayerObjectManager.h"
#include "GCommand.h"
#include "GMoneySystem.h"
#include "GItemRemover.h"
#include "GItemHolder.h"
#include "GDBManager.h"
#include "GNPCShopSystem.h"
#include "GNPCShopSeller.h"

GDBTaskItemSellDelete::GDBTaskItemSellDelete( const MUID& uidReqPlayer ) 
: GDBTaskNPCShopTrade(uidReqPlayer, SDBTID_ITEMSELLDELETE)
{	
}

GDBTaskItemSellDelete::~GDBTaskItemSellDelete()
{

}

bool GDBTaskItemSellDelete::Input( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	if (!BeginCharCacheSync(data.m_uidPlayer))
		return false;
	
	if (!BeginItemCacheSync(data.m_uidPlayer, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	m_Data = data;

	return true;
}

void GDBTaskItemSellDelete::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_SELL));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSellDelete::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemSellDelete::Completer::Do()
{
	gsys.pDBManager->NPCShop_SellLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	DeleteItem(pPlayer);
	IncreaseMoney(pPlayer);

	gsys.pNPCShopSystem->GetSeller().Sell_Route(pPlayer);
}

void GDBTaskItemSellDelete::Completer::DeleteItem( GEntityPlayer* pPlayer )
{
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID);
	if (NULL == pItem) return;

	vector<REMOVE_ITEM> vecItem;
	vecItem.push_back(REMOVE_ITEM(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, 0, pItem->GetAmount()));

	gsys.pItemSystem->GetRemover().RemoveForDBTask(m_Data.m_uidPlayer, vecItem);
}

void GDBTaskItemSellDelete::Completer::IncreaseMoney( GEntityPlayer* pPlayer )
{
	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_nModMoney);
}