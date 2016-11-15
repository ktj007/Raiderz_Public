#include "stdafx.h"
#include "GDBTaskGuildInsert.h"
#include "GDBTaskGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForDBTask.h"
#include "GDBManager.h"
#include "GTimeCalculator.h"
#include "GDB_CODE.h"


GDBTaskGuildInsert::GDBTaskGuildInsert( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_GUILDINSERT)
{

}

GDBTaskGuildInsert::~GDBTaskGuildInsert()
{

}

mdb::MDB_THRTASK_RESULT GDBTaskGuildInsert::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void GDBTaskGuildInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(GUILD_INSERT)))
		return;
	
	if (0 == rs.GetFetchedRowCount())
	{
		SetTaskFail();
		return;
	}

	m_Data.m_nGID = rs.FieldW(L"GUILD_ID").AsInt64();

	rs.Close();
}

void GDBTaskGuildInsert::Input( GDBT_GUILD& data )
{
	m_Data = data;
}

void GDBTaskGuildInsert::Completer::Do()
{
	if (IsVaildGID())
	{
		gsys.pDBManager->GuildLog(m_Data);
		
		gsys.pDBManager->CharUpdateMoneyLog(GLOG_DATA_MONEY(m_Data.m_nAID, m_Data.m_nCID, GDB_CODE::CD_L_MONEY_GUILD_CREATE, m_Data.m_nCharPtm, m_Data.m_nDeltaCharPtm, m_Data.m_nLevel
			, m_Data.m_nMoney, -m_Data.m_nDeltaMoney, GTimeCalculator::GetTimeAsString(GTimeSec(0)), 0));

		InsertGuild();		
	}
	else
	{
		ErrorGuildInsert();
	}
}

bool GDBTaskGuildInsert::Completer::IsVaildGID()
{
	return 0 < m_Data.m_nGID;
}

void GDBTaskGuildInsert::Completer::ErrorGuildInsert()
{
	gsys.pGuildSystem->RouteInsertGuildFailed(m_Data.m_uidPlayer, (int)m_Data.m_nGID);
}

void GDBTaskGuildInsert::Completer::InsertGuild()
{
	gsys.pGuildSystem->ForDBTask().InsertGuild(m_Data);
}