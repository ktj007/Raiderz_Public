#pragma once

#include "ServerCommonUnitTestLib.h"
#include "UTestDBRecordSet.h"
#include "../MAsyncDatabase/MDatabase.h"
#include "SDBManager.h"


#include <map>
using std::map;


class DBTestSeedData;


typedef map<wstring, DBTestSeedData*> SeedDataMap;


class SCOMMON_UNITTEST_API UTestDatabase
{
public :
	UTestDatabase();
	~UTestDatabase();


	mdb::MDatabase*		GetDB();
	
	bool				Execute(TDBRecordSet& recordSet, const wchar_t* pFormat, ...);
	bool				Execute(TDBRecordSet& recordSet, int& ret, const wchar_t* pFormat, ...);
	bool				Execute(const wchar_t* pFormat, ...);
	bool				Execute(int& ret, const wchar_t* pFormat, ...);

	bool				ConnectTestDB(const mdb::MDatabaseDesc& desc);
	void				Disconnect();
	void				Seed(const wchar_t* szFileName);
	bool				IsValid();
	void				Release();
	void				WriteDBLog(mdb::MSQLSTMT& Stmt);


private:
	bool				Connect(const mdb::MDatabaseDesc& desc);
	bool				CreateTestDB(const wchar_t* szDatabaseName);
	void				DropTestDB(const wchar_t* szDatabaseName);
	bool				CheckExistUsedDBName(const wchar_t* szDBName);	
	void				SetValid();
	void				SetInvalid();

	DBTestSeedData*		GetSeedData(const wstring& strSeedDataName);
	

private :
	bool				m_bIsValid;

	mdb::MDatabase		m_DB;

	SeedDataMap			m_SeedDataCache;
};