/**
 *	@brief	CLI 코드로 작성된 라이브러리 입니다.
 *			본 라이브러리는 CLR 프로젝트에서 정상 작동 합니다.
 *			내부에서 2개의 스레드가 생성되어 Execute 동작을 수행합니다.
*/

#pragma once

#include <map>
#include <list>
#include "MRecordset.h"

using namespace std;
using namespace System::Data;

const int MAX_CONNECTION_STRING_LENGTH	= 512;
const int MAX_QUERY_STRING_LENGTH		= 128;
const int MAX_THREAD_COUNT				= 16;
const int MAX_ERROR_STRING_LENGTH		= 512;

struct MINPUTQUERY
{
	unsigned int nJobNumber;
	char szQuery[MAX_QUERY_STRING_LENGTH];
};
typedef list<MINPUTQUERY*> InputQueryList;

typedef void (*fnCompletionCallback)(unsigned int nJobNumber, MRecordset* pRs, char* szError);

class MADODatabase
{
public:
	MADODatabase();
	virtual ~MADODatabase();

	bool InitADODatabase(const char* szDBServerIP, const char* szDBName, const char* szUserID, const char* szUserPWD, fnCompletionCallback fnCallback);
	void SetConnectionPoolSize( unsigned int nPoolSize );
	void ReleaseADODatabase();
	void PushQuery(const char* szQuery, unsigned int nJobNumber);

	int	GetQueryCount()			{ return (int)m_listInputQuery.size(); }

protected:

private:
	char m_szConnection[MAX_CONNECTION_STRING_LENGTH];
	unsigned int m_nPoolSize;
	HANDLE m_hExecThread[MAX_THREAD_COUNT];
	int m_nThreadCount;
	bool m_bStopThread;
	fnCompletionCallback m_fnCompletionCallback;

	CRITICAL_SECTION m_csInputQuery;
	InputQueryList m_listInputQuery;
	CRITICAL_SECTION m_csCallback;

	static DWORD WINAPI ExecuteThread(LPVOID lpParam);
	void ExecuteSQL(MINPUTQUERY* pQuery);

	static void AsyncCompletionCallback(System::IAsyncResult^ result);

	MINPUTQUERY* PopQuery();
	void ClearQuery();

	bool IsStopThread()			{ return m_bStopThread; }
	void LockCallback()			{ EnterCriticalSection(&m_csCallback); }
	void UnLockCallback()		{ LeaveCriticalSection(&m_csCallback); }
};

ref class MADOCommand
{
public:
	MADODatabase* m_pADODB;
	System::Data::SqlClient::SqlCommand^ m_pCmd;
	unsigned int m_nJobNumber;

	MADOCommand(MADODatabase* pADODB)
	{
		m_nJobNumber = 0;
		m_pADODB = pADODB;
		m_pCmd = gcnew System::Data::SqlClient::SqlCommand;
	}
	~MADOCommand() { }
};