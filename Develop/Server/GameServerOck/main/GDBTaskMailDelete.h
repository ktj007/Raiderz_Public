#ifndef _GDBTASK_MAIL_DELETE_H
#define _GDBTASK_MAIL_DELETE_H

#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskData.h"


class GDBTaskMailDelete : public GDBAsyncTask, public MMemPool<GDBTaskMailDelete>
{
public:
	GDBTaskMailDelete(const MUID& uidReqPlayer, int64 nMailUID);
	~GDBTaskMailDelete(void);

	enum
	{
		SQL_MAIL_DELETE = 0
	};

	void Input(const GDBT_MAIL_DELETE& data);
	
	virtual void			OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

private:
	struct _DATA
	{
		MUID				uidPlayer;
		int64				nMailUID;
		GDBT_MAIL_DELETE	MailData;

		_DATA() : uidPlayer(MUID::ZERO), nMailUID(0) {}
	};

	_DATA m_Data;

	class Completer
	{
	public :
		Completer(_DATA& Data);
		void Do();

	private:
		_DATA m_Data;
	};

};

#endif