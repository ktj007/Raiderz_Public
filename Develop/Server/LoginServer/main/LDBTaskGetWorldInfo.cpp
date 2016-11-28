#include "stdafx.h"
#include "LDBTaskGetWorldInfo.h"
#include "LWorldLocatorServer.h"
#include "LGlobal.h"
#include "LServer.h"

using namespace mdb;

LDBTaskGetWorldInfo::LDBTaskGetWorldInfo()
: LDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, SDBTID_WORLD_GET_LIST)
{
	// do nothing
}

LDBTaskGetWorldInfo::~LDBTaskGetWorldInfo()
{
	// do nothing
}

void LDBTaskGetWorldInfo::OnExecute(MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(GET_WORLD_INFO)))
	{
		return;
	}


	if (0 == rs.GetFetchedRowCount())
	{
		return;
	}


	while(!rs.IsEOF())
	{
		DBTASK_WORLD_INFO data;

		data.nID = rs.FieldW(L"WORLD_ID").AsInt();
		data.strName = rs.FieldW(L"NAME").AsWString();
		data.strIP = rs.FieldW(L"IP").AsWString();
		data.nType = rs.FieldW(L"TYPE").AsInt();
		data.nCurrentPlayerCount = rs.FieldW(L"CUR_PLAYER_COUNT").AsInt();
		data.nMaxPlayerCount = rs.FieldW(L"MAX_PLAYER_COUNT").AsInt();
		data.isServable = rs.FieldW(L"SERVABLE").AsBool();
		data.isExpire = rs.FieldW(L"EXPIRE").AsBool();
		data.strState = rs.FieldW(L"STATE").AsWString();
		data.nOrderNum = rs.FieldW(L"ORD_NUM").AsInt();

		m_vecWorldInfo.push_back(data);

		rs.MoveNext();
	}


	SetTaskSuccess();
}

MDB_THRTASK_RESULT LDBTaskGetWorldInfo::_OnCompleted()
{
	LWorldLocatorServer* pWorldLocatorServer = gsys.pServer->GetWorldLocatorServer();
	if (pWorldLocatorServer == NULL)	return MDBTR_SUCESS;


	vector<CSWorldInfo> vecWorldInfo;
	for each(const DBTASK_WORLD_INFO& dbInfo in m_vecWorldInfo)
	{
		if (dbInfo.isExpire)	continue;
		
		CSWorldInfo csInfo;

		csInfo.nID = dbInfo.nID;
		csInfo.strName = dbInfo.strName;
		csInfo.strIP = dbInfo.strIP;
		csInfo.nCurrentPlayerCount = dbInfo.nCurrentPlayerCount;
		csInfo.nMaxPlayerCount = dbInfo.nMaxPlayerCount;
		csInfo.nType = dbInfo.nType;
		csInfo.nOrderNum = dbInfo.nOrderNum;

		/*
		if (dbInfo.strState == L"MAINTENANCE")				// Ongoing Maintenance
			csInfo.strName += L" (Maintenance)";
		else if (dbInfo.isServable && dbInfo.isExpire)		// Server is unavailable due to errors
			csInfo.strName += L" (Down)";
		else if (!dbInfo.isServable && !dbInfo.isExpire)	// Server is starting up
			csInfo.strName += L" (Starting)";
		else if (!dbInfo.isServable)						// Server is not running
			csInfo.strName += L" (Inactive)";
		*/

		vecWorldInfo.push_back(csInfo);
	}

	pWorldLocatorServer->SetWorldInfo(vecWorldInfo);


	return MDBTR_SUCESS;
}
