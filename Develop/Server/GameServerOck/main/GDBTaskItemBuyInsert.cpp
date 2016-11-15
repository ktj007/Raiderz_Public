#include "stdafx.h"
#include "GDBTaskItemBuyInsert.h"
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

GDBTaskItemBuyInsert::GDBTaskItemBuyInsert( const MUID& uidReqPlayer ) 
: GDBTaskNPCShopTrade(uidReqPlayer, SDBTID_ITEMBUYINSERT)
{	
}

GDBTaskItemBuyInsert::~GDBTaskItemBuyInsert()
{

}

bool GDBTaskItemBuyInsert::Input( const GDBT_NPC_SHOP_TRADE_DATA& InputData )
{
	if (!BeginCharCacheSync(InputData.m_uidPlayer))
		return false;

	m_InputData = InputData;

	return true;
}

void GDBTaskItemBuyInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(ITEM_BUY)))
		return;

	m_Result.nIUID = rs.FieldW(L"IUID").AsInt64();
}

mdb::MDB_THRTASK_RESULT GDBTaskItemBuyInsert::_OnCompleted()
{
	Completer completer(m_InputData, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskItemBuyInsert::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_InputData.m_uidPlayer);
	if (NULL != pPlayer)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_InputData.m_Item.m_nSlotID);
	}

	return mdb::MDBTR_SUCESS;
}

GDBTaskItemBuyInsert::Completer::Completer( GDBT_NPC_SHOP_TRADE_DATA& Data, _RESULT& Result ) : m_Data(Data), m_Result(Result)
{

}

void GDBTaskItemBuyInsert::Completer::Do()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	m_Data.m_Item.m_nIUID = m_Result.nIUID;

	gsys.pDBManager->NPCShop_BuyLog(m_Data);

	InsertItem();
	DecreaseMoney(pPlayer);

	gsys.pNPCShopSystem->GetBuyer().Route(pPlayer);
}

void GDBTaskItemBuyInsert::Completer::InsertItem()
{
	if (0 >= m_Data.m_Item.m_nStackAmt || 0 == m_Result.nIUID)
		return;

	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_Item.m_nStackAmt,
		m_Result.nIUID, m_Data.m_Item.m_nItemID, m_Data.m_nAddExpiMin, m_Data.m_Item.m_nUsagePeriod, m_Data.m_Item.m_nDura, m_Data.m_Item.m_nColor);
	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);
}

void GDBTaskItemBuyInsert::Completer::DecreaseMoney(GEntityPlayer* pPlayer)
{
	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, pPlayer->GetMoney() - m_Data.m_nMoney);
}