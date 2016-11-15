#pragma once


#include "MDBFieldValueType.h"


namespace mdb
{
	class MDBFieldValueTypeWString : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeWString(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeWString();

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
		std::wstring m_wstrVal;
	};
}