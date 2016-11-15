#include "stdafx.h"
#include "GDBTaskItemStackAmt.h"
#include "GDBTaskGlobal.h"


GDBTaskItemStackAmt::GDBTaskItemStackAmt( const MUID& uidPlayer, const SDBTASK_ID TaskID ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, TaskID), m_SyncingList(uidPlayer)
{

}


bool GDBTaskItemStackAmt::IncCache( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nModStackAmt, const int16 nStackAmt, const uint8 nTier )
{
	GItemDBCache* pi = gsys.pDBCacheSystem->FindItemCache(uidPlayer, nSlotType, nSlotID, nIUID);
	if (NULL == pi)
		return false;

	if (!pi->IncStackAmt(nModStackAmt))
		return false;

	if (!pi->IsOverflow())
	{
		SetSync(false);
		pi->EndCacheSync(false);
	}
	else
	{
		if (NULL == m_SyncingList.PushItem(nIUID))
		{
			pi->EndCacheSync(false);
			return false;
		}				
	}	
	
	return true;
}

bool GDBTaskItemStackAmt::DecCache( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nModStackAmt, const int16 nStackAmt, const uint8 nTier )
{
	GItemDBCache* pi = gsys.pDBCacheSystem->FindItemCache(uidPlayer, nSlotType, nSlotID, nIUID);
	if (NULL == pi)
		return false;

	if (!pi->DecStackAmt(nModStackAmt))
		return false;

	if (!pi->IsOverflow())
	{
		SetSync(false);
		pi->EndCacheSync(false);
	}
	else
	{
		if (NULL == m_SyncingList.PushItem(nIUID))
		{
			pi->EndCacheSync(false);
			return false;
		}
	}	

	return true;
}

void GDBTaskItemStackAmt::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	m_SyncingList.EndCacheSync(IsSuccess());
}

