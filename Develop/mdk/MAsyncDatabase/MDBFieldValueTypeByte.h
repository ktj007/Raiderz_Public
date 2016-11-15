#pragma once


#include "MDBFieldValueType.h"


namespace mdb
{
	class MADB_API MDBFieldValueTypeByte : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeByte(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeByte();


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
		unsigned char			m_nVal;
	};
}