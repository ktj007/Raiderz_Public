#pragma once


#include "MDBFieldValueTypeHeaderList.h"


namespace mdb
{
	class MDBFieldValueTypeFactory
	{
	public :
		MDBFieldValueTypeFactory() {}
		~MDBFieldValueTypeFactory() {}

		static MDBFieldValueType* Create(const SQLSMALLINT nSQLType, SQLSMALLINT nIndex);


	private :
		static MDBFieldValueTypeNull m_FieldNull;
	};
}