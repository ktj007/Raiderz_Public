#include "stdafx.h"
#include "GDBTaskItemSellUpdate.h"
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
#include "GNPCShopSeller.h"
#include "GNPCShopSystem.h"

GDBTaskItemSellUpdate::GDBTaskItemSellUpdate( const MUID& uidReqPlayer ) 
: GDBTaskNPCShopTrade(uidReqPlayer, SDBTID_ITEMSELLUPDATE)
{	
}

GDBTaskItemSellUpdate::~GDBTaskItemSellUpdate()
{

}

bool GDBTaskItemSellUpdate::Input( const GDBT_NPC_SHOP_TRADE_DATA& data )
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

void GDBTaskItemSellUpdate::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_SELL));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSellUpdate::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemSellUpdate::Completer::Do()
{
	gsys.pDBManager->NPCShop_SellLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	UpdateItem(pPlayer);
	IncreaseMoney(pPlayer);

	gsys.pNPCShopSystem->GetSeller().Sell_Route(pPlayer);
}

void GDBTaskItemSellUpdate::Completer::UpdateItem( GEntityPlayer* pPlayer )
{
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID);
	if (NULL == pItem) return;

	gsys.pItemSystem->GetRemover().RemoveForDBTask(m_Data.m_uidPlayer
		, REMOVE_ITEM(m_Data.m_Item.m_nSlotType, m_Data.m_Item.m_nSlotID, (uint8)m_Data.m_Item.m_nStackAmt, m_Data.m_nModStackAmt));
}

void GDBTaskItemSellUpdate::Completer::IncreaseMoney( GEntityPlayer* pPlayer )
{
	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_nModMoney);
}