#include "stdafx.h"
#include "MDBFieldValueTypeInt64.h"
#include "MDatabaseDefine.h"


namespace mdb
{
	MDBFieldValueTypeInt64::MDBFieldValueTypeInt64(const SQLSMALLINT nIndex)  : MDBFieldValueType(nIndex)
	{
		m_nVal = 0;
	}


	MDBFieldValueTypeInt64::~MDBFieldValueTypeInt64() 
	{
		m_nVal = 0;
	}


	bool MDBFieldValueTypeInt64::GetData(MDBRecordSet* pRecordSet)
	{
		SQLINTEGER nReadLength;

		if (!pRecordSet->GetData(m_nIndex, &m_nVal, SQL_C_SBIGINT, 8, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		if (8 != nReadLength)
		{
			return false;
		}

		SetNotNull();

		_ASSERT((-1 != nReadLength) && (8 == nReadLength) && !IsNull());

		return true;
	}


	void MDBFieldValueTypeInt64::Reset()
	{
		SetNull();
		m_nVal		= 0;
	}


	__int64 MDBFieldValueTypeInt64::AsInt64()
	{
		return m_nVal;
	}


	long MDBFieldValueTypeInt64::AsLong()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<long>(m_nVal);
	}


	int MDBFieldValueTypeInt64::AsInt()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<int>(m_nVal);
	}


	short MDBFieldValueTypeInt64::AsShort()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<short>(m_nVal);
	}


	unsigned char MDBFieldValueTypeInt64::AsByte()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<unsigned char>(m_nVal);
	}


	bool MDBFieldValueTypeInt64::AsBool()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return 0 != m_nVal;
	}


	const string MDBFieldValueTypeInt64::AsString()
	{
		char szVal[21] = {0,};
		sprintf_s(szVal, 21, "%I64d", m_nVal);
		return string(szVal);
	}

	const TIMESTAMP_STRUCT MDBFieldValueTypeInt64::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeInt64::AsFloat()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<float>(m_nVal);
	}

	const std::wstring MDBFieldValueTypeInt64::AsWString() 
	{
		wchar_t szVal[21] = {0,};
		_snwprintf_s(szVal, 21, 20, L"%I64d", m_nVal);
		return std::wstring(szVal);
	}
}