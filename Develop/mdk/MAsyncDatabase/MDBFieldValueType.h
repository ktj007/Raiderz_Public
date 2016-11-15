#pragma once


#include <sqltypes.h>
#include "MADBLib.h"


namespace mdb
{
	class MDBRecordSet;


	class MADB_API MDBFieldValueType
	{
	public :
		MDBFieldValueType(const SQLSMALLINT nIndex);
		virtual ~MDBFieldValueType();


		virtual	bool					IsNull();
		virtual void					SetNull();
		virtual void					SetNotNull();
		virtual SQLSMALLINT				GetIndex();

		virtual __int64					AsInt64()		= 0;
		virtual long					AsLong()		= 0;
		virtual int						AsInt()			= 0;
		virtual short					AsShort()		= 0;
		virtual unsigned char			AsByte()		= 0;
		virtual bool					AsBool()		= 0;
		virtual const string			AsString()		= 0;
		virtual const std::wstring		AsWString()		= 0;
		virtual const TIMESTAMP_STRUCT	AsTimestamp()	= 0;
		virtual const float				AsFloat()		= 0;

		virtual void					Reset()			= 0;
		virtual bool GetData(MDBRecordSet* pRecordSet)	= 0;


	protected :
		bool							m_bIsNull;
		SQLSMALLINT						m_nIndex;
	};
}