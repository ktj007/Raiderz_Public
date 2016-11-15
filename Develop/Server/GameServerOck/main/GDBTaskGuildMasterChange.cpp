#include "stdafx.h"
#include "GDBTaskGuildMasterChange.h"
#include "GDBTaskGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"
#include "GDBManager.h"


GDBTaskGuildMasterChange::GDBTaskGuildMasterChange( const MUID& uidCurMaster ) 
: GDBAsyncTask(uidCurMaster, SDBT_DBTYPE_GAMEDB, SDBTID_GUILDMASTERCHANGE)
{
	
}

GDBTaskGuildMasterChange::~GDBTaskGuildMasterChange()
{

}

void GDBTaskGuildMasterChange::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(GUILD_MASTER_CHANGE));
}

mdb::MDB_THRTASK_RESULT GDBTaskGuildMasterChange::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskGuildMasterChange::Input( GDBT_GUILD_CHANGE_MASTER& data )
{
	m_Data = data;

	IncreaseRefCount(m_Data.m_NewMaster.m_uidPlayer);
}

void GDBTaskGuildMasterChange::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	DecreaseRefCount(m_Data.m_NewMaster.m_uidPlayer);
}

void GDBTaskGuildMasterChange::Completer::Do()
{
	gsys.pDBManager->GuildMemberLog(m_Data.m_NewMaster);
	gsys.pGuildSystem->ForDBTask().ChangeMaster((int)m_Data.m_NewMaster.m_nGID, m_Data.m_uidPreMaster, (int)m_Data.m_NewMaster.m_nCID);
}