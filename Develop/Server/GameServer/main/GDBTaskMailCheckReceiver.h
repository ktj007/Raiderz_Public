#pragma once

#include "GDBAsyncTask.h"
#include "GDBTaskData.h"
#include "CCommandResultTable.h"

class GDBTaskMailCheckReceiver: public GDBAsyncTask, public MMemPool<GDBTaskMailCheckReceiver>
{
public:
	GDBTaskMailCheckReceiver(const MUID& uidReqPlayer);
	virtual ~GDBTaskMailCheckReceiver();

	enum
	{
		SQL_MAIL_CHECK_RCVR = 0
	};

	void Input(const GDBT_MAIL_WRITE& data);
	mdb::MDB_THRTASK_RESULT RouteResult(CCommandResultTable nResult);
	mdb::MDB_THRTASK_RESULT MailWrite(void);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

private :
	struct _DATA
	{
		MUID m_uidPlayer;
		GDBT_MAIL_WRITE m_mailWrite;
	};

	struct _RESULT
	{
		int	m_nErrorNo;
	};

protected :
	_DATA	m_Data;
	_RESULT m_Result;
};
