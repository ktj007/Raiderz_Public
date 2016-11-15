#include "StdAfx.h"
#include "ZDBManager.h"
#include "SAsyncDB.h"
#include "ZDBTaskSQL.h"

ZDBManager::ZDBManager() : m_pAccountDB(NULL), m_pGameDB(NULL), m_pLogDB(NULL)
{
}

ZDBManager::~ZDBManager()
{
}

bool ZDBManager::ServerStatusStart( const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm )
{
	LGameDBTaskQuery* pTask = new LGameDBTaskQuery(SDBTID_SERVER_START);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_SERVER_START (%d, %d, '%s', '%s', '%s', %d, %d, %d, %d, %d)}"
		, nWorldID, nServerID, MCleanSQLStr(strServerName).c_str(), MCleanSQLStr(strServerVersion).c_str(), MCleanSQLStr(strIP).c_str()
		, nPort, nType, nMaxUser, nUpdateElapsedTimeSec, nAllowDelayTm );

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool ZDBManager::ServerStatusUpdate( const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable )
{
	LGameDBTaskQuery* pTask = new LGameDBTaskQuery(SDBTID_SERVER_UPDATE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL USP_RZ_SERVER_UPDATE (%d, %d, %d, %d)}", nWordID, nServerID, nCurUserCount, bIsServable);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool ZDBManager::Init(mdb::MDatabaseDesc& dbAccountDBDesc, mdb::MDatabaseDesc& dbGameDBDesc, mdb::MDatabaseDesc& dbLogDBDesc)
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

void ZDBManager::Release()
{
	ReleaseAccountDB();
	ReleaseGameDB();
	ReleaseLogDB();
}

void ZDBManager::Update()
{
	if (NULL != m_pAccountDB)	m_pAccountDB->Update();
	if (NULL != m_pGameDB)		m_pGameDB->Update();
	if (NULL != m_pLogDB)		m_pLogDB->Update();
}

bool ZDBManager::Post( SDBAsyncTask* pTask )
{
	if (NULL == pTask)
		return false;

	SDBT_DBTYPE DBType = pTask->GetDBType();
	if (SDBT_DBTYPE_ACCOUNTDB != DBType && SDBT_DBTYPE_GAMEDB != DBType && SDBT_DBTYPE_LOGDB != DBType)
	{
		SAFE_DELETE(pTask);
		return false;
	}

	SAsyncDB* pAsyncDB = NULL;
	if (SDBT_DBTYPE_ACCOUNTDB == DBType)
		pAsyncDB = m_pAccountDB;
	else if (SDBT_DBTYPE_GAMEDB == DBType)
		pAsyncDB = m_pGameDB;
	else if (SDBT_DBTYPE_LOGDB == DBType)
		pAsyncDB = m_pLogDB;
	else
	{
		SAFE_DELETE(pTask);
		return false;
	}

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

bool ZDBManager::InitAccountDB(mdb::MDatabaseDesc& dbAccountDBDesc)
{
	if (NULL == m_pAccountDB)
	{
		m_pAccountDB = new SAsyncDB;
		if (NULL == m_pAccountDB)
		{
			return false;
		}
	}

	if (mdb::MDBAR_SUCCESS != m_pAccountDB->Init(SDBT_DBTYPE_ACCOUNTDB, dbAccountDBDesc))
	{
		return false;
	}

	return true;
}

void ZDBManager::ReleaseAccountDB()
{
	if (NULL != m_pAccountDB)
	{
		m_pAccountDB->Release();
		SAFE_DELETE(m_pAccountDB);
	}
}

bool ZDBManager::InitGameDB( mdb::MDatabaseDesc& dbGameDBDesc )
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

void ZDBManager::ReleaseGameDB()
{
	if (NULL != m_pGameDB)
	{
		m_pGameDB->Release();
		SAFE_DELETE(m_pGameDB);
	}
}

bool ZDBManager::InitLogDB( mdb::MDatabaseDesc& dbLogDBDesc )
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

void ZDBManager::ReleaseLogDB()
{
	if (NULL != m_pLogDB)
	{
		m_pLogDB->Release();
		SAFE_DELETE(m_pLogDB);
	}
}

static const wchar_t g_szDB_WORLD_INSERT[] = L"{CALL dbo.USP_RZ_WORLD_INSERT(%d, '%s', '%s', %d, %d, %d, %d, %d)};";
bool ZDBManager::WorldInsert(const ZWorldContext& wc)
{
	LAccountDBTaskQuery* pTask = new LAccountDBTaskQuery(SDBTID_WORLD_INSERT);
	if (pTask == NULL)	return false;


	CString strSQL;
	strSQL.Format(g_szDB_WORLD_INSERT, 	wc.nID,
										MCleanSQLStr(wc.strName).c_str(),
										MCleanSQLStr(wc.strIP).c_str(),
										wc.nType,
										wc.nMaxPlayerCount,
										wc.nServable, 
										wc.nState, 
										wc.nMaxElapsedTime
										);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

static const wchar_t g_szDB_WORLD_UPDATE[] = L"{CALL dbo.USP_RZ_WORLD_UPDATE(%d, %d, %d)}";
bool ZDBManager::WorldUpdate(int nWorldID, int nCurPlayerCount, bool isServable)
{
	LAccountDBTaskQuery* pTask = new LAccountDBTaskQuery(SDBTID_WORLD_UPDATE);
	if (pTask == NULL)	return false;


	CString strSQL;
	strSQL.Format(g_szDB_WORLD_UPDATE, nWorldID, nCurPlayerCount, isServable);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}
