#include "stdafx.h"
#include "GDBTaskGuildMemberInsert.h"
#include "GDBTaskGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"
#include "GDBManager.h"


GDBTaskGuildMemberInsert::GDBTaskGuildMemberInsert( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_GUILDMEMBERINSERT)
{	
	
}

GDBTaskGuildMemberInsert::~GDBTaskGuildMemberInsert()
{

}

void GDBTaskGuildMemberInsert::Input( GDBT_GUILD& data )
{
	m_Data = data;
}

void GDBTaskGuildMemberInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(GUILD_MEMBER_INSERT));
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildMemberInsert::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskGuildMemberInsert::Completer::Do()
{
	gsys.pDBManager->GuildMemberLog(m_Data);
	gsys.pGuildSystem->ForDBTask().Join(m_Data.m_uidPlayer, (int)m_Data.m_nGID);	
}