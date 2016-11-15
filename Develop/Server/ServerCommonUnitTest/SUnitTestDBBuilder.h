#ifndef _SUNITTEST_DB_BUILDER_H
#define _SUNITTEST_DB_BUILDER_H


#include "UTestDatabaseContainer.h"
#include "SUnitTest.h"
#include "../../MAsyncDatabase/MDatabaseDesc.h"

class UTestDatabase;


class SUnitTestDBBuilder
{
public :
	SUnitTestDBBuilder();
	~SUnitTestDBBuilder();

	bool						LoadDropTableOrder(const wstring& strFileName);
	bool						LoadTableScript(const wstring& strFielName);
	bool						LoadProcScript(const wstring& strFielName);
	bool						LoadFuncScript(const wstring& strFielName);
	bool						DropTable();
	void						MakeTableSQL();
	const mdb::MDatabaseDesc	GetDesc();
	UTestDatabase&				GetUTestDB();
	void						SetDesc(const string& strODBCDriver, const string& strServer, const string& strDB, const string& strUser, const string& strPasswd);
	void						CreateDBContainer();
	void						ReleaseDBContainer();
	void						CreateTestDB();
	void						ReleaseDB();
	void						DroDatabase();

private :
	wstring						GetMyComputerName();
	bool						LoadScript(vector<wstring>& vecSQL, const wstring& strFielName);
	bool						ScriptFlush(vector<wstring>& vecSQL);
	bool						SQLFlush(const wstring& strSQL);
	
private :
	UTestDatabaseContainer		m_UTestDBContainer;
	mdb::MDatabaseDesc			m_DatabaseDesc;
	wstring						m_strDropTable;
	vector<wstring>				m_vecTable;
	wstring						m_strTable;
	vector<wstring>				m_vecProc;
	vector<wstring>				m_vecFunc;
	bool						m_bWasCreateDB;
};


#endif