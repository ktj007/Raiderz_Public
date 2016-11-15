#include "stdafx.h"
#include "GDBTaskGuildDelete.h"
#include "GDBTaskGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"
#include "GDBManager.h"


GDBTaskGuildDelete::GDBTaskGuildDelete( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_GUILDDELETE)
{

}

GDBTaskGuildDelete::~GDBTaskGuildDelete()
{

}

void GDBTaskGuildDelete::Input(GDBT_GUILD& data)
{
	m_Data = data;
}

void GDBTaskGuildDelete::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(GUILD_DELETE));
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildDelete::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskGuildDelete::Completer::Do()
{
	gsys.pDBManager->GuildLog(m_Data);
	gsys.pGuildSystem->ForDBTask().DeleteGuild((int)m_Data.m_nGID);	
}