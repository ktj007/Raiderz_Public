#include "stdafx.h"
#include "MDBFieldValueTypeFactory.h"


namespace mdb
{
	MDBFieldValueTypeNull MDBFieldValueTypeFactory::m_FieldNull(-1);


	MDBFieldValueType* MDBFieldValueTypeFactory::Create(const SQLSMALLINT nSQLType, SQLSMALLINT nIndex)
	{
		switch (nSQLType)
		{
		case SQL_C_SBIGINT :
		case SQL_BIGINT :
			{	
				return new MDBFieldValueTypeInt64(nIndex);
			}
			break;

		case SQL_C_LONG : 
			{
				return new MDBFieldValueTypeLong(nIndex);
			}
			break;

		case SQL_C_SHORT :
			{
				return new MDBFieldValueTypeShort(nIndex);
			}
			break;

		case SQL_C_TINYINT :
			{
				return new MDBFieldValueTypeByte(nIndex);
			}
			break;

		case SQL_CHAR :
		case SQL_VARCHAR :
			{
				return new MDBFieldValueTypeString(nIndex);
			}
			break;

		case SQL_WCHAR :
		case SQL_WVARCHAR :
			{
				return new MDBFieldValueTypeWString(nIndex);
			}
			break;

		case SQL_C_BIT :
			{
				return new MDBFieldValueTypeBool(nIndex);
			}
			break;

		case SQL_C_TYPE_TIMESTAMP :
			{
				return new MDBFieldValueTypeTimestamp(nIndex);
			}
			break;

		case SQL_TYPE_NULL :
			{
				return new MDBFieldValueTypeNull(nIndex);
			}
			break;

		case SQL_REAL :
			{
				return new MDBFieldValueTypeFloat(nIndex);
			}
			break;

		case SQL_FLOAT :
			{
				return new MDBFieldValueTypeFloat(nIndex);
			}
			break;

		default :
			{
				_ASSERT(0 && "아직 구현되지 않은 타입. 추가해야함.");
			}
			break;
		}

		return new MDBFieldValueTypeNull(nIndex);
	}
}