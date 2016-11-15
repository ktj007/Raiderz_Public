#pragma once


#include "MDBFieldValueType.h"


namespace mdb
{
	class MADB_API MDBFieldValueTypeTimestamp : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeTimestamp(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeTimestamp();

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
		TIMESTAMP_STRUCT		m_Timestamp;
	};
}