#include "stdafx.h"
#include "MdBFieldValueTypeLong.h"
#include "MDatabaseDefine.h"


namespace mdb
{
	MDBFieldValueTypeLong::MDBFieldValueTypeLong(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{
		m_nVal = 0;
	}


	MDBFieldValueTypeLong::~MDBFieldValueTypeLong()
	{

	}


	bool MDBFieldValueTypeLong::GetData(MDBRecordSet* pRecordSet)
	{
		SQLINTEGER nReadLength;

		if (!pRecordSet->GetData(m_nIndex, &m_nVal, SQL_C_LONG, 4, nReadLength))
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


	void MDBFieldValueTypeLong::Reset()
	{
		SetNull();
		m_nVal		= 0;
	}


	__int64 MDBFieldValueTypeLong::AsInt64()
	{
		return static_cast<__int64>(m_nVal);
	}


	long MDBFieldValueTypeLong::AsLong()
	{
		return m_nVal;
	}


	int MDBFieldValueTypeLong::AsInt()
	{
		return static_cast<int>(m_nVal);
	}


	short MDBFieldValueTypeLong::AsShort()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<short>(m_nVal);
	}


	unsigned char MDBFieldValueTypeLong::AsByte()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return static_cast<unsigned char>(m_nVal);
	}


	bool MDBFieldValueTypeLong::AsBool()
	{
		_ASSERT("데이터 손실이 발생할 수 있습니다.");
		return 0 != m_nVal;
	}


	const string MDBFieldValueTypeLong::AsString()
	{
		char szVal[12] = {0,};
		sprintf_s(szVal, 12, "%d", m_nVal);
		return string(szVal);
	}

	const TIMESTAMP_STRUCT MDBFieldValueTypeLong::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeLong::AsFloat()
	{
		return static_cast<float>(m_nVal);
	}

	const std::wstring MDBFieldValueTypeLong::AsWString()
	{
		wchar_t szVal[12] = {0,};
		_snwprintf_s(szVal, 12, 11, L"%d", m_nVal);
		return std::wstring(szVal);
	}
}