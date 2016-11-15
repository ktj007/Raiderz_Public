#pragma once


#include "MDBFieldValueType.h"


namespace mdb
{
	class MADB_API MDBFieldValueTypeFloat : public MDBFieldValueType
	{
	public :
		MDBFieldValueTypeFloat(const SQLSMALLINT nIndex);
		~MDBFieldValueTypeFloat();

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


	private :
		float					m_fVal;
	};
};