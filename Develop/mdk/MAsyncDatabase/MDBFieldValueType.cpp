#include "stdafx.h"
#include "MDBFieldValueType.h"


namespace mdb
{
	MDBFieldValueType::MDBFieldValueType(const SQLSMALLINT nIndex) 
	{
		SetNull();
		m_nIndex = nIndex;
	}


	MDBFieldValueType::~MDBFieldValueType() 
	{

	}


	bool MDBFieldValueType::IsNull()
	{
		return m_bIsNull;
	}

	void MDBFieldValueType::SetNull()
	{
		m_bIsNull	= true;
		m_nIndex	= -1;
	}

	void MDBFieldValueType::SetNotNull()
	{
		m_bIsNull = false;
	}

	SQLSMALLINT MDBFieldValueType::GetIndex()
	{
		return m_nIndex;
	}
}