#include "stdafx.h"
#include "GDBTaskItemSwapSlot.h"
#include "GDBTaskGlobal.h"
#include "GItemHolder.h"
#include "GItemSystem.h"
#include "GItemEquiper.h"
#include "GItemUnequiper.h"
#include "GItemMoverForDBTask.h"
#include "GGuildSystem.h"
#include "GDBManager.h"


GDBTaskItemSwapSlot::GDBTaskItemSwapSlot( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMSWAPSLOT), m_Syncer(uidReqPlayer)
{
}

GDBTaskItemSwapSlot::~GDBTaskItemSwapSlot()
{

}

bool GDBTaskItemSwapSlot::Input(GDBT_ITEM_MOVE& swapData)
{
	if (0 != swapData.m_FromSlot.m_nIUID && !m_Syncer.BeginCacheSync(swapData.m_FromSlot.m_nSlotType, swapData.m_FromSlot.m_nSlotID, swapData.m_FromSlot.m_nIUID))
		return false;

	if (0 != swapData.m_ToSlot.m_nIUID && !m_Syncer.BeginCacheSync(swapData.m_ToSlot.m_nSlotType, swapData.m_ToSlot.m_nSlotID, swapData.m_ToSlot.m_nIUID))
	{
		m_Syncer.EndCacheSync(false);
		return false;
	}

	m_swapData = swapData;	

	return true;
}

void GDBTaskItemSwapSlot::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_SWAP_SLOT));
}

void GDBTaskItemSwapSlot::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_Syncer.EndCacheSync(IsSuccess());

	gsys.pItemSystem->GetMoverForDBTask().OnPreCompleted(m_swapData.m_uidPlayer, m_swapData.m_FromSlot.m_nSlotType, m_swapData.m_FromSlot.m_nSlotID
		, m_swapData.m_ToSlot.m_nSlotType, m_swapData.m_ToSlot.m_nSlotID, m_swapData.m_FromSlot.m_nStackAmt);
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSwapSlot::_OnCompleted()
{
	gsys.pDBManager->ItemMoveLog(m_swapData);

	if (m_swapData.m_uidPlayer.IsValid())
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_swapData.m_uidPlayer);
		if (NULL == pPlayer) return mdb::MDBTR_SUCESS;

		gsys.pItemSystem->GetMoverForDBTask().Move(m_swapData.m_uidPlayer	, m_swapData.m_FromSlot.m_nSlotType, m_swapData.m_FromSlot.m_nSlotID
																			, m_swapData.m_ToSlot.m_nSlotType, m_swapData.m_ToSlot.m_nSlotID, m_swapData.m_FromSlot.m_nStackAmt);
		gsys.pDBCacheSystem->SetItemSlot(m_swapData.m_uidPlayer, m_swapData.m_ToSlot.m_nSlotType, m_swapData.m_ToSlot.m_nSlotID, m_swapData.m_FromSlot.m_nIUID);

		if (0 != m_swapData.m_ToSlot.m_nIUID)
		{
			gsys.pDBCacheSystem->SetItemSlot(m_swapData.m_uidPlayer, m_swapData.m_FromSlot.m_nSlotType, m_swapData.m_FromSlot.m_nSlotID, m_swapData.m_ToSlot.m_nIUID);
		}		
	}	

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskItemSwapSlot::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_swapData.m_uidPlayer);
	if (NULL == pPlayer) return mdb::MDBTR_SUCESS;

	pPlayer->GetItemHolder()->CancelReserveSlot(m_swapData.m_ToSlot.m_nSlotType, m_swapData.m_ToSlot.m_nSlotID);

	if (SLOTTYPE_GUILDSTORAGE == m_swapData.m_FromSlot.m_nSlotType || SLOTTYPE_GUILDSTORAGE == m_swapData.m_ToSlot.m_nSlotType)
	{
		gsys.pGuildSystem->RouteSyncFail(m_swapData.m_uidPlayer);
	}

	return mdb::MDBTR_SUCESS;
}
