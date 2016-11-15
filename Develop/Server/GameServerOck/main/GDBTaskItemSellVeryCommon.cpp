#include "stdafx.h"
#include "GDBTaskItemSellVeryCommon.h"
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

GDBTaskItemSellVeryCommon::GDBTaskItemSellVeryCommon( const MUID& uidReqPlayer ) 
: GDBTaskNPCShopTrade(uidReqPlayer, SDBTID_ITEMSELLVERYCOMMON)
{

}

GDBTaskItemSellVeryCommon::~GDBTaskItemSellVeryCommon()
{

}

bool GDBTaskItemSellVeryCommon::Input( GDBT_SELL_ITEM_VERY_COMMON& data )
{
	if (!BeginCacheSync(data))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	m_Data.SetData(data);

	return true;
}

void GDBTaskItemSellVeryCommon::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_SELL));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSellVeryCommon::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskItemSellVeryCommon::BeginCacheSync(GDBT_SELL_ITEM_VERY_COMMON& data)
{
	if (!BeginCharCacheSync(data.m_uidPlayer))
		return false;

	for each(const GITEM_STACK_AMT& amt in data.m_vVeryComm)
	{
		if (!BeginItemCacheSync(data.m_uidPlayer, amt.nSlotType, amt.nSlotID, amt.nIUID))
			return false;
	}

	return true;
}

void GDBTaskItemSellVeryCommon::Completer::Do()
{
	gsys.pDBManager->NPCShop_SellVeryCommonLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	DeleteItem(pPlayer);
	IncreaseMoney(pPlayer);

	gsys.pNPCShopSystem->GetSeller().SellVeryCommon_Route(pPlayer);
}

void GDBTaskItemSellVeryCommon::Completer::DeleteItem( GEntityPlayer* pPlayer )
{
	vector<REMOVE_ITEM> vecRemoveItem;
	
	for (size_t i = 0; i < m_Data.m_vVeryComm.size(); ++i)
	{
		GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, m_Data.m_vVeryComm[i].nSlotID);
		if (NULL == pItem) continue;

		vecRemoveItem.push_back(REMOVE_ITEM(SLOTTYPE_INVENTORY, m_Data.m_vVeryComm[i].nSlotID, 0, pItem->GetAmount()));
	}

	gsys.pItemSystem->GetRemover().RemoveForDBTask(m_Data.m_uidPlayer, vecRemoveItem);
}

void GDBTaskItemSellVeryCommon::Completer::IncreaseMoney( GEntityPlayer* pPlayer )
{
	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_nModMoney);
}