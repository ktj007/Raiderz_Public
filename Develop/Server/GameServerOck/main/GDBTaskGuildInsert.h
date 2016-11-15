#ifndef _GDBTASK_GUILD_INSERT_H
#define _GDBTASK_GUILD_INSERT_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataGuild.h"
#include "MMemPool.h"


class GDBTaskGuildInsert : public GDBAsyncTask, public MMemPool<GDBTaskGuildInsert>
{
public :
	GDBTaskGuildInsert(const MUID& uidReqPlayer);
	~GDBTaskGuildInsert();

	enum
	{
		GUILD_INSERT,
	};

	void Input(GDBT_GUILD& data);


protected :
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	class Completer
	{
	public :
		Completer(GDBT_GUILD& Data) : m_Data(Data) {}

		void Do();

	private :
		bool IsVaildGID();

		void InsertGuild();
		void ErrorGuildInsert();

	private :
		GDBT_GUILD& m_Data;
	};


protected :
	GDBT_GUILD m_Data;
};



#endif