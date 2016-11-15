#include "stdafx.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "MXml.h"
#include "DBTestSeedData.h"
#include "DBTestGlobal.h"
#include "MDatabaseInfo.h"
#include "MLocale.h"

UTestDatabase::UTestDatabase()
{
	SetInvalid();
}

UTestDatabase::~UTestDatabase()
{
	SetInvalid();
}

bool UTestDatabase::CreateTestDB(const wchar_t* szDatabaseName)
{
	if (CheckExistUsedDBName(szDatabaseName)) return false;

	DropTestDB(szDatabaseName);

	CString strSQL;
	
	strSQL.Format(L"CREATE DATABASE %s", szDatabaseName);
	if (!Execute(strSQL))
	{
		return false;
	}

	SetValid();
	return true;
}

void UTestDatabase::DropTestDB( const wchar_t* szDatabaseName )
{
	if (CheckExistUsedDBName(szDatabaseName)) return;

	CString strSQL;
	
	strSQL.Format(L"IF (DB_ID('%s') IS NOT NULL) DROP DATABASE %s", szDatabaseName, szDatabaseName);
	Execute(strSQL);


}

bool UTestDatabase::ConnectTestDB(const mdb::MDatabaseDesc& desc)
{
	mdb::MDatabaseDesc descForCreation = desc;
	descForCreation.strDatabaseName = L"tempdb";
	
	if (Connect(descForCreation) == false) return false;

	if (!CreateTestDB(desc.strDatabaseName.c_str()))
	{
		SetInvalid();
		return false;
	}
	Disconnect();

	if (Connect(desc) == false) return false;

	return true;
}

bool UTestDatabase::Connect(const mdb::MDatabaseDesc& desc)
{
	if (!m_DB.ConnectW(desc.BuildDSNStringW()))
		SetInvalid();

	mdb::MSQLSTMT stmt;
	if (!stmt.Init(&m_DB))
		return false;

	mdb::MDatabaseStatus st;
	if (!st.Init(stmt))
		return false;
	
	SetValid();

	if (!IsValid())
		return false;
		
	return true;
}

void UTestDatabase::Disconnect()
{
	m_DB.Disconnect();
	
	SetInvalid();
}

bool UTestDatabase::Execute( TDBRecordSet& recordSet, const wchar_t *pFormat, ... )
{
	if (!IsValid())
	{
		return false;
	}

	static wchar_t temp[8192];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	if (!m_DB.IsConnected())
	{
		if (!m_DB.Reconnect())
		{
			mlog("db reconnect fail.\n");
			return false;
		}	
	}

	int nReturn = 0;

	if (!recordSet.GetRecordSet().OpenW(&m_DB, temp, nReturn, false))
	{
		WriteDBLog(recordSet.GetRecordSet().GetSQLSTMT());
		return false;
	}

	recordSet.m_strSQL = temp;
	
	return true;
}

bool UTestDatabase::Execute( TDBRecordSet& recordSet, int& ret, const wchar_t* pFormat, ... )
{
	if (!IsValid())
	{
		return false;
	}

	static wchar_t temp[8192];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	if (!m_DB.IsConnected())
	{
		if (!m_DB.Reconnect())
		{
			mlog("db reconnect fail.\n");
			return false;
		}	
	}
	
	if (!recordSet.GetRecordSet().OpenW(&m_DB, temp, ret, true))
	{
		WriteDBLog(recordSet.GetRecordSet().GetSQLSTMT());
		return false;
	}

	recordSet.m_strSQL = temp;

	return true;
}


bool UTestDatabase::Execute( const wchar_t* pFormat, ... )
{
	if (!IsValid())
	{
		return false;
	}

	static wchar_t temp[8192];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	mdb::MDatabaseQuery q(&m_DB);
	if (!q.ExecuteW(temp))
	{
		WriteDBLog(q.GetSQLSTMT());
		return false;
	}
	
	return true;
}

bool UTestDatabase::Execute( int& ret, const wchar_t* pFormat, ... )
{
	if (!IsValid())
	{
		return false;
	}

	static wchar_t temp[8192];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	mdb::MDatabaseQuery q(&m_DB);
	if (!q.ExecuteW(temp, ret))
	{
		WriteDBLog(q.GetSQLSTMT());
		return false;
	}

	return true;
}


void UTestDatabase::Seed( const wchar_t* szFileName )
{
	wstring strFullPath = DB_UNIT_TEST_PATH;
	strFullPath = strFullPath + szFileName;

	DBTestSeedData* pSeedData = GetSeedData(strFullPath);
	if (NULL != pSeedData)
	{
		pSeedData->Flush();
	}
	else
	{
		pSeedData = new DBTestSeedData(this);
		pSeedData->Load(strFullPath.c_str());
		pSeedData->Flush();

		m_SeedDataCache.insert(SeedDataMap::value_type(strFullPath, pSeedData));
	}
}


bool UTestDatabase::CheckExistUsedDBName(const wchar_t* szDBName)
{
	for (int i = 0; i < USED_DB_NAME_COUNT; i++)
	{
		if (!_wcsicmp(szDBName, USED_DB_NAME[i].c_str())) return true;
	}
	return false;
}


bool UTestDatabase::IsValid()							
{ 
	return m_bIsValid; 
}


void UTestDatabase::SetValid()							
{ 
	m_bIsValid = true; 
}


void UTestDatabase::SetInvalid()						
{ 
	m_bIsValid = false; 
} 

mdb::MDatabase* UTestDatabase::GetDB()
{
	return &m_DB; 
}


DBTestSeedData* UTestDatabase::GetSeedData(const wstring& strSeedDataName)
{
	SeedDataMap::iterator it = m_SeedDataCache.find(strSeedDataName);
	if (m_SeedDataCache.end() == it)
	{
		return NULL;
	}

	return it->second;
}


void UTestDatabase::Release()
{
	for (SeedDataMap::iterator it  = m_SeedDataCache.begin()
		; it != m_SeedDataCache.end()
		; ++it)
	{
		delete it->second;
	}

	m_SeedDataCache.clear();
}

void UTestDatabase::WriteDBLog( mdb::MSQLSTMT& Stmt )
{
	mdb::MDatabaseStatus st;
	if (!st.Init(Stmt))
	{
		mlog("TDBManager::Execute error.\n");
		return;
	}

	char szError[8192];

	mdb::MDB_STATE_VEC& vState =  st.GetStateVec();
	for (size_t i = 0; i < vState.size(); ++i)
	{
		_snprintf_s(szError, 8192, 8191, "  DB ERROR : NUM(%d), %s\n"
			, i
			, vState[i].ToString().c_str());

		mlog(szError);
	}	
}