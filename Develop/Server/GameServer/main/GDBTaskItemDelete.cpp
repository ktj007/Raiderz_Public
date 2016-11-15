#include "stdafx.h"
#include "GDBTaskItemDelete.h"
#include "GGlobal.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
#include "GItemSystem.h"
#include "GItemRemover.h"
#include "GPlayerObjectManager.h"
#include "GItemHolder.h"
#include "GDBManager.h"


GDBTaskItemDelete::GDBTaskItemDelete( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMDELETE), m_Syncer(uidReqPlayer)
{

}

GDBTaskItemDelete::~GDBTaskItemDelete()
{

}

void GDBTaskItemDelete::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_Syncer.EndCacheSync(IsSuccess());
}

bool GDBTaskItemDelete::Input( const GDBT_ITEM_DATA& data )
{
	if (false == m_Syncer.BeginCacheSync(data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID))
	{
		m_Syncer.EndCacheSync(false);
		return false;
	}

	m_Data = data;

	return true;
}

void GDBTaskItemDelete::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_DELETE));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemDelete::_OnCompleted()
{
	// if (ITEM_TIER_RARE <= m_Data.m_nTier)
	gsys.pDBManager->ItemDeleteLog(m_Data);

	GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_Data.m_uidPlayer);
	if (NULL == pPlayerObj)
		return mdb::MDBTR_SUCESS;

	GEntityPlayer* pPlayer = pPlayerObj->GetEntity();
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	GItem* pItem = pPlayer->GetItemHolder()->GetItem((SH_ITEM_SLOT_TYPE)m_Data.m_Item.m_nSlotType, m_Data.m_Item.m_nSlotID); 
	if (NULL == pItem) 
		return mdb::MDBTR_SUCESS;

	if (m_Data.m_Item.m_nIUID != pItem->m_nIUID)
		return mdb::MDBTR_SUCESS;

	vector<REMOVE_ITEM> vecRemoveItem;	
	vecRemoveItem.push_back(REMOVE_ITEM(m_Data.m_Item.m_nSlotType, m_Data.m_Item.m_nSlotID, 0, pItem->GetAmount()));
	gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), vecRemoveItem);

	return mdb::MDBTR_SUCESS;
}