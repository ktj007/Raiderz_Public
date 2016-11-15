#ifndef _GDBTASK_CRAFT_H
#define _GDBTASK_CRAFT_H


#include "GDBAsyncTask.h"
#include "GSyncingCacheList.h"
#include "GITEM_STACK_AMT.h"


class GPlayerDBCache;


class GDBTaskCraft : public GDBAsyncTask
{
public :
	GDBTaskCraft(const MUID& uidReqPlayer, const SDBTASK_ID TaskID);
	virtual ~GDBTaskCraft() {}

	virtual void	_OnPreCompleted() override;

	bool			BeginCacheSync(const MUID& uidPlayer, GITEM_STACK_AMT_VEC& vRecp);
	bool			SyncCharacter(GPlayerDBCache* pc);
	bool			SyncItem(const MUID& uidPlayer, GITEM_STACK_AMT_VEC& vItem);
	void			EndCacheSync(const bool bZero);

protected :
	GSyncingCacheList	m_SyncingList;
};


#endif