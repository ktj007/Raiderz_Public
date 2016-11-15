#include "stdafx.h"
#include "MDBFieldValueTypeFloat.h"


namespace mdb
{
	MDBFieldValueTypeFloat::MDBFieldValueTypeFloat(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{

	}

	MDBFieldValueTypeFloat::~MDBFieldValueTypeFloat()
	{

	}

	__int64 MDBFieldValueTypeFloat::AsInt64()
	{
		_ASSERT("데이터가 손실될 수 있습니다.");

		return static_cast<__int64>(m_fVal);
	}

	long MDBFieldValueTypeFloat::AsLong()
	{		
		_ASSERT("데이터가 손실될 수 있습니다.");

		return static_cast<long>(m_fVal);
	}

	int MDBFieldValueTypeFloat::AsInt()
	{
		_ASSERT("데이터가 손실될 수 있습니다.");

		return static_cast<int>(m_fVal);
	}

	short MDBFieldValueTypeFloat::AsShort()
	{
		_ASSERT("데이터가 손실될 수 있습니다.");

		return static_cast<short>(m_fVal);
	}

	unsigned char MDBFieldValueTypeFloat::AsByte()
	{
		_ASSERT("데이터가 손실될 수 있습니다.");

		return static_cast<unsigned char>(m_fVal);
	}

	bool MDBFieldValueTypeFloat::AsBool()
	{
		return 0.0f != m_fVal;
	}

	const string MDBFieldValueTypeFloat::AsString()
	{
		char szVal[16] = {0,};
		sprintf_s(szVal, 16, "%f", m_fVal);
		return string(szVal);
	}

	const TIMESTAMP_STRUCT MDBFieldValueTypeFloat::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeFloat::AsFloat()
	{
		return m_fVal;
	}

	void MDBFieldValueTypeFloat::Reset()
	{
		SetNull();
		m_fVal		= 0.0f;
	}

	bool MDBFieldValueTypeFloat::GetData( MDBRecordSet* pRecordSet)
	{
		SQLINTEGER nReadLength;

		if (!pRecordSet->GetData(m_nIndex, &m_fVal, SQL_REAL, 4, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		if (4 != nReadLength)
		{
			return false;
		}

		SetNotNull();

		_ASSERT((-1 != nReadLength) && (4 == nReadLength) && !IsNull());

		return true;
	}

	const std::wstring MDBFieldValueTypeFloat::AsWString()
	{
		wchar_t wszVal[16];
		_snwprintf_s(wszVal, 16, 15, L"%f", m_fVal);
		return std::wstring(wszVal);
	}
}