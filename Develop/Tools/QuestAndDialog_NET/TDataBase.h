#ifndef _TDATABASE_H
#define _TDATABASE_H

#include <string>

using std::string;

typedef void(LOGCALLBACK)( const string& strLog );

using namespace System::Data::OleDb;
using namespace System::Data;

class TDatabase
{
public:
	TDatabase(void);
	~TDatabase(void);

	bool CheckOpen();
	String^ BuildDSNString(String^ strDSN, String^ strUserName, String^ strPassword);
	bool Connect(String^ strDSNConnect);
	void Disconnect();
	bool IsOpen();
	void ExecuteSQL(String^ strSQL);
	void ExecuteSQL(String^ strSQL, DataSet^& outDs, String^ strTable);
	// thread unsafe
	void SetLogCallback(LOGCALLBACK* fnLogCallback) { m_fnLogCallback = fnLogCallback; }

	bool Begin();
	void End();

private :
	void WriteLog( const string& strLog );

private :
	string				m_strDSNConnect;
	LOGCALLBACK*		m_fnLogCallback;
};


#endif //_TDATABASE_H
