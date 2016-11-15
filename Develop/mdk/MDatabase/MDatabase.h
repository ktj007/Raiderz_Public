#pragma once

#pragma push_macro ("new")
#ifdef new
#undef new
#endif

// malloc
// calloc
// realloc
// _expand
// free
// _msize

#include "ODBCRecordset.h"
#include <string>

using std::string;

typedef void(LOGCALLBACK)( const string& strLog );

struct MDatabaseDesc
{
	string strServer;
	string strDatabaseName;
	string strUserName;
	string strPassword;
	MDatabaseDesc() {}
	MDatabaseDesc(string server, string database_name, string user_name, string password) 
		: strServer(server), strDatabaseName(database_name), strUserName(user_name), strPassword(password)	{}

	string MDatabaseDesc::BuildDSNString()
	{
		// SQL2005 : "Driver={SQL Native Client};Server=myServerAddress;Database=myDataBase;Uid=myUsername;Pwd=myPassword;"
		// Âü°í : http://www.connectionstrings.com/

		std::string strDSNConnect = std::string("Driver={SQL Native Client};Server=") + strServer + 
			std::string(";Database=") + strDatabaseName +
			std::string(";Uid=") + strUserName +
			std::string(";Pwd=") + strPassword +
			std::string(";");

		return strDSNConnect;
	}
};

class MDatabase
{
private :
	MDatabaseDesc	m_Desc;
	CDatabase		m_DB;
	DWORD			m_dwOptions;
	LOGCALLBACK*	m_fnLogCallback;
private :
	void WriteLog( const string& strLog );
	std::string BuildDSNString(const std::string strServer, const std::string strDatabaseName, const std::string strUserName, const std::string strPassword);
public:
	MDatabase(void);
	~MDatabase(void);

	CDatabase* GetDatabase()	{ return &m_DB; }

	bool CheckOpen();
	bool Connect(MDatabaseDesc& desc);
	void Disconnect();
	BOOL IsOpen() const;
	BOOL BeginTrans();
	BOOL CommitTrans();
	BOOL Rollback();
	void ExecuteSQL(LPCTSTR lpszSQL);
	// thread unsafe
	void SetLogCallback( LOGCALLBACK* fnLogCallback ) { m_fnLogCallback = fnLogCallback; }
};


#pragma pop_macro("new")