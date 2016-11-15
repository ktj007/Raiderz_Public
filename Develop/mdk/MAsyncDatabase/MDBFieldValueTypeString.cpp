#include "stdafx.h"
#include "MDBFieldValueTypeString.h"
#include "MDatabaseDefine.h"
#include <atlcore.h>
#include "MLocale.h"

namespace mdb
{
	MDBFieldValueTypeString::MDBFieldValueTypeString(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{

	}


	MDBFieldValueTypeString::~MDBFieldValueTypeString()
	{

	}


	bool MDBFieldValueTypeString::GetData(MDBRecordSet* pRecordSet)
	{
		char		szValue[MDB_DEFAULT_STRING_LEN];
		SQLINTEGER	nReadLength;

		if (!pRecordSet->GetData(m_nIndex, szValue, SQL_C_CHAR, MDB_DEFAULT_STRING_LEN-1, nReadLength))
		{
			return false;
		}

		if (-1 == nReadLength)
		{
			return true;
		}

		szValue[nReadLength] = '\0';
		m_strVal = string(szValue);

		SetNotNull();

		_ASSERT((-1 != nReadLength) && !IsNull());

		return true;
	}


	void MDBFieldValueTypeString::Reset()
	{
		SetNull();
		m_strVal.clear();
	}


	__int64 MDBFieldValueTypeString::AsInt64()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	long MDBFieldValueTypeString::AsLong()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	int MDBFieldValueTypeString::AsInt()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	short MDBFieldValueTypeString::AsShort()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	unsigned char MDBFieldValueTypeString::AsByte()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	bool MDBFieldValueTypeString::AsBool()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return false;
	}


	const string MDBFieldValueTypeString::AsString()
	{
		return m_strVal;
	}

	
	const TIMESTAMP_STRUCT MDBFieldValueTypeString::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeString::AsFloat()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0.0f;
	}

	const std::wstring MDBFieldValueTypeString::AsWString()
	{
		return MLocale::ConvAnsiToUCS2(m_strVal.c_str());
	}
}