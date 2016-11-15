#include "stdafx.h"
#include "MDBFieldValueTypeByte.h"



namespace mdb
{
	MDBFieldValueTypeByte::MDBFieldValueTypeByte(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{
		m_nVal = 0;
	}


	MDBFieldValueTypeByte::~MDBFieldValueTypeByte()
	{

	}


	bool MDBFieldValueTypeByte::GetData(MDBRecordSet* pRecordSet)
	{
		SQLINTEGER nReadLength;

		if (!pRecordSet->GetData(m_nIndex, &m_nVal, SQL_C_TINYINT, 1, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		if (1 != nReadLength)
		{
			return false;
		}

		SetNotNull();

		_ASSERT((-1 != nReadLength) && (1 == nReadLength) && !IsNull());

		return true;
	}


	void MDBFieldValueTypeByte::Reset()
	{
		SetNull();
		m_nVal		= 0;
	}


	__int64 MDBFieldValueTypeByte::AsInt64()
	{
		return static_cast<__int64>(m_nVal);
	}


	long MDBFieldValueTypeByte::AsLong()
	{
		return static_cast<long>(m_nVal);
	}


	int MDBFieldValueTypeByte::AsInt()
	{
		return static_cast<int>(m_nVal);
	}


	short MDBFieldValueTypeByte::AsShort()
	{
		return static_cast<short>(m_nVal);
	}


	unsigned char MDBFieldValueTypeByte::AsByte()
	{
		return m_nVal;
	}


	bool MDBFieldValueTypeByte::AsBool()
	{
		return 0 != m_nVal;
	}


	const string MDBFieldValueTypeByte::AsString()
	{
		char szVal[12] = {0,};
		_snprintf_s(szVal, 12, 11, "%d", m_nVal);
		return string(szVal);
	}

	const TIMESTAMP_STRUCT MDBFieldValueTypeByte::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeByte::AsFloat()
	{
		return static_cast<float>(m_nVal);
	}

	const std::wstring MDBFieldValueTypeByte::AsWString()
	{
		wchar_t wszVal[12];

		_snwprintf_s(wszVal, 12, 11, L"%d", m_nVal);

		return std::wstring(wszVal);
	}
}