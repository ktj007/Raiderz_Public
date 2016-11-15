#include "stdafx.h"
#include "PDBManager.h"
#include "SDBTask.h"
#include "SAsyncDB.h"
#include "PDBTaskSQL.h"


PDBManager::PDBManager() : m_pGameDB(NULL), m_pLogDB(NULL)
{
}

PDBManager::~PDBManager()
{
}

bool PDBManager::ServerStatusStart( const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm )
{
	PGameDBTaskQuery* pTask = new PGameDBTaskQuery(SDBTID_SERVER_START);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_SERVER_START (%d, %d, '%s', '%s', '%s', %d, %d, %d, %d, %d)}"
		, nWorldID, nServerID, MCleanSQLStr(strServerName).c_str(), MCleanSQLStr(strServerVersion).c_str(), MCleanSQLStr(strIP).c_str()
		, nPort, nType, nMaxUser, nUpdateElapsedTimeSec, nAllowDelayTm );

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool PDBManager::ServerStatusUpdate( const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable )
{
	PGameDBTaskQuery* pTask = new PGameDBTaskQuery(SDBTID_SERVER_UPDATE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL USP_RZ_SERVER_UPDATE (%d, %d, %d, %d)}", nWordID, nServerID, nCurUserCount, bIsServable);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool PDBManager::Init( mdb::MDatabaseDesc& dbGameDBDesc, mdb::MDatabaseDesc& dbLogDBDesc )
{
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
	ReleaseGameDB();
	ReleaseLogDB();
}

void PDBManager::Update()
{
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
	if (SDBT_DBTYPE_GAMEDB != pTask->GetDBType())
	{
		SAFE_DELETE(pTask);
		return false;
	}

	SAsyncDB* pAsyncDB = NULL;
	if (SDBT_DBTYPE_GAMEDB == pTask->GetDBType())
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