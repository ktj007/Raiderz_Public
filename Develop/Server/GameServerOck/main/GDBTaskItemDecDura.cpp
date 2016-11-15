#include "stdafx.h"
#include "GDBTaskItemDecDura.h"
#include "GGlobal.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
#include "GItemSystem.h"
#include "GItemDurabilityUpdater.h"
#include "GPlayerObjectManager.h"
#include "GItemHolder.h"
#include "GDBTaskGlobal.h"


GDBTaskItemDecDura::GDBTaskItemDecDura( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMUPDATE_DURA), m_SyncingList(uidReqPlayer)
{

}

GDBTaskItemDecDura::~GDBTaskItemDecDura()
{

}

bool GDBTaskItemDecDura::Input( GDBT_ITEM_DEC_DURA_DATA& data )
{
	if (!DecCache(data))
		return false;

	m_Data = data;

	return true;
}

void GDBTaskItemDecDura::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_UPDATE_DURA));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemDecDura::_OnCompleted()
{
	GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_Data.m_uidPlayer);
	if (NULL == pPlayerObj)
		return mdb::MDBTR_SUCESS;

	GEntityPlayer* pPlayer = pPlayerObj->GetEntity();
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	GItem* pItem = pPlayer->GetItemHolder()->GetItem((SH_ITEM_SLOT_TYPE)m_Data.m_nSlotType, m_Data.m_nSlotID); 
	if (NULL == pItem) 
		return mdb::MDBTR_SUCESS;

	if (m_Data.m_nIUID != pItem->m_nIUID)
		return mdb::MDBTR_SUCESS;

	vector<GItem*>	vecRepairItem;
	vector<int>		vecRepairDurability;

	vecRepairItem.push_back(pItem);
	vecRepairDurability.push_back(m_Data.m_nDura);

	gsys.pItemSystem->GetDurabilityUpdater().ApplyServerAndRoute(pPlayer, vecRepairItem, vecRepairDurability);

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskItemDecDura::DecCache( GDBT_ITEM_DEC_DURA_DATA& data )
{
	GItemDBCache* pi = gsys.pDBCacheSystem->FindItemCache(data.m_uidPlayer, data.m_nSlotType, data.m_nSlotID, data.m_nIUID);
	if (NULL == pi)
		return false;

	if (!pi->DecDura(data.m_nModDura))
		return false;

	if (!pi->IsOverflow())
	{
		SetSync(false);
		pi->EndCacheSync(false);
	}
	else
	{
		if (NULL == m_SyncingList.PushItem(data.m_nIUID))
		{
			pi->EndCacheSync(false);
			return false;
		}

		SetSync(true);
	}

	return true;
}

void GDBTaskItemDecDura::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	if (!IsSync())
		return;

	m_SyncingList.EndCacheSync(IsSuccess());
}