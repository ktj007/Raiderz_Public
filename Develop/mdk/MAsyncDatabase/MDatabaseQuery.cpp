#include "stdafx.h"
#include "MDatabaseQuery.h"
#include "MDatabaseDefine.h"
#include "MLocale.h"

namespace mdb
{
	MDatabaseQuery::MDatabaseQuery( MDatabase* pDB ) : m_pDatabase(pDB)
	{

	}

	MDatabaseQuery::~MDatabaseQuery()
	{
		m_SqlStmt.Close();

		m_pDatabase = NULL;
	}

	MDatabase* MDatabaseQuery::GetDatabase()
	{
		return m_pDatabase;
	}

	bool MDatabaseQuery::Close()
	{
		if (!m_SqlStmt.Close())
			return false;

		return true;
	}

	MSQLSTMT& MDatabaseQuery::GetSQLSTMT()
	{
		return m_SqlStmt;
	}

	bool MDatabaseQuery::Execute( const std::string& strSQL )
	{
		if (!m_SqlStmt.Init(m_pDatabase))
			return false;

		const SQLRETURN Ret = SQLExecDirectA(m_SqlStmt.GetSTMT(), (SQLCHAR*)strSQL.c_str(), SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		return true;
	}

	bool MDatabaseQuery::Execute( const std::string& strSQL, int& nRefReturn )
	{
		if (!m_SqlStmt.Init(m_pDatabase))
			return false;

		SQLINTEGER nRetLen = 0;
		SQLINTEGER nReturn = 0;

		const SQLRETURN BindParam = SQLBindParameter( m_SqlStmt.GetSTMT(), 1, SQL_PARAM_OUTPUT, 
											SQL_C_LONG, SQL_INTEGER, sizeof(&nReturn), 0, &nReturn, 0 , &nRetLen);
		CHK_SQLRET_FAIL(BindParam, false);

		const SQLRETURN Ret = SQLExecDirectA(m_SqlStmt.GetSTMT(), (SQLCHAR*)strSQL.c_str(), SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		nRefReturn = nReturn;

		return true;
	}

	bool MDatabaseQuery::ExecuteW(const std::wstring& strSQL)
	{
		if (!m_SqlStmt.Init(m_pDatabase))
			return false;

		const SQLRETURN Ret = SQLExecDirectW(m_SqlStmt.GetSTMT(), (SQLWCHAR*)strSQL.c_str(), SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		return true;
	}

	bool MDatabaseQuery::ExecuteW( const std::wstring& strSQL, int& nRefReturn )
	{
		if (!m_SqlStmt.Init(m_pDatabase))
			return false;

		SQLINTEGER nRetLen = 0;
		SQLINTEGER nReturn = 0;

		const SQLRETURN BindParam = SQLBindParameter( m_SqlStmt.GetSTMT(), 1, SQL_PARAM_OUTPUT, 
			SQL_C_LONG, SQL_INTEGER, sizeof(&nReturn), 0, &nReturn, 0 , &nRetLen);
		CHK_SQLRET_FAIL(BindParam, false);

		const SQLRETURN Ret = SQLExecDirectW(m_SqlStmt.GetSTMT(), (SQLWCHAR*)strSQL.c_str(), SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		nRefReturn = nReturn;

		return true;
	}
}
