#ifndef _GDBTASK_GUILD_DELETE_H
#define _GDBTASK_GUILD_DELETE_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataGuild.h"
#include "MMemPool.h"


class GDBTaskGuildDelete : public GDBAsyncTask, public MMemPool<GDBTaskGuildDelete>
{
public :
	GDBTaskGuildDelete(const MUID& uidReqPlayer);
	~GDBTaskGuildDelete();

	enum
	{
		GUILD_DELETE,
	};

	void Input(GDBT_GUILD& data);

	
protected :
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	class Completer
	{
	public :
		Completer(GDBT_GUILD& data) : m_Data(data) {}

		void Do();

	private :
		GDBT_GUILD& m_Data;
	};

protected :
	GDBT_GUILD m_Data;
};

#endif