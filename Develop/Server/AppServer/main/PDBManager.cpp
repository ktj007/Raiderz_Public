#include "stdafx.h"
#include "PDBManager.h"
#include "SDBTask.h"
#include "SAsyncDB.h"
#include "PDBTaskSQL.h"
#include "MDBUtil.h"


PDBManager::PDBManager() : m_pAccountDB(NULL), m_pGameDB(NULL), m_pLogDB(NULL)
{
}

PDBManager::~PDBManager()
{
}

bool PDBManager::ServerStatusStart( const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm )
{
	PAccountDBTaskQuery* pTask = new PAccountDBTaskQuery(SDBTID_SERVER_START);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL RZ_SERVER_START (%d, %d, '%s', '%s', '%s', %d, %d, %d, %d, %d)}"
		, nWorldID
		, nServerID
		, mdb::MDBStringEscaper::Escape(strServerName).c_str()
		, mdb::MDBStringEscaper::Escape(strServerVersion).c_str()
		, strIP.c_str()
		, nPort
		, nType
		, nMaxUser
		, nUpdateElapsedTimeSec
		, nAllowDelayTm );

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool PDBManager::ServerStatusUpdate( const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable, const unsigned long long nTaskCount, const int nCPUUsage, const int nMemoryUsage, const int nFieldCount, const int nFPS )
{
	PAccountDBTaskQuery* pTask = new PAccountDBTaskQuery(SDBTID_SERVER_UPDATE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL RZ_SERVER_UPDATE (%d, %d, %d, %s, %I64d, %d, %d, %d, %d)}", 
		nWordID, nServerID, nCurUserCount, mdb::MDBBoolUtil::ToWString(bIsServable), nTaskCount, nCPUUsage, nMemoryUsage, nFieldCount, nFPS);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool PDBManager::Init( mdb::MDatabaseDesc& dbAccountDBDesc, mdb::MDatabaseDesc& dbGameDBDesc, mdb::MDatabaseDesc& dbLogDBDesc )
{
	if (!InitAccountDB(dbAccountDBDesc))
	{
		MLog3("Init AccountDB fail.\n");
		return false;
	}

	if (!InitGameDB(dbGameDBDesc))
	{
		MLog3("Init GameDB fail.\n");
		return false;
	}

	if (!InitLogDB(dbLogDBDesc))
	{
		MLog3("Init LogDB fail.\n");
		return false;
	}
	
	return true;
}

void PDBManager::Release()
{
	ReleaseAccountDB();
	ReleaseGameDB();
	ReleaseLogDB();
}

void PDBManager::Update()
{
	if (NULL != m_pAccountDB)
		m_pAccountDB->Update();

	if (NULL != m_pGameDB)
		m_pGameDB->Update();

	if (NULL != m_pLogDB)
		m_pLogDB->Update();
}

bool PDBManager::Post( SDBAsyncTask* pTask )
{
	if (NULL == pTask)
		return false;


	SDBT_DBTYPE DBType = pTask->GetDBType();
	if (SDBT_DBTYPE_ACCOUNTDB != pTask->GetDBType() && SDBT_DBTYPE_GAMEDB != pTask->GetDBType())
	{
		SAFE_DELETE(pTask);
		return false;
	}

	SAsyncDB* pAsyncDB = NULL;
	if (SDBT_DBTYPE_ACCOUNTDB == pTask->GetDBType())
		pAsyncDB = m_pAccountDB;
	else if (SDBT_DBTYPE_GAMEDB == pTask->GetDBType())
		pAsyncDB = m_pGameDB;

	if (NULL == pAsyncDB)
	{
		SAFE_DELETE(pTask);
		return false;
	}
	else
	{
		if (!pAsyncDB->ExecuteAsync(pTask))
		{
			pTask->OnCompleted();
			SAFE_DELETE(pTask);
			return false;
		}
	}

	return true;
}

bool PDBManager::InitAccountDB(mdb::MDatabaseDesc& dbAccountDBDesc)
{
	if (NULL == m_pAccountDB)
	{
		m_pAccountDB = new SAsyncDB;
		if (NULL == m_pAccountDB)
			return false;
	}

	if (mdb::MDBAR_SUCCESS != m_pAccountDB->Init(SDBT_DBTYPE_ACCOUNTDB, dbAccountDBDesc))
		return false;

	return true;
}

void PDBManager::ReleaseAccountDB()
{
	if (NULL != m_pAccountDB)
	{
		m_pAccountDB->Release();
		SAFE_DELETE(m_pAccountDB);
	}
}

bool PDBManager::InitGameDB(mdb::MDatabaseDesc& dbGameDBDesc)
{
	if (NULL == m_pGameDB)
	{
		m_pGameDB = new SAsyncDB;
		if (NULL == m_pGameDB)
			return false;
	}

	if (mdb::MDBAR_SUCCESS != m_pGameDB->Init(SDBT_DBTYPE_GAMEDB, dbGameDBDesc))
		return false;

	return true;
}

void PDBManager::ReleaseGameDB()
{
	if (NULL != m_pGameDB)
	{
		m_pGameDB->Release();
		SAFE_DELETE(m_pGameDB);
	}
}

bool PDBManager::InitLogDB( mdb::MDatabaseDesc& dbLogDBDesc )
{
	if (NULL == m_pLogDB)
	{
		m_pLogDB = new SAsyncDB;
		if (NULL == m_pLogDB)
			return false;
	}

	if (mdb::MDBAR_SUCCESS != m_pLogDB->Init(SDBT_DBTYPE_LOGDB, dbLogDBDesc))
		return false;

	return true;
}

void PDBManager::ReleaseLogDB()
{
	if (NULL != m_pLogDB)
	{
		m_pLogDB->Release();
		SAFE_DELETE(m_pLogDB);
	}
}