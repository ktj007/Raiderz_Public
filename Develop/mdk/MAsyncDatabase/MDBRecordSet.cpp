#include "stdafx.h"
#include "MDBRecordSet.h"
#include "MDatabase.h"
#include "MDatabaseDefine.h"
#include "MDBFieldValueTypeHeaderList.h"
#include "MDBFieldValueTypeFactory.h"
#include "MLocale.h"

namespace mdb
{
	MDBRecordSet::MDBRecordSet() : MDatabaseQuery(NULL), m_pNullField(NULL), m_bIsEOF(true)
	{
		InitData();
	}

	MDBRecordSet::MDBRecordSet(MDatabase* pDB) : MDatabaseQuery(pDB), m_pNullField(NULL), m_bIsEOF(true)
	{
		InitData();
	}

	MDBRecordSet::~MDBRecordSet()
	{
		Close();
	}

	void MDBRecordSet::StartMovingRecordSet()
	{
		m_bIsOpened = true;
		m_bIsClosed = false;
		m_bIsEOF	= false;

		MoveNext();		
	}

	bool MDBRecordSet::InitOpen( MDatabase* pDB, size_t nSqlLen )
	{
		_ASSERT(NULL != pDB);
		_ASSERT(nSqlLen > 0);

		if (NULL == pDB || nSqlLen == 0)
			return false;

		if (!pDB->IsConnected())
			return false;

		if (!m_SqlStmt.Init(pDB))
			return false;

		if (!InitEnv())
			return false;

		m_pDatabase = pDB;

		return true;
	}

	bool MDBRecordSet::Open(MDatabase* pDB, const string& strSQL, int& nRefReturn, bool bHaveReturn)
	{
		if (!InitOpen(pDB, strSQL.length()))
			return false;

		SQLRETURN	Ret		= SQL_FALSE;
		SQLINTEGER	nReturn = 0;
		SQLINTEGER	nRetLen = 0;

		if (bHaveReturn)
		{
			const SQLRETURN BindParam = SQLBindParameter( m_SqlStmt.GetSTMT(), 1, SQL_PARAM_OUTPUT, 
				SQL_C_LONG, SQL_INTEGER, sizeof(&nReturn), 0, &nReturn, 0 , &nRetLen);
			CHK_SQLRET_FAIL(Ret, false);			
		}

		Ret = SQLExecDirectA(m_SqlStmt.GetSTMT(), (SQLCHAR*)strSQL.c_str(), SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		nRefReturn = nReturn;

		if (!BuildField())
			return false;

		StartMovingRecordSet();

		return true;
	}

	bool MDBRecordSet::OpenW(MDatabase* pDB, const wstring& strSQL, int& nRefReturn, bool bHaveReturn)
	{
		if (!InitOpen(pDB, strSQL.length()))
			return false;

		SQLRETURN	Ret		= SQL_FALSE;
		SQLINTEGER	nReturn = 0;
		SQLINTEGER	nRetLen = 0;

		if (bHaveReturn)
		{
			const SQLRETURN BindParam = SQLBindParameter( m_SqlStmt.GetSTMT(), 1, SQL_PARAM_OUTPUT, 
				SQL_C_LONG, SQL_INTEGER, sizeof(&nReturn), 0, &nReturn, 0 , &nRetLen);
			CHK_SQLRET_FAIL(Ret, false);			
		}

		Ret = SQLExecDirectW(m_SqlStmt.GetSTMT(), (SQLWCHAR*)strSQL.c_str(), SQL_NTS);
		CHK_SQLRET_FAIL(Ret, false);

		nRefReturn = nReturn;

		if (false == BuildField())
			return false;

		StartMovingRecordSet();

		return true;
	}

	bool MDBRecordSet::Close()
	{
		if (!MDatabaseQuery::Close())
			return false;

		ReleaseField();
		InitData();
		
		return true;
	}


	int MDBRecordSet::GetFieldCountFromODBC()
	{
		SQLSMALLINT nFieldCount;
		const SQLRETURN Ret = SQLNumResultCols(m_SqlStmt.GetSTMT(), &nFieldCount);
		CHK_SQLRET_FAIL(Ret, -1);

		return static_cast<int>(nFieldCount);
	}


	bool MDBRecordSet::BuildField()
	{
		_ASSERT(SQL_NULL_HSTMT != m_SqlStmt.GetSTMT());

		if (SQL_NULL_HSTMT == m_SqlStmt.GetSTMT())
			return false;

		const int nFieldCount = GetFieldCountFromODBC();

		if (0 > nFieldCount)	return false;	// error.
		if (0 == nFieldCount)	return true;

		m_FieldVec.reserve(nFieldCount);

		wchar_t					szName[MDB_MAX_FIELD_NAME_LEN];
		SQLSMALLINT				nNameLen;
		SQLRETURN				Ret;
		SQLSMALLINT				SqlType;

		for (SQLSMALLINT i = 1; i <= nFieldCount; ++i)
		{
			Ret = SQLDescribeColW(m_SqlStmt.GetSTMT()
				, i
				, (SQLWCHAR*)szName
				, MDB_MAX_FIELD_NAME_LEN
				, &nNameLen
				, &SqlType
				, 0
				, 0
				, 0);
			CHK_SQLRET_FAIL(Ret, false);

			szName[nNameLen]	= L'\0';
			
			if (!InsertField(wstring(szName), SqlType, i))
				return false;			
		}

		return true;
	}


	bool MDBRecordSet::InsertField(const wstring& strFieldName, const SQLSMALLINT SqlType, const SQLSMALLINT nIndex)
	{
		_ASSERT(0 != strFieldName.length());
		if (strFieldName.empty())
			return false;

		const size_t nHash = MakeHashValueW(strFieldName);

		MDBFieldValueType* pvt = MDBFieldValueTypeFactory::Create(SqlType, nIndex);
		MDB_FIELD_HMAP_RETURN ret2 = m_FieldMap.insert(MDB_FIELD_HMAP::value_type(nHash, pvt));
		_ASSERT(true == ret2.second && "crash Hash value. check duplecated column name.");
		if (false == ret2.second)
			return false;

		m_FieldVec.push_back(pvt);

		return true;
	}


	bool MDBRecordSet::BuildFieldValue()
	{
		_ASSERT(!m_FieldMap.empty());

		if (m_FieldMap.empty())
			return false;

		const size_t nSize = m_FieldVec.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			if (!m_FieldVec[i]->GetData(this))
				return false;
		}		

		return true;
	}


	bool MDBRecordSet::MoveNext()
	{
		SQLRETURN Ret = SQLFetchScroll(m_SqlStmt.GetSTMT(), SQL_FETCH_NEXT, 0);
		if ((SQL_NO_DATA == Ret) || (SQL_ERROR == Ret))
		{
			SetEOF(true);
			return false;
		}
		CHK_SQLRET_FAIL(Ret, false);

		if (!BuildFieldValue())
		{
			SetEOF(true);
			return false;
		}

		m_nFetchedRowCount += m_nFetchedCount;

		SetEOF(false);

		return true;
	}


	void MDBRecordSet::SetEOF(const bool nState)
	{
		m_bIsEOF = nState;
	}


	bool MDBRecordSet::IsEOF()
	{
		return m_bIsEOF;
	}

	bool MDBRecordSet::GetData(const short nIndex
		, SQLPOINTER pTargetValue
		, const SQLSMALLINT nValueType
		, const SQLSMALLINT nValueLength
		, long& nReadLength)
	{
		SQLRETURN Ret = SQLGetData(m_SqlStmt.GetSTMT()
			, nIndex
			, nValueType
			, pTargetValue
			, nValueLength
			, &nReadLength);
		CHK_SQLRET_FAIL(Ret, false);

		return true;
	}	


	long MDBRecordSet::GetFetchedRowCount()
	{
		return m_nFetchedRowCount;
	}


	int MDBRecordSet::GetFetchedCount()
	{
		return m_nFetchedCount;
	}


	size_t MDBRecordSet::MakeHashValue(const string& str)
	{
		return MakeHashValueW(MLocale::ConvAnsiToUCS2(str.c_str()));
	}

	size_t MDBRecordSet::MakeHashValueW(const wstring& str)
	{
		size_t seed = 0;
		size_t x	= 0;

		// 부스트를 참고 했음.
		BYTE* pByte = (BYTE*)str.c_str();
		size_t nSize = str.length() * sizeof(wchar_t);

		for(size_t i = 0; i < nSize; i++)
		{
			x = pByte[i];

			x += (x >> 3);

			seed ^= x + 0x9e3779b9 + (seed<<6) + (seed>>2);
		}

		return seed;
	}

	short MDBRecordSet::GetFieldCount()
	{
		return static_cast<short>(m_FieldMap.size());
	}


	void MDBRecordSet::InitData()
	{
		m_bIsEOF			= true;
		m_nFetchedCount		= 0;
		m_nFetchedRowCount	= 0;
		m_bIsOpened			= false;
		m_bIsClosed			= true;
	}

	bool MDBRecordSet::IsOpen()
	{
		return m_bIsOpened;
	}

	bool MDBRecordSet::InitEnv()
	{
		SQLRETURN Ret;

		Ret = SQLSetStmtAttr(GetSQLSTMT().GetSTMT()
			, SQL_ATTR_CURSOR_TYPE
			, (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY
			, 0);
		CHK_SQLRET_FAIL(Ret, false);

		Ret = SQLSetStmtAttr(GetSQLSTMT().GetSTMT()
			, SQL_ATTR_CONCURRENCY
			, (SQLPOINTER)SQL_CONCUR_READ_ONLY
			, 0);
		CHK_SQLRET_FAIL(Ret, false);

		Ret = SQLSetStmtAttr(GetSQLSTMT().GetSTMT()
			, SQL_ATTR_ROWS_FETCHED_PTR
			, (SQLPOINTER)&m_nFetchedCount
			, 4);
		CHK_SQLRET_FAIL(Ret, false);

		Ret = SQLSetStmtAttr(GetSQLSTMT().GetSTMT()
			, SQL_ATTR_ROW_ARRAY_SIZE
			, (SQLPOINTER)1
			, 0);
		CHK_SQLRET_FAIL(Ret, false);

		Ret = SQLSetStmtAttr(GetSQLSTMT().GetSTMT()
			, SQL_ATTR_CURSOR_SCROLLABLE
			, (SQLPOINTER)SQL_NONSCROLLABLE
			, 0);
		CHK_SQLRET_FAIL(Ret, false);

		return true;
	}

	void MDBRecordSet::ReleaseField()
	{
		m_FieldVec.clear();

		MDB_FIELD_HMAP::iterator end = m_FieldMap.end();
		for (MDB_FIELD_HMAP::iterator it = m_FieldMap.begin(); it != end; ++it)
		{
			delete it->second;
		}

		m_FieldMap.clear();

		if (NULL != m_pNullField)
		{
			delete m_pNullField;
			m_pNullField = NULL;
		}
	}

	MDBFieldValueType& MDBRecordSet::Field( const string& strName )
	{
		return FieldW(MLocale::ConvAnsiToUCS2(strName.c_str()));
	}

	MDBFieldValueType& MDBRecordSet::FieldW(const wstring& strName)
	{
		return FieldHash(MakeHashValueW(strName));
	}

	MDBFieldValueType& MDBRecordSet::FieldHash( const size_t nHash )
	{
		MDB_FIELD_HMAP::iterator it = m_FieldMap.find(nHash);
		if (m_FieldMap.end() == it)
		{
			_ASSERT(false && "invalid column name");
			return GetNullField();
		}
		
		return *(it->second);
	}

	MDBFieldValueType& MDBRecordSet::FieldIndex( const size_t nIndex )
	{
		if (m_FieldMap.size() <= nIndex)
		{
			return GetNullField();
		}

		return *m_FieldVec[nIndex];
	}

	MDBFieldValueType& MDBRecordSet::GetNullField()
	{
		// 찾는 필드가 없을 경우 반환하기 위해서 만드며, 
		// 처음 필드 검색을 실패할때까진 생성을 하지 않는다.
		if (NULL == m_pNullField)
		{
			m_pNullField = new MDBFieldValueTypeNull(-1);
		}

		return *m_pNullField;
	}

	bool MDBRecordSet::Execute( const string& strSQL )
	{
		int nReturn = 0;

		return Open(m_pDatabase, strSQL, nReturn, false);
	}

	bool MDBRecordSet::Execute( const string& strSQL, int& nRefReturn )
	{
		return Open(m_pDatabase, strSQL, nRefReturn, true);
	}

	bool MDBRecordSet::ExecuteW(const wstring& strSQL)
	{
		int nReturn = 0;

		return OpenW(m_pDatabase, strSQL, nReturn, false);
	}

	bool MDBRecordSet::ExecuteW( const wstring& strSQL, int& nRefReturn )
	{
		return OpenW(m_pDatabase, strSQL, nRefReturn, true);
	}

	bool MDBRecordSet::Open(const string& strSQL)
	{
		int nReturn = 0;

		return Open(m_pDatabase, strSQL, nReturn, false);
	}

	bool MDBRecordSet::Open( const string& strSQL, int& nRefReturn )
	{
		return Open(m_pDatabase, strSQL, nRefReturn, true);
	}

	bool MDBRecordSet::OpenW(const wstring& strSQL)
	{
		int nReturn = 0;

		return OpenW(m_pDatabase, strSQL, nReturn, false);
	}

	bool MDBRecordSet::OpenW( const wstring& strSQL, int& nRefReturn )
	{
		return OpenW(m_pDatabase, strSQL, nRefReturn, true);
	}
}
