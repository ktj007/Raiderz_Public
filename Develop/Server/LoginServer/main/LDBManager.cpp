#include "stdafx.h"
#include "LDBManager.h"
#include "CCommandResultTable.h"
#include "MDebug.h"
#include "LDBTask.h"
#include "LDBTaskHeaderList.h"
#include "MDBRecordSet.h"
#include "MDatabaseDesc.h"
#include "SAsyncDB.h"
#include "SDsnFactory.h"
#include "SBaseDsnFactory.h"
#include "LDBParamData.h"
#include "LConfig.h"

// 계정 - 기본 정보 읽기
// 캐릭터
// 아이템
// 퀘스트

LDBManager::LDBManager() : m_pAccountDB(NULL), m_pGameDB(NULL), m_pLogDB(NULL)
{
	// do nothing
}

LDBManager::~LDBManager()
{
	// do nothing
}

void LDBManager::Update()
{
	if (NULL != m_pAccountDB)	m_pAccountDB->Update();
	if (NULL != m_pGameDB)		m_pGameDB->Update();
	if (NULL != m_pLogDB)		m_pLogDB->Update();	
}

void LDBManager::Release()
{
	ReleaseAccountDB();
	ReleaseGameDB();
	ReleaseLogDB();
}

mdb::MDB_AYSNC_RESULT LDBManager::InitAsyncDB()
{
	if (!InitAccountDB())
		return mdb::MDBAR_DBCONN_ERR;

	if (!InitGameDB())
		return mdb::MDBAR_DBCONN_ERR;

	if (!InitLogDB())
		return mdb::MDBAR_DBCONN_ERR;

	RegistTraceTaskID();	

	return mdb::MDBAR_SUCCESS;
}


// 서버군의 모든 서버들 상태 정보 받아오기
wchar_t g_szDB_SERVER_GET_STATUS_INFO[] = L"{CALL dbo.USP_RZ_SERVER_GET_STATUS_INFO (%d, %d)}";
bool LDBManager::GetServerStatusList(const int nWorldID, const std::vector<int>& vecServerID)
{
	LDBTaskServerGetStatus* pTask = new LDBTaskServerGetStatus();
	if (NULL == pTask)
		return false;

	pTask->AssignServerID(vecServerID);

	CString strSQL;	
	for (int i = 0; i < (int)vecServerID.size(); ++i)
	{
		strSQL.Format(g_szDB_SERVER_GET_STATUS_INFO, nWorldID, vecServerID[i]);
		pTask->PushSQL(strSQL);
	}

	return Post(pTask);
}


// 계정 로그인 정보 받아오기
wchar_t g_szDB_LOGIN_GETINFO[] = L"{CALL dbo.USP_RZ_LOGIN_GET_INFO (N'%s', N'%s')}";
bool LDBManager::LoginGetInfo(LDBT_ACC_LOGIN& data)
{
	LDBTaskLogin* pTask = new LDBTaskLogin(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_LOGIN_GETINFO, MCleanSQLStr(data.m_strUSER_ID).c_str(), MCleanSQLStr(data.m_strPWD).c_str());

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 계정 로그인 정보 받아오기
wchar_t g_szDB_PMANG_LOGIN_GETINFO[] = L"{CALL dbo.USP_RZ_LOGIN_GET_INFO (N'%d', N'%s')}";
bool LDBManager::PmangLoginGetInfo(const MUID& uidPlayer, const wstring& strUserID, const PmUserData& pmangUserData)
{
	LDBTaskPmangLogin* pTask = new LDBTaskPmangLogin(uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_PMANG_LOGIN_GETINFO, pmangUserData.nUserType , MCleanSQLStr(pmangUserData.strUserSerial).c_str());

	pTask->Input(strUserID, pmangUserData);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


// 계정 추가 - UserID, Name
wchar_t g_szDB_ACC_INSERT[] = L"{CALL USP_RZ_ACCOUNT_INSERT (N'%s', N'%s', N'%s', N'%s')}";
bool LDBManager::InsertAccount(LDBT_ACC_INSERT& data)
{
	LDBTaskAccountInsert* pTask = new LDBTaskAccountInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_ACC_INSERT, MCleanSQLStr(data.m_strUSER_ID).c_str(), MCleanSQLStr(data.m_strSITE_CODE).c_str(), MCleanSQLStr(data.m_strSITE_USER_ID).c_str(), MCleanSQLStr(data.m_strPWD).c_str());

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 피망 계정 추가 - UserID, Name
wchar_t g_szDB_PMANG_ACC_INSERT[] = L"{CALL USP_RZ_ACCOUNT_INSERT (N'%s', N'%d', N'%s', N'%s')}";
bool LDBManager::PmangInsertAccount(const MUID& uidPlayer, const PmUserData& pmangUserData)
{
	LDBTaskPmangAccountInsert* pTask = new LDBTaskPmangAccountInsert(uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_PMANG_ACC_INSERT, MCleanSQLStr(pmangUserData.strUserID).c_str(), pmangUserData.nUserType, MCleanSQLStr(pmangUserData.strUserSerial).c_str(), L"");

	pTask->Input(pmangUserData.strUserID, L"", pmangUserData);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

static const wchar_t g_szDB_CHAR_GET_SIMPLE_INFO[] = L"{CALL dbo.USP_RZ_CHAR_GET_SIMPLE_INFO (%d, %I64d)}";
static const wchar_t g_szDB_CHAR_GET_EQUIPMENT_INFO[] = L"{CALL dbo.USP_RZ_CHAR_GET_EQUIPMENT_INFO (%d, %I64d)}";
bool LDBManager::GetAccountCharList(const MUID& uidPlayer, const int64 nAID)
{
	LDBTaskCharGetLookList* pTask = new LDBTaskCharGetLookList();
	if (NULL == pTask)
		return false;
	
	CString strSQLCharGetSimpleInfo;
	CString strSQLCharGetEquipmentInfo;

	strSQLCharGetSimpleInfo.Format(g_szDB_CHAR_GET_SIMPLE_INFO, LConfig::m_nWorldID, nAID);
	strSQLCharGetEquipmentInfo.Format(g_szDB_CHAR_GET_EQUIPMENT_INFO, LConfig::m_nWorldID, nAID);	

	pTask->Input(uidPlayer);
	
	pTask->PushSQL(strSQLCharGetSimpleInfo)
		.PushSQL(strSQLCharGetEquipmentInfo);	

	return Post(pTask);
}

wchar_t g_szDB_CHAR_DELETE[] = L"{CALL dbo.USP_RZ_CHAR_DELETE (%d, %I64d, %I64d)}";
// 캐릭터 삭제
bool LDBManager::DeleteCharacter(const MUID& uidPlayer, const int64 nAID, const int64 nCID, const int nIndex)
{
	LDBTaskCharDelete* pTask = new LDBTaskCharDelete();
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_CHAR_DELETE, LConfig::m_nWorldID, nAID, nCID);

	pTask->Input(uidPlayer, nIndex);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

wchar_t g_szDB_CHAR_INSERT[] = L"{CALL dbo.USP_RZ_CHAR_INSERT (%d, %I64d, N'%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}";
bool LDBManager::InsertCharacter(const DBP_CHAR_INSERT& rfParam)
{
	LDBTaskCharInsert* pTask = new LDBTaskCharInsert();
	if (NULL == pTask)
		return false;

	CString strSQLInsertCharacter;
	strSQLInsertCharacter.Format(g_szDB_CHAR_INSERT
		, LConfig::m_nWorldID
		, rfParam.nAID
		, MCleanSQLStr(rfParam.szName).c_str()
		, rfParam.nLevel
		, rfParam.nXP
		, rfParam.nSilver
		, rfParam.nRace
		, rfParam.nSex
		, rfParam.nFeatureHair
		, rfParam.nFeatureFace
		, rfParam.nFeatureHairColor
		, rfParam.nFeatureSkinColor
		, rfParam.nEyeColor
		, rfParam.nTattooType
		, rfParam.nTattooPosX
		, rfParam.nTattooPosY
		, rfParam.nTattooScale
		, rfParam.nMakeUp
		, rfParam.nTotalTP
		, rfParam.nHP
		, rfParam.nEN
		, rfParam.nSTA
		, rfParam.nFatigue
		, rfParam.nSoulBindingID
		, rfParam.nInnRoomID
		, rfParam.nPrimaryPalettedID
		, rfParam.nSecondaryPalettedID
		, rfParam.nTalentStyle
		, rfParam.nEquipmentIndex
		, rfParam.nEquipmentColorIndex);

	pTask->Input(rfParam.uidPlayer, rfParam.nAID);
	pTask->PushSQL(strSQLInsertCharacter);

	return Post(pTask);
}

bool LDBManager::InitAccountDB()
{
	_ASSERT(NULL == m_pAccountDB);
	
	if (NULL != m_pAccountDB)
		return false;

	m_pAccountDB = new SAsyncDB;
	if (NULL == m_pAccountDB)
		return false;

	mdb::MDatabaseDesc DBDesc = SDsnFactory::GetInstance().Get()->GetAccountDSN();

	const mdb::MDB_AYSNC_RESULT AsyncDBRes = m_pAccountDB->Init(SDBT_DBTYPE_ACCOUNTDB, DBDesc);		
	if (mdb::MDBAR_SUCCESS != AsyncDBRes)
	{
		mlog3 ("Create Async AccountDB failed(errno %d).\n", AsyncDBRes);
		return false;
	}

	mlog ("Create Async AccountDB success.\n");

	return true;
}

bool LDBManager::InitGameDB()
{
	_ASSERT(NULL == m_pGameDB);

	if (NULL != m_pGameDB)
		return false;

	m_pGameDB = new SAsyncDB;
	if (NULL == m_pGameDB)
		return false;

	mdb::MDatabaseDesc DBDesc = SDsnFactory::GetInstance().Get()->GetGameDSN();

	const mdb::MDB_AYSNC_RESULT AsyncDBRes = m_pGameDB->Init(SDBT_DBTYPE_GAMEDB, DBDesc);		
	if (mdb::MDBAR_SUCCESS != AsyncDBRes)
	{
		mlog3 ("Create Async GameDB failed(errno %d).\n", AsyncDBRes);
		return false;
	}

	mlog ("Create Async GameDB success.\n");

	return true;
}

bool LDBManager::InitLogDB()
{
	_ASSERT(NULL == m_pLogDB);

	if (NULL != m_pLogDB)
		return false;

	m_pLogDB = new SAsyncDB;
	if (NULL == m_pLogDB)
		return false;

	mdb::MDatabaseDesc DBDesc = SDsnFactory::GetInstance().Get()->GetLogDSN();

	const mdb::MDB_AYSNC_RESULT AsyncDBRes = m_pLogDB->Init(SDBT_DBTYPE_LOGDB, DBDesc);		
	if (mdb::MDBAR_SUCCESS != AsyncDBRes)
	{
		mlog3 ("Create Async LogDB failed(errno %d).\n", AsyncDBRes);
		return false;
	}

	mlog ("Create Async LogDB success.\n");

	return true;
}

void LDBManager::ReleaseAccountDB()
{
	if (NULL == m_pAccountDB)
		return;

	m_pAccountDB->Release();
	SAFE_DELETE(m_pAccountDB);
}

void LDBManager::ReleaseGameDB()
{
	if (NULL == m_pGameDB)
		return;

	m_pGameDB->Release();
	SAFE_DELETE(m_pGameDB);
}

void LDBManager::ReleaseLogDB()
{
	if (NULL == m_pLogDB)
		return;

	m_pLogDB->Release();
	SAFE_DELETE(m_pLogDB);
}

bool LDBManager::Post( LDBAsyncTask* pTask )
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

bool LDBManager::ServerStatusStart( const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm )
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

bool LDBManager::ServerStatusUpdate( const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable )
{
	LGameDBTaskQuery* pTask = new LGameDBTaskQuery(SDBTID_SERVER_UPDATE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL USP_RZ_SERVER_UPDATE (%d, %d, %d, %d)}", nWordID, nServerID, nCurUserCount, bIsServable);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void LDBManager::UpdaateCreateDt( const int64 nAID )
{
	LAccountDBTaskQuery* pTask = new LAccountDBTaskQuery(SDBTID_ACC_UPDATE_CREATE_DT);
	if (NULL == pTask)
		return;

	CString strSQl;
	strSQl.Format(L"{CALL USP_RZ_ACCOUNT_UPDATE_REG_DATE (%I64d)}", nAID);

	pTask->PushSQL(strSQl);

	Post(pTask);
}

wchar_t g_szDB_WORLD_GET_LIST[] = L"{CALL dbo.USP_RZ_WORLD_GET_LIST}";
bool LDBManager::WorldGetList(void)
{
	LDBTaskGetWorldInfo* pTask = new LDBTaskGetWorldInfo();
	if (pTask == NULL)	return false;


	CString strSQL;
	strSQL.Format(g_szDB_WORLD_GET_LIST);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool LDBManager::ConnectLog( LDBT_CONN_LOG& data )
{
	LLogDBTaskQuery* pTask = new LLogDBTaskQuery(SDBTID_CONN_LOG);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_CONN_LOG (%I64d, %d, '%s')}"
		, data.m_nAID, LConfig::m_nWorldID, MCleanSQLStr(data.m_strIP).c_str());

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void LDBManager::RegistTraceTaskID()
{
	SDBAsyncTask::GetTaskTracer().PushTraceTaskID(SDBTID_CONN_LOG);
}
