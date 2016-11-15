#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeTimestamp.h"


SUITE(DBTestFieldValueTypeTimestamp)
{
	class FakeField : public mdb::MDBFieldValueTypeTimestamp
	{
	public :
		FakeField() : MDBFieldValueTypeTimestamp(0)
		{
			m_Timestamp.year = 2008;
			m_Timestamp.month = 12;
			m_Timestamp.day = 30;
			m_Timestamp.hour = 3;
			m_Timestamp.minute = 56;
			m_Timestamp.second = 31;
			m_Timestamp.fraction = 132;	// 그냥 임의값.
		}
		
	};


	
	class FieldValueTypeTimestampFixture
	{
	public :
		FieldValueTypeTimestampFixture()
		{
		}


	public :
		FakeField f;
	};


	TEST_FIXTURE(FieldValueTypeTimestampFixture, CheckFieldValueTypeTimestamp)
	{
		f.SetNotNull();

		CHECK_EQUAL(0, f.AsInt64());
		CHECK_EQUAL(0, f.AsLong());
		CHECK_EQUAL(0, f.AsInt());
		CHECK_EQUAL(0, f.AsShort());
		CHECK_EQUAL(0, f.AsByte());
		CHECK_EQUAL(false, f.AsBool());
		CHECK_EQUAL(0, f.AsInt64());
		CHECK(L"2008-12-30 3:56:31" == f.AsWString());

		TIMESTAMP_STRUCT t = f.AsTimestamp();

		CHECK_EQUAL(2008, t.year);
		CHECK_EQUAL(12, t.month);
		CHECK_EQUAL(30, t.day);
		CHECK_EQUAL(3, t.hour);
		CHECK_EQUAL(56, t.minute);
		CHECK_EQUAL(31, t.second);
		CHECK_EQUAL(132, t.fraction);
	}	

	TEST_FIXTURE(FieldValueTypeTimestampFixture, CheckFieldValueTypeWhenNull)
	{
		f.SetNull();

		CHECK(L"NULL" == f.AsWString());	
	}	
}