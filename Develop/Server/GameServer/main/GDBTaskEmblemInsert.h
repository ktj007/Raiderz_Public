#ifndef _GDBTASK_EMBLEM_INSERT_H
#define _GDBTASK_EMBLEM_INSERT_H


#include "GDBAsyncTask.h"
#include "MMemPool.h"


class GDBTaskEmblemInsert : public GDBAsyncTask, public MMemPool<GDBTaskEmblemInsert>
{
public :
	GDBTaskEmblemInsert(const MUID& uidPlayer);
	~GDBTaskEmblemInsert();

	enum
	{
		QUALIFY_INSERT,
	};

	struct _DATA
	{
		MUID	uidPlayer;
		int		nQualifyID;
	};

public :
	void					Input(const int nQualifyID);
	const _DATA&			GetData()							{ return m_Data; }

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();

protected :
	_DATA m_Data;
};


#endif