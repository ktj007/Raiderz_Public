#include "stdafx.h"
#include "GDBTaskCraftUpdate.h"
#include "GDBTaskGlobal.h"
#include "GSystem.h"
#include "GQuestSystem.h"
#include "GItemAdder.h"
#include "GMoneySystem.h"
#include "GItemRemover.h"
#include "GItemHolder.h"
#include "GDBManager.h"

GDBTaskCraftUpdate::GDBTaskCraftUpdate( const MUID& uidPlayer ) 
: GDBTaskCraft(uidPlayer, SDBTID_CRAFTUPDATE)
{
	
}

GDBTaskCraftUpdate::~GDBTaskCraftUpdate()
{

}

bool GDBTaskCraftUpdate::Input(GDBT_CRAFT_DATA& data)
{
	if (!BeginCacheSync(data))
		return false;

	m_Data.SetData(data);
	
	return true;
}

void GDBTaskCraftUpdate::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(CRAFT));
}

mdb::MDB_THRTASK_RESULT GDBTaskCraftUpdate::_OnFailed()
{
	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskCraftUpdate::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskCraftUpdate::BeginCacheSync( GDBT_CRAFT_DATA& data )
{
	if (!__super::BeginCacheSync(data.m_uidPlayer, data.m_vRecp))
		return false;

	GItemDBCache* ppi = gsys.pDBCacheSystem->FindItemCache(data.m_uidPlayer, (SH_ITEM_SLOT_TYPE)data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID);
	if (NULL == ppi)
		return false;

	if (!ppi->BeginCacheSync())
	{
		EndCacheSync(false);
		return false;
	}

	if (NULL == m_SyncingList.PushItem(ppi->GetIUID()))
	{
		ppi->EndCacheSync(false);
		return false;
	}

	return true;
}
void GDBTaskCraftUpdate::Completer::Do()
{
	gsys.pDBManager->CraftLog(m_Data, m_Data.m_Item.m_nIUID);

	SetMoney();
	RemoveMatItem();
	MergeProductItem();	
}

void GDBTaskCraftUpdate::Completer::SetMoney()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer) return;

	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, m_Data.m_nModMoney);
}

void GDBTaskCraftUpdate::Completer::RemoveMatItem()
{
	vector<REMOVE_ITEM> vecRemoveItem;
	for (size_t i = 0; i < m_Data.m_vRecp.size(); ++i)
	{
		vecRemoveItem.push_back(REMOVE_ITEM(m_Data.m_vRecp[i].nSlotType, m_Data.m_vRecp[i].nSlotID, m_Data.m_vRecp[i].nToStackAmt, m_Data.m_vRecp[i].nModStackAmt));
	}

	gsys.pItemSystem->GetRemover().RemoveForDBTask(m_Data.m_uidPlayer, vecRemoveItem);
}

void GDBTaskCraftUpdate::Completer::MergeProductItem()
{
	ADD_ITEM addItem = ADD_ITEM((SH_ITEM_SLOT_TYPE)m_Data.m_Item.m_nSlotType, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_nModStackAmt);
	gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem);
}
