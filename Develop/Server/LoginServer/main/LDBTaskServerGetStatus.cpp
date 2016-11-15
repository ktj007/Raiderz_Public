#include "StdAfx.h"
#include "LDBTaskServerGetStatus.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LServableChecker.h"

LDBTaskServerGetStatus::LDBTaskServerGetStatus()
: LDBAsyncTask(SDBT_DBTYPE_GAMEDB, SDBTID_SERVERGETSTATUS)
{
}

LDBTaskServerGetStatus::~LDBTaskServerGetStatus()
{
}


void LDBTaskServerGetStatus::AssignServerID(const std::vector<int>& vecServerID)
{
	m_vecServerID = vecServerID;
}

void LDBTaskServerGetStatus::OnExecute(mdb::MDatabase& rfDB)
{
	for (size_t i = 0; i < GetQuerySize(); ++i)
	{
		if (!GetServerStatusInfo(rfDB, i))
			return;
	}
}

bool LDBTaskServerGetStatus::GetServerStatusInfo(mdb::MDatabase& rfDB, int nSQLIndex)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(nSQLIndex)))
		return false;

	if (0 == rs.GetFetchedCount())
		return false;

	if ((size_t)nSQLIndex >= m_vecServerID.size()) return false;

	const int nServerID		= m_vecServerID[nSQLIndex];
	const bool bIsRunning	= rs.FieldW(L"IS_RUN").AsBool();
	const bool bServable	= rs.FieldW(L"SERVABLE").AsBool();

	_SERVER_STATUS_INFO serverInfo;
	serverInfo.nServerID	= nServerID;
	serverInfo.bIsRunning	= bIsRunning;
	serverInfo.bServable	= bServable;

	m_Data.vecServerStatusInfo.push_back(serverInfo);

	rs.Close();

	return true;
}

mdb::MDB_THRTASK_RESULT LDBTaskServerGetStatus::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

LDBTaskServerGetStatus::Completer::Completer( _DATA& Data ) : m_Data(Data)
{

}

void LDBTaskServerGetStatus::Completer::Do()
{
	for (int i = 0; i < (int)m_Data.vecServerStatusInfo.size(); ++i)
	{
		gsys.pServer->GetServableChecker().UpdateServerStatusInfoByDB(
			m_Data.vecServerStatusInfo[i].nServerID,
			m_Data.vecServerStatusInfo[i].bIsRunning,
			m_Data.vecServerStatusInfo[i].bServable);
	}
}
