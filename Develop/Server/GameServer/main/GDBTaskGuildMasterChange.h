#ifndef _GDBTASK_GUILD_MASTER_CHANGE_H
#define _GDBTASK_GUILD_MASTER_CHANGE_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataGuild.h"
#include "MMemPool.h"


class GDBTaskGuildMasterChange : public GDBAsyncTask, public MMemPool<GDBTaskGuildMasterChange>
{
public :
	GDBTaskGuildMasterChange(const MUID& uidCurMaster);
	~GDBTaskGuildMasterChange();

	enum
	{
		GUILD_MASTER_CHANGE = 0,
	};

	void Input(GDBT_GUILD_CHANGE_MASTER& data);
	

protected :
	void					OnExecute(mdb::MDatabase& rfDB) override;
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;	


protected :
	class Completer
	{
	public :
		Completer(GDBT_GUILD_CHANGE_MASTER& data) : m_Data(data) {}

		void Do();

	private :
		GDBT_GUILD_CHANGE_MASTER& m_Data;
	};


protected :
	GDBT_GUILD_CHANGE_MASTER m_Data;
};


#endif