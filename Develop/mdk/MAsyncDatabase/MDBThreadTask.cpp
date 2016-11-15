#include "stdafx.h"
#include "MDBThreadTask.h"
#include "MDatabase.h"
#include "MLocale.h"

namespace mdb
{
	const wstring MDBThreadTask::m_strNULLQuery;

	MDBThreadTask::MDBThreadTask(const size_t nReserveSQLSize)
	{
		m_vecSQL.reserve(nReserveSQLSize);
	}


	MDBThreadTask::~MDBThreadTask()
	{

	}


	const MDB_SQL_VEC& MDBThreadTask::GetSQLList() 
	{
		return m_vecSQL;
	}


	MDBThreadTask& MDBThreadTask::PushSQL(const string& strSQL)
	{
		PushSQLW(MLocale::ConvAnsiToUCS2(strSQL.c_str()));		

		return *this;
	}

	MDBThreadTask& MDBThreadTask::PushSQLW(const wstring& strSQL)
	{
		m_vecSQL.push_back(strSQL);
		return *this;
	}

	const string MDBThreadTask::GetSQL( const size_t nIndex )
	{
		return MLocale::ConvUTF16ToAnsi(GetSQLW(nIndex).c_str());
	}

	const wstring MDBThreadTask::GetSQLW(const size_t nIndex)
	{
		_ASSERT(nIndex < m_vecSQL.size());

		if (nIndex >= m_vecSQL.size())
			return m_strNULLQuery;

		return m_vecSQL[nIndex];
	}

	const size_t MDBThreadTask::GetQuerySize()
	{
		return m_vecSQL.size();
	}
}