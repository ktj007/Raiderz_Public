#include "stdafx.h"
#include "MDBFieldValueTypeTimestamp.h"
#include "MDatabaseConst.h"


namespace mdb
{
	MDBFieldValueTypeTimestamp::MDBFieldValueTypeTimestamp(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{

	}


	MDBFieldValueTypeTimestamp::~MDBFieldValueTypeTimestamp()
	{

	}


	void MDBFieldValueTypeTimestamp::Reset()
	{
		SetNull();
		memset(&m_Timestamp, 0, sizeof(m_Timestamp));
	}


	bool MDBFieldValueTypeTimestamp::GetData(MDBRecordSet* pRecordSet)
	{
		SQLINTEGER	nReadLength;
		if (!pRecordSet->GetData(m_nIndex
			, (SQLPOINTER)&m_Timestamp
			, SQL_C_TYPE_TIMESTAMP
			, sizeof(m_Timestamp)
			, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		if (sizeof(m_Timestamp) != nReadLength)
		{
			return false;
		}

		SetNotNull();

		_ASSERT((-1 != nReadLength) && (sizeof(m_Timestamp) == nReadLength) && !IsNull());

		return true;
	}


	__int64 MDBFieldValueTypeTimestamp::AsInt64()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	long MDBFieldValueTypeTimestamp::AsLong()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	int MDBFieldValueTypeTimestamp::AsInt()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	short MDBFieldValueTypeTimestamp::AsShort()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	unsigned char MDBFieldValueTypeTimestamp::AsByte()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	bool MDBFieldValueTypeTimestamp::AsBool()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return false;
	}


	const string MDBFieldValueTypeTimestamp::AsString()
	{
		if (IsNull())
			return string("NULL");
		
		char szTimestamp[MDB_TIMESTAMP_STR_LEN] = {0,};
		sprintf_s(szTimestamp, 32, "%d-%d-%d %d:%d:%d"
			, m_Timestamp.year
			, m_Timestamp.month
			, m_Timestamp.day
			, m_Timestamp.hour
			, m_Timestamp.minute
			, m_Timestamp.second);

		return string(szTimestamp);
	}


	const TIMESTAMP_STRUCT	MDBFieldValueTypeTimestamp::AsTimestamp()
	{
		return m_Timestamp;
	}

	const float MDBFieldValueTypeTimestamp::AsFloat()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0.0f;
	}

	const std::wstring MDBFieldValueTypeTimestamp::AsWString()
	{
		if (IsNull())
			return wstring(L"NULL");

		wchar_t szTimestamp[MDB_TIMESTAMP_STR_LEN] = {0,};
		_snwprintf_s(szTimestamp, 32, 31, L"%d-%d-%d %d:%d:%d"
			, m_Timestamp.year
			, m_Timestamp.month
			, m_Timestamp.day
			, m_Timestamp.hour
			, m_Timestamp.minute
			, m_Timestamp.second);

		return std::wstring(szTimestamp);
	}
}