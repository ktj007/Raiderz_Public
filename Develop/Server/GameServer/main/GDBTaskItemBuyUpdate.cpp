#include "stdafx.h"
#include "GDBTaskItemBuyUpdate.h"
#include "GDBTaskGlobal.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GPlayerObjectManager.h"
#include "GMoneySystem.h"
#include "GItemManager.h"
#include "GCommand.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "GDBManager.h"
#include "GNPCShopSystem.h"
#include "GNPCShopBuyer.h"

GDBTaskItemBuyUpdate::GDBTaskItemBuyUpdate( const MUID& uidReqPlayer ) 
: GDBTaskNPCShopTrade(uidReqPlayer, SDBTID_ITEMBUYUPDATE)
{	
}

GDBTaskItemBuyUpdate::~GDBTaskItemBuyUpdate()
{

}

bool GDBTaskItemBuyUpdate::Input( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	if(!BeginCharCacheSync(data.m_uidPlayer))
		return false;

	if (!BeginItemCacheSync(data.m_uidPlayer, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}
	
	m_Data = data;

	return true;
}

void GDBTaskItemBuyUpdate::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery qr(&rfDB);

	ExecuteW(qr, GetSQLW(ITEM_BUY));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemBuyUpdate::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

GDBTaskItemBuyUpdate::Completer::Completer( GDBT_NPC_SHOP_TRADE_DATA& Data ) : m_Data(Data)
{

}

void GDBTaskItemBuyUpdate::Completer::Do()
{
	gsys.pDBManager->NPCShop_BuyLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	UpdateItem(pPlayer);
	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, m_Data.m_nModMoney);	
	gsys.pNPCShopSystem->GetBuyer().Route(pPlayer);
}

void GDBTaskItemBuyUpdate::Completer::UpdateItem( GEntityPlayer* pPlayer )
{
	if (0 >= m_Data.m_Item.m_nStackAmt)
		return;

	ADD_ITEM addItem(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, (uint8)m_Data.m_Item.m_nStackAmt, m_Data.m_nModStackAmt);
	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);
}