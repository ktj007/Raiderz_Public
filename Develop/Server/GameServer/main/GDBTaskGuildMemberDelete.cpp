#include "stdafx.h"
#include "GDBTaskGuildMemberDelete.h"
#include "GDBTaskGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"
#include "GDBManager.h"
#include "GDB_CODE.h"


GDBTaskGuildMemberDelete::GDBTaskGuildMemberDelete( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_GUILDMEMBERDELETE)
{
	
}

GDBTaskGuildMemberDelete::~GDBTaskGuildMemberDelete()
{

}

void GDBTaskGuildMemberDelete::Input( GDBT_GUILD& data )
{
	m_Data = data;
}

void GDBTaskGuildMemberDelete::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(GUILD_MEMBER_DELETE));
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildMemberDelete::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskGuildMemberDelete::Completer::Do()
{
	gsys.pDBManager->GuildMemberLog(m_Data);

	if (GDB_CODE::CD_L_GUILD_MEMBER_DELETE == m_Data.m_nCODE)
		gsys.pGuildSystem->ForDBTask().Leave(m_Data.m_uidPlayer, (int)m_Data.m_nGID);		
	else if (GDB_CODE::CD_L_GUILD_MEMBER_KICK_OFF == m_Data.m_nCODE)
		gsys.pGuildSystem->ForDBTask().Kick((int)m_Data.m_nCID, (int)m_Data.m_nGID);
	else
	{
		_ASSERT(0 && "INVALIED DELETE MEMBER CODE");
	}
}