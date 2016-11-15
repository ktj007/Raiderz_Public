#ifndef _GDBTASK_EMBLEM_DELETE_H
#define _GDBTASK_EMBLEM_DELETE_H


#include "GDBAsyncTask.h"
#include "MMemPool.h"


class GDBTaskEmblemDelete : public GDBAsyncTask, public MMemPool<GDBTaskEmblemDelete>
{
public :
	GDBTaskEmblemDelete(const MUID& uidPlayer);

	enum
	{
		QUALIFY_DELETE,
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