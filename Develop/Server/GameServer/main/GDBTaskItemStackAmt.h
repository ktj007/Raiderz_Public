#ifndef _GDBTASK_ITEM_STACK_AMD_H
#define _GDBTASK_ITEM_STACK_AMD_H


#include "GDBAsyncTask.h"
#include "GSyncingCacheList.h"


class GDBTaskItemStackAmt : public GDBAsyncTask
{
public :
	GDBTaskItemStackAmt(const MUID& uidPlayer, const SDBTASK_ID TaskID);


	bool			IncCache(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nModStackAmt, const int16 nStackAmt, const uint8 nTier);
	bool			DecCache(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nModStackAmt, const int16 nStackAmt, const uint8 nTier);
	virtual void	_OnPreCompleted() override;


protected :
	GSyncingCacheList	m_SyncingList;	
};


#endif