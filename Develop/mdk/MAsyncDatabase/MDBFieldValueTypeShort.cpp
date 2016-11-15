#include "stdafx.h"
#include "MDBFieldValueTypeShort.h"
#include "MDatabaseDefine.h"


namespace mdb
{
	MDBFieldValueTypeShort::MDBFieldValueTypeShort(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{
		m_nVal = 0;
	}


	MDBFieldValueTypeShort::~MDBFieldValueTypeShort()
	{

	}


	bool MDBFieldValueTypeShort::GetData(MDBRecordSet* pRecordSet)
	{
		SQLINTEGER nReadLength;

		if (!pRecordSet->GetData(m_nIndex, &m_nVal, SQL_C_SHORT, 2, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		if (2 != nReadLength)
		{
			return false;
		}

		SetNotNull();

		_ASSERT((-1 != nReadLength) && (2 == nReadLength) && !IsNull());

		return true;
	}


	void MDBFieldValueTypeShort::Reset()
	{
		SetNull();
		m_nVal		= 0;
	}


	__int64 MDBFieldValueTypeShort::AsInt64()
	{
		return static_cast<__int64>(m_nVal);
	}


	long MDBFieldValueTypeShort::AsLong()
	{
		return static_cast<long>(m_nVal);
	}


	int	MDBFieldValueTypeShort::AsInt()
	{
		return static_cast<int>(m_nVal);
	}


	short MDBFieldValueTypeShort::AsShort()
	{
		return m_nVal;
	}


	unsigned char MDBFieldValueTypeShort::AsByte()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<unsigned char>(m_nVal);
	}


	bool MDBFieldValueTypeShort::AsBool()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return 0 != m_nVal;
	}


	const string MDBFieldValueTypeShort::AsString()
	{
		char szVal[7] = {0,};
		sprintf_s(szVal, 7, "%d", m_nVal);
		return string(szVal);
	}


	const TIMESTAMP_STRUCT MDBFieldValueTypeShort::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}	

	const float MDBFieldValueTypeShort::AsFloat()
	{
		return static_cast<float>(m_nVal);
	}

	const std::wstring MDBFieldValueTypeShort::AsWString()
	{
		wchar_t szVal[7] = {0,};
		_snwprintf_s(szVal, 7, 6, L"%d", m_nVal);
		return std::wstring(szVal);
	}
}