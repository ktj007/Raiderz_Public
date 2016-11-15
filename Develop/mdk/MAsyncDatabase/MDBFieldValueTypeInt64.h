#pragma once


#include "MDBFieldValueType.h"


namespace mdb
{
	class MADB_API MDBFieldValueTypeInt64 : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeInt64(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeInt64();

		__int64					AsInt64();
		long					AsLong();
		int						AsInt();
		short					AsShort();
		unsigned char			AsByte();
		bool					AsBool();
		const string			AsString();
		const std::wstring		AsWString();
		const TIMESTAMP_STRUCT	AsTimestamp();
		const float				AsFloat();

		void					Reset();
		bool					GetData(MDBRecordSet* pRecordSet);


	protected :
		__int64					m_nVal;
	};
}