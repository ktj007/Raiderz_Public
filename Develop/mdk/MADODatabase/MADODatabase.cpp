#include "MADODatabase.h"
#include <crtdbg.h>

using namespace System;
using namespace System::Data::SqlClient;

MADODatabase::MADODatabase()
{
	m_nPoolSize = 100;
	m_nThreadCount = 2;
	m_bStopThread = false;
	m_fnCompletionCallback = NULL;

	for (int i = 0; i < MAX_THREAD_COUNT; i++)
	{
		m_hExecThread[i] = INVALID_HANDLE_VALUE;
	}
	memset(m_szConnection, 0, MAX_CONNECTION_STRING_LENGTH);

	InitializeCriticalSection(&m_csInputQuery);
	InitializeCriticalSection(&m_csCallback);
}

MADODatabase::~MADODatabase()
{
	ReleaseADODatabase();

	DeleteCriticalSection(&m_csInputQuery);
	DeleteCriticalSection(&m_csCallback);

	m_fnCompletionCallback = NULL;
}

bool MADODatabase::InitADODatabase(const char* szDBServerIP, const char* szDBName, const char* szUserID, const char* szUserPWD, fnCompletionCallback fnCallback)
{
	if ((strlen(szDBServerIP) == 0) || (strlen(szDBName) == 0) || (strlen(szUserID) == 0) || (strlen(szUserPWD) == 0) || (fnCallback == NULL))
	{
		return false;
	}

	sprintf_s(m_szConnection, MAX_CONNECTION_STRING_LENGTH,
		"Data Source=%s;Initial Catalog=%s;user id=%s;password=%s;Asynchronous Processing=true;",	// Max Pool Size=%u
		szDBServerIP, szDBName, szUserID, szUserPWD);
	m_fnCompletionCallback	= fnCallback;

	SqlConnection^ pConnect = gcnew SqlConnection;
	pConnect->ConnectionString = gcnew String(m_szConnection);

	try
	{
		pConnect->Open();
	}
	catch (SqlException^ e)
	{
		char szOutput[512] = {0, };
		sprintf_s(szOutput, 512, "Exception =>\tErrorCode : %d\n\t\t%s\n", e->Number, e->Message);
		OutputDebugString(szOutput);
		return false;
	}

	if (pConnect->State != ConnectionState::Closed)
	{
		pConnect->Close();
	}

	for (int i = 0; i < m_nThreadCount; i++)
	{
		m_hExecThread[i] = CreateThread(NULL, 0, MADODatabase::ExecuteThread, this, 0, NULL);
		if (m_hExecThread[i] == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}
	return true;
}

void MADODatabase::SetConnectionPoolSize( unsigned int nPoolSize )
{
	m_nPoolSize	= nPoolSize;

	char szMaxPoolSize[20] = {0, };
	sprintf_s(szMaxPoolSize, "Max Pool Size=%d;", m_nPoolSize);
	strncat_s(m_szConnection, szMaxPoolSize, 20);
}

void MADODatabase::ReleaseADODatabase()
{
	m_bStopThread = true;

	if (m_hExecThread != NULL)
	{
		DWORD ret = WaitForMultipleObjects(m_nThreadCount, m_hExecThread, TRUE, INFINITE);
		if (ret == WAIT_OBJECT_0)
		{
			for(int i = 0; i < m_nThreadCount; i++)
			{
				CloseHandle(m_hExecThread[i]);
				m_hExecThread[i] = INVALID_HANDLE_VALUE;
			}
		}
	}

	ClearQuery();
}

DWORD WINAPI MADODatabase::ExecuteThread(LPVOID lpParam)
{
	MINPUTQUERY* pQuery = NULL;
	MADODatabase* pAdoDB = (MADODatabase*)lpParam;

	while (true)
	{
		if (pAdoDB->IsStopThread() == true)
		{
			break;
		}

		while (true)
		{
			if (pAdoDB->GetQueryCount() == 0)
			{
				break;
			}

			pQuery = pAdoDB->PopQuery();
			if (pQuery != NULL)
			{
				pAdoDB->ExecuteSQL(pQuery);
				delete pQuery;
			}
		}
		Sleep(1);
	}

	return 0;
}

void MADODatabase::ExecuteSQL(MINPUTQUERY* pQuery)
{
	if (pQuery == NULL)
	{
		return;
	}

	SqlConnection^ pConnect = gcnew SqlConnection;
	pConnect->ConnectionString = gcnew String(m_szConnection);

	MADOCommand^ pCommand = gcnew MADOCommand(this);
	pCommand->m_pCmd->Connection = pConnect;
	pCommand->m_pCmd->CommandText = gcnew String(pQuery->szQuery);
	pCommand->m_pCmd->CommandType = CommandType::Text;
	pCommand->m_nJobNumber = pQuery->nJobNumber;

	AsyncCallback^ fnAsyncCallback = gcnew AsyncCallback(&MADODatabase::AsyncCompletionCallback);

	int nHandle = 0;
	try
	{
		pConnect->Open();
		IAsyncResult^ iRst = pCommand->m_pCmd->BeginExecuteReader(fnAsyncCallback, pCommand);
	}
	catch (SqlException^ e)
	{
		char szOutput[512] = {0, };
		sprintf_s(szOutput, 512, "Exception =>\tErrorCode : %d\n\t\t%s\n", e->Number, e->Message);
		OutputDebugString(szOutput);
		
		LockCallback();
		m_fnCompletionCallback(pCommand->m_nJobNumber, NULL, szOutput);
		UnLockCallback();

		if (pConnect->State != ConnectionState::Closed)
		{
			pConnect->Close();
		}
	}
}

void MADODatabase::AsyncCompletionCallback(IAsyncResult^ result)
{
	MADOCommand^ pCommand = (MADOCommand^)result->AsyncState;
	SqlConnection^ pConnection = pCommand->m_pCmd->Connection;

	SqlDataReader^ pDataReader;
	MRecordset* pRs = NULL;
	char szError[MAX_ERROR_STRING_LENGTH] = {'\0', };

	try
	{
		pDataReader = pCommand->m_pCmd->EndExecuteReader(result);
		pRs = new MRecordset(pDataReader);
	}
	catch (SqlException^ e)
	{
		sprintf_s(szError, MAX_ERROR_STRING_LENGTH, "[Exception]\nErrorCode : %d\n%s\n", e->Number, e->Message);
	}

	pCommand->m_pADODB->LockCallback();
	pCommand->m_pADODB->m_fnCompletionCallback(pCommand->m_nJobNumber, pRs, szError);
	pCommand->m_pADODB->UnLockCallback();

	if (pRs != NULL)
	{
		pDataReader->Close();
		delete pRs;
	}

	pConnection->Close();
}

void MADODatabase::PushQuery(const char* szQuery, unsigned int nJobNumber)
{
	if (strlen(szQuery) == 0)
	{
		return;
	}

	MINPUTQUERY* pInputData = new MINPUTQUERY;
	pInputData->nJobNumber = nJobNumber;
	strcpy_s(pInputData->szQuery, MAX_QUERY_STRING_LENGTH, szQuery);

	EnterCriticalSection(&m_csInputQuery);

	m_listInputQuery.push_back(pInputData);

	LeaveCriticalSection(&m_csInputQuery);
}

MINPUTQUERY* MADODatabase::PopQuery()
{
	MINPUTQUERY* pQuery = NULL;

	EnterCriticalSection(&m_csInputQuery);

	InputQueryList::iterator iter = m_listInputQuery.begin();
	if (iter != m_listInputQuery.end())
	{
		pQuery = *iter;
		m_listInputQuery.pop_front();
	}

	LeaveCriticalSection(&m_csInputQuery);
	return pQuery;
}

void MADODatabase::ClearQuery()
{
	EnterCriticalSection(&m_csInputQuery);

	InputQueryList::iterator iter = m_listInputQuery.begin();
	for ( ; iter != m_listInputQuery.end(); iter++)
	{
		delete *iter;
	}
	m_listInputQuery.clear();

	LeaveCriticalSection(&m_csInputQuery);
}
