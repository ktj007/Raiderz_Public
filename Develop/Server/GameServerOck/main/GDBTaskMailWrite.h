#pragma once

#include "GDBAsyncTask.h"
#include "GDBTaskData.h"
#include "CCommandResultTable.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GMailSyncingCacheList : public GSyncingCacheList, public MMemPool<GMailSyncingCacheList>
{
public :
	GMailSyncingCacheList() : GSyncingCacheList(0) {}
};


class GDBTaskMailWrite: public GDBAsyncTask, public MMemPool<GDBTaskMailWrite>
{
public :
	GDBTaskMailWrite(const MUID& uidReqPlayer);
	virtual ~GDBTaskMailWrite();

	enum
	{
		SQL_MAIL_WRITE = 0
	};

	bool Input(const GDBT_MAIL_WRITE& data);
	void OnExecute(mdb::MDatabase& rfDB) override;
	

protected:	
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

	mdb::MDB_THRTASK_RESULT RouteResult(CCommandResultTable nResult);

	bool	BeginCacheSync(const GDBT_MAIL_WRITE& data);	
	bool	SyncCharacter(void);
	bool	SyncItem(const GDBT_MAIL_WRITE& data);
	void	EndSync(bool bZero);
	
private :
	struct _DATA
	{
		MUID			m_uidPlayer;
		GDBT_MAIL_WRITE	m_mailWrite;
	};

	struct _RESULT
	{
		GDBT_MAIL_WRITE_RESULT	m_mailResult;
	};

protected :
	_DATA	m_Data;
	_RESULT m_Result;
	GMailSyncingCacheList	m_SyncingList;
};
