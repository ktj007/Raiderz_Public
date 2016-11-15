#include "stdafx.h"
#include "MDBFieldValueTypeWString.h"
#include "MLocale.h"

namespace mdb
{
	MDBFieldValueTypeWString::MDBFieldValueTypeWString(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{

	}

	MDBFieldValueTypeWString::~MDBFieldValueTypeWString()
	{

	}


	bool MDBFieldValueTypeWString::GetData(MDBRecordSet* pRecordSet)
	{
		wchar_t wszValue[MDB_DEFAULT_STRING_LEN];

		SQLINTEGER	nReadLength;

		if (!pRecordSet->GetData(m_nIndex, wszValue, SQL_C_WCHAR, MDB_DEFAULT_STRING_LEN-1, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		wszValue[nReadLength] = '\0';
		m_wstrVal = std::wstring(wszValue);

		SetNotNull();

		_ASSERT((-1 != nReadLength) && !IsNull());
		
		return true;
	}


	void MDBFieldValueTypeWString::Reset()
	{
		m_wstrVal.clear();
		SetNull();
	}


	__int64 MDBFieldValueTypeWString::AsInt64()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	long MDBFieldValueTypeWString::AsLong()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	int MDBFieldValueTypeWString::AsInt()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	short MDBFieldValueTypeWString::AsShort()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	unsigned char MDBFieldValueTypeWString::AsByte()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	bool MDBFieldValueTypeWString::AsBool()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return false;
	}


	const string MDBFieldValueTypeWString::AsString()
	{
		return MLocale::ConvUTF16ToAnsi(m_wstrVal.c_str());
	}


	const TIMESTAMP_STRUCT MDBFieldValueTypeWString::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeWString::AsFloat()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0.0f;
	}

	const std::wstring MDBFieldValueTypeWString::AsWString()
	{
		return m_wstrVal;
	}
}