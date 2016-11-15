#ifndef _GDBTASK_NPC_SHOP_H
#define _GDBTASK_NPC_SHOP_H


#include "GDBAsyncTask.h"
#include "GDBTaskNPCShopData.h"
#include "GSyncingCacheList.h"


class GPlayerDBCache;
class GCharacterDBCache;


class GDBTaskNPCShopTrade : public GDBAsyncTask
{
public :
	GDBTaskNPCShopTrade(const MUID& uidPlayer, const SDBTASK_ID TaskID);
	virtual ~GDBTaskNPCShopTrade() {}

	virtual void	_OnPreCompleted()	override;

protected :
	bool			BeginCharCacheSync(const MUID& uidPlayer);
	bool			BeginItemCacheSync(const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);

protected :
	GSyncingCacheList	m_SyncingList;
};



#endif