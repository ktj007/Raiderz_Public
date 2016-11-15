#pragma once


#include "MDBfieldValueType.h"


namespace mdb
{
	class MADB_API MDBFieldValueTypeShort : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeShort(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeShort();


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
		short					m_nVal;
	};
}