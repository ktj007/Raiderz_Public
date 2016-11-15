#pragma once


#include "MSQLSTMT.h"
#include "MADBLib.h"

namespace mdb
{
	class MDatabase;

	class MADB_API MDatabaseQuery
	{
	public :
		MDatabaseQuery(MDatabase* pDB);
		virtual ~MDatabaseQuery();

		MDatabase*		GetDatabase();
		MSQLSTMT&		GetSQLSTMT();

		virtual bool	Execute(const std::string& strSQL);
		virtual bool	ExecuteW(const std::wstring& strSQL);

		virtual bool	Execute(const std::string& strSQL, int& nRefReturn);
		virtual bool	ExecuteW(const std::wstring& strSQL, int& nRefReturn);

		virtual bool	Close();

		
	protected :
		MDatabase*		m_pDatabase;
		MSQLSTMT		m_SqlStmt;		
	};
}