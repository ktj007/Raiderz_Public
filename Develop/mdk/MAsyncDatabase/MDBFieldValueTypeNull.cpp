#include "stdafx.h"
#include "MDBFieldValueTypeNull.h"
#include "MDatabaseDefine.h"


namespace mdb
{
	bool MDBFieldValueTypeNull::IsNull()
	{
		return true;
	}


	bool MDBFieldValueTypeNull::GetData(MDBRecordSet* pRecordSet)
	{
		_ASSERT(IsNull());
		return true;
	}


	void MDBFieldValueTypeNull::Reset()
	{
		SetNull();
	}
	

	__int64 MDBFieldValueTypeNull::AsInt64()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	long MDBFieldValueTypeNull::AsLong()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	int	MDBFieldValueTypeNull::AsInt()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	short MDBFieldValueTypeNull::AsShort()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	unsigned char MDBFieldValueTypeNull::AsByte()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0;
	}


	bool MDBFieldValueTypeNull::AsBool()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return false;
	}


	const string MDBFieldValueTypeNull::AsString()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return string("NULL");
	}


	const TIMESTAMP_STRUCT MDBFieldValueTypeNull::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeNull::AsFloat()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		return 0.0f;
	}

	const std::wstring MDBFieldValueTypeNull::AsWString()
	{
		return std::wstring(L"NULL");
	}

	MDBFieldValueTypeNull::MDBFieldValueTypeNull(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{

	}
}