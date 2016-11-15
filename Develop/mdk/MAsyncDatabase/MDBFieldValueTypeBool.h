#pragma once


#include "MDBFieldValueType.h"


namespace mdb
{
	class MADB_API MDBFieldValueTypeBool : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeBool(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeBool();


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
		bool					m_bVal;
	};
}