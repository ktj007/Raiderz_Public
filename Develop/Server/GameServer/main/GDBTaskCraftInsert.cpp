#include "stdafx.h"
#include "GDBTaskCraftInsert.h"
#include "GDBTaskGlobal.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GQuestSystem.h"
#include "GItemAdder.h"
#include "GMoneySystem.h"
#include "GItemRemover.h"
#include "GItemHolder.h"
#include "GDBManager.h"

GDBTaskCraftInsert::GDBTaskCraftInsert( const MUID& uidReqPlayer ) 
: GDBTaskCraft(uidReqPlayer, SDBTID_CRAFTINSERT)
{
	
}

GDBTaskCraftInsert::~GDBTaskCraftInsert()
{

}

bool GDBTaskCraftInsert::Input( GDBT_CRAFT_DATA& data )
{
	if (!BeginCacheSync(data.m_uidPlayer, data.m_vRecp))
		return false;

	m_Data.SetData(data);

	return true;
}

void GDBTaskCraftInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(CRAFT)))
		return;

	if(1 == rs.GetFetchedCount())
		m_Result.nIUID = rs.FieldW(L"IUID").AsInt64();		
}

mdb::MDB_THRTASK_RESULT GDBTaskCraftInsert::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL != pPlayer)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID);
	}

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskCraftInsert::_OnCompleted()
{
	Completer completer(m_Data, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskCraftInsert::Completer::Do()
{	
	gsys.pDBManager->CraftLog(m_Data, m_Result.nIUID);

	SetMoney();
	RemoveMatItem();
	AddProductItem();	
}

void GDBTaskCraftInsert::Completer::SetMoney()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, m_Data.m_nModMoney);
}

void GDBTaskCraftInsert::Completer::RemoveMatItem()
{
	vector<REMOVE_ITEM> vecItem;
	
	for (size_t i = 0; i < m_Data.m_vRecp.size(); ++i)
	{
		vecItem.push_back(REMOVE_ITEM(m_Data.m_vRecp[i].nSlotType, m_Data.m_vRecp[i].nSlotID, m_Data.m_vRecp[i].nToStackAmt, m_Data.m_vRecp[i].nModStackAmt));
	}

	gsys.pItemSystem->GetRemover().RemoveForDBTask(m_Data.m_uidPlayer, vecItem);
}

void GDBTaskCraftInsert::Completer::AddProductItem()
{
	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, m_Data.m_Item.m_nSlotID, m_Data.m_Item.m_nStackAmt, m_Data.m_nModStackAmt
		, m_Result.nIUID, m_Data.m_Item.m_nItemID, m_Data.m_nAddExpiMin, m_Data.m_Item.m_nUsagePeriod, m_Data.m_Item.m_nDura, m_Data.m_Item.m_nColor);
	if (false == gsys.pItemSystem->GetAdder().AddForDBTask(m_Data.m_uidPlayer, addItem))
	{
		mlog3("GDBTaskCraftInsert::AddProductItem() - Assert! pItem in NULL\n");
		return;
	}

	m_Data.m_Item.m_nIUID = m_Result.nIUID;
}