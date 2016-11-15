#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeLong.h"


SUITE(DBTestFielValueTypeLong)
{
	class MockFieldValueTypeLong : public mdb::MDBFieldValueTypeLong
	{
	public :
		MockFieldValueTypeLong() : MDBFieldValueTypeLong(0) {}

		using mdb::MDBFieldValueTypeLong::m_nVal;
	};


	class FieldValueTypeLongFixture
	{
	public :
		FieldValueTypeLongFixture()
		{
			nVal		= 1234567890;
			fl.m_nVal	= nVal;
		}


	public :
		long					nVal;
		MockFieldValueTypeLong	fl;
	};


	TEST_FIXTURE(FieldValueTypeLongFixture, CheckFieldValueTypeLong)
	{
		CHECK_EQUAL(1234567890, nVal);
		CHECK_EQUAL(nVal, fl.m_nVal);
		CHECK_EQUAL(nVal, fl.AsInt64());
		CHECK_EQUAL(nVal, fl.AsLong());
		CHECK_EQUAL(nVal, fl.AsInt());
		CHECK(nVal != fl.AsShort());
		CHECK(nVal != fl.AsByte());
		CHECK_EQUAL(true, fl.AsBool());
		CHECK(L"1234567890" == fl.AsWString());
	}


	TEST_FIXTURE(FieldValueTypeLongFixture, FieldValueTypeLongConvertTimestampException)
	{
			CHECK_EQUAL(0, fl.AsTimestamp().year);		
	}
}