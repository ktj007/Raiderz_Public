#include "stdafx.h"
#include "MSQLSTMT.h"
#include "MDatabase.h"


namespace mdb
{
	MSQLSTMT::MSQLSTMT() : m_hStmt(SQL_NULL_HSTMT)
	{

	}

	MSQLSTMT::~MSQLSTMT()
	{
		Close();
	}

	bool MSQLSTMT::Init( MDatabase* pDatabase )
	{
		_ASSERT(NULL != pDatabase);
		_ASSERT(SQL_NULL_HDBC != pDatabase->m_hDBC);
		_ASSERT(SQL_NULL_HSTMT == m_hStmt);

		if (NULL == pDatabase)
			return false;

		if (SQL_NULL_HDBC == pDatabase->m_hDBC)
			return false;

		if (SQL_NULL_HSTMT != m_hStmt)
			return false;

		if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, pDatabase->m_hDBC, &m_hStmt))
			return false;
		
		return true;
	}

	bool MSQLSTMT::Close()
	{
		if (SQL_NULL_HSTMT == m_hStmt)
			return true;

		if (SQL_SUCCESS != SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt))
			return false;

		m_hStmt = SQL_NULL_HSTMT;

		return true;
	}	

	SQLHSTMT MSQLSTMT::GetSTMT()
	{
		return m_hStmt;
	}
}