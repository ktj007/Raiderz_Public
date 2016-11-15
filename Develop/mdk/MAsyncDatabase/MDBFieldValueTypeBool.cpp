#include "stdafx.h"
#include "MDBFieldValueTypeBool.h"


namespace mdb
{
	MDBFieldValueTypeBool::MDBFieldValueTypeBool(const SQLSMALLINT nIndex) : MDBFieldValueType(nIndex)
	{
		m_bVal = false;
	}


	MDBFieldValueTypeBool::~MDBFieldValueTypeBool()
	{

	}


	bool MDBFieldValueTypeBool::GetData(MDBRecordSet* pRecordSet)
	{
		SQLINTEGER nReadLength;

		if (!pRecordSet->GetData(m_nIndex, &m_bVal, SQL_C_BIT, 1, nReadLength))
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


	void MDBFieldValueTypeBool::Reset()
	{	
		SetNull();
		m_bVal		= false;
	}


	__int64 MDBFieldValueTypeBool::AsInt64()
	{
		return static_cast<__int64>(m_bVal);
	}


	long MDBFieldValueTypeBool::AsLong()
	{
		return static_cast<long>(m_bVal);
	}


	int MDBFieldValueTypeBool::AsInt()
	{
		return static_cast<int>(m_bVal);
	}


	short MDBFieldValueTypeBool::AsShort()
	{
		return static_cast<short>(m_bVal);
	}


	unsigned char MDBFieldValueTypeBool::AsByte()
	{
		return static_cast<unsigned char>(m_bVal);
	}


	bool MDBFieldValueTypeBool::AsBool()
	{
		return m_bVal;
	}


	const string MDBFieldValueTypeBool::AsString()
	{
		if (!m_bVal)
			return std::string("false");

		return std::string("true");		
	}

	const TIMESTAMP_STRUCT MDBFieldValueTypeBool::AsTimestamp()
	{
		_ASSERT("지원하지 않는 형변환입니다.");
		TIMESTAMP_STRUCT ts;
		ts.year = 0;
		return ts;
	}

	const float MDBFieldValueTypeBool::AsFloat()
	{
		if (m_bVal)
			return 1.0f;
		else 
			return 0.0;

		return 0.0f;
	}

	const std::wstring MDBFieldValueTypeBool::AsWString()
	{
		if (!m_bVal)
			return std::wstring(L"false");

		return std::wstring(L"true");
	}
}