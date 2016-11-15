#include "stdafx.h"
#include "MDatabase.h"
#include "MDatabaseDefine.h"
#include "MDatabaseDesc.h"
#include "MDatabaseInfo.h"
#include "MLocale.h"

namespace mdb
{
	MDatabase::MDatabase()
	{
		Reset();
	}


	MDatabase::~MDatabase()
	{
		Disconnect();
	}


	bool MDatabase::Connect(const std::string& strDSN)
	{
		return ConnectW(MLocale::ConvAnsiToUCS2(strDSN.c_str()));
	}

	bool MDatabase::ConnectW(const std::wstring& strDSN)
	{
		_ASSERT(!strDSN.empty());

		SQLRETURN Ret = SQL_FALSE;

		Ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
		CHK_SQLRET_FAIL(Ret, false);

		Ret = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		CHK_SQLRET_FAIL(Ret, false);

		Ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDBC); 
		CHK_SQLRET_FAIL(Ret, false);

		int nLoginTimeSec = 5;
		Ret = SQLSetConnectAttr(m_hDBC, SQL_LOGIN_TIMEOUT, (SQLPOINTER)nLoginTimeSec, 0);
		CHK_SQLRET_FAIL(Ret, false);

		SQLWCHAR	szConnectOutput[256];
		SQLSMALLINT nConnectOption = SQL_DRIVER_COMPLETE;
		SQLSMALLINT	nResult;

		Ret = SQLDriverConnectW(m_hDBC
			, NULL
			, (SQLWCHAR*)strDSN.c_str(), SQL_NTS
			, szConnectOutput, 256
			, &nResult
			, nConnectOption);
		CHK_SQLRET_FAIL(Ret, false);

		if (!SetDriverConcurrency())
			return false;

		if (!CheckDriverScrollable())
			return false;

		m_bConnected	= true;
		m_strDSN		= strDSN;

		return true;
	}


	bool MDatabase::Reconnect()
	{
		_ASSERT(!m_strDSN.empty());

		if (m_strDSN.empty())
			return false;

		if (m_bConnected)
			Disconnect();
		
		return ConnectW(m_strDSN);
	}


	void MDatabase::Disconnect()
	{
		if (m_bConnected)
		{
			SQLDisconnect(m_hDBC);
			SQLFreeHandle(SQL_HANDLE_DBC, m_hDBC);
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);

			Reset();
		}		
	}


	bool MDatabase::IsConnected()
	{
		return m_bConnected;		
	}


	const std::wstring& MDatabase::GetDSN()
	{
		return m_strDSN;
	}


	bool MDatabase::BeginTran()
	{
		// 1step transaction만 지원한다.

		_ASSERT(false == m_bIsTransaction);
		_ASSERT(SQL_NULL_HDBC != m_hDBC);

		if (true == m_bIsTransaction)
		{
			return false;	
		}

		if (SQL_NULL_HDBC == m_hDBC)
		{
			return false;
		}

		const SQLRETURN Ret = SQLSetConnectAttr(m_hDBC
											, SQL_ATTR_AUTOCOMMIT
											, (SQLPOINTER)SQL_AUTOCOMMIT_OFF
											, SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		m_bIsTransaction = true;

		return true;
	}


	bool MDatabase::CommitTran()
	{
		_ASSERT(true == m_bIsTransaction);
		_ASSERT(SQL_NULL_HDBC != m_hDBC);

		if (!m_bIsTransaction)
		{
			return false;
		}

		if (SQL_NULL_HDBC == m_hDBC)
		{
			return false;
		}

		const SQLRETURN RetCommint = SQLEndTran(SQL_HANDLE_ENV, m_hEnv, SQL_COMMIT);
		CHK_SQLRET_FAIL(RetCommint, false);

		const SQLRETURN RetConnAttr = SQLSetConnectAttr(m_hDBC
			, SQL_ATTR_AUTOCOMMIT
			, (SQLPOINTER)SQL_AUTOCOMMIT_ON
			, SQL_NTS);
		CHK_SQLRET_FAIL(RetConnAttr, false);

		m_bIsTransaction = false;

		return true;
	}


	bool MDatabase::RollbackTran()
	{
		_ASSERT(true == m_bIsTransaction);
		_ASSERT(SQL_NULL_HDBC != m_hDBC);

		if (!m_bIsTransaction)
		{
			return false;
		}

		if (SQL_NULL_HDBC == m_hDBC)
		{
			return false;
		}

		const SQLRETURN RetRollback = SQLEndTran(SQL_HANDLE_ENV, m_hEnv, SQL_ROLLBACK);
		CHK_SQLRET_FAIL(RetRollback, false);

		const SQLRETURN RetConnAttr = SQLSetConnectAttr(m_hDBC
			, SQL_ATTR_AUTOCOMMIT
			, (SQLPOINTER)SQL_AUTOCOMMIT_ON
			, SQL_NTS);
		CHK_SQLRET_FAIL(RetConnAttr, false);

		m_bIsTransaction = false;

		return true;
	}


	bool MDatabase::SetDriverConcurrency()
	{
		SQLSMALLINT nResult;

		const SQLRETURN Ret = SQLGetInfo(m_hDBC, SQL_SCROLL_CONCURRENCY,
			&m_nDriverConcurrency, 4, &nResult);
		CHK_SQLRET_FAIL(Ret, false);

		if (SQL_FALSE == nResult)
		{
			return false;
		}

		return true;
	}


	SQLINTEGER MDatabase::GetDriverConcurrency()
	{
		return m_nDriverConcurrency;
	}


	bool MDatabase::CheckDriverScrollable()
	{
		_ASSERT(SQL_NULL_HDBC != m_hDBC);

		if (SQL_NULL_HDBC == m_hDBC)
		{
			return  false;
		}

		SQLUSMALLINT nScrollable;
		const SQLRETURN Ret = SQLGetFunctions(m_hDBC
			, SQL_API_SQLEXTENDEDFETCH
			, &nScrollable);
		CHK_SQLRET_FAIL(Ret, false);

		if (1 == nScrollable)
		{
			m_bIsScrollable = true;
		}

		return true;
	}


	bool MDatabase::IsScrollable()
	{
		return m_bIsScrollable;
	}

	void MDatabase::Reset()
	{
		m_hDBC				= SQL_NULL_HDBC;
		m_hEnv				= SQL_NULL_HENV;

		m_bConnected		= false;
		m_bIsTransaction	= false;	
		m_bIsScrollable		= false;
	}
}
