#ifndef _GDBTASK_GUILD_MEMBER_INSERT_H
#define _GDBTASK_GUILD_MEMBER_INSERT_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataGuild.h"
#include "MMemPool.h"


class GDBTaskGuildMemberInsert : public GDBAsyncTask, public MMemPool<GDBTaskGuildMemberInsert>
{
public :
	GDBTaskGuildMemberInsert(const MUID& uidReqPlayer);
	~GDBTaskGuildMemberInsert();

	enum
	{
		GUILD_MEMBER_INSERT = 0,
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