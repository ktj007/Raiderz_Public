#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeBool.h"


SUITE(DBTestFieldValueTypeBool)
{
	class MockFieldValueTypeBool : public mdb::MDBFieldValueTypeBool
	{
	public :
		MockFieldValueTypeBool() : MDBFieldValueTypeBool(0) {}

		using mdb::MDBFieldValueTypeBool::m_bVal;
	};


	class FieldValueTypeBoolFixture
	{
	public :
		FieldValueTypeBoolFixture()
		{
			fb.m_bVal = true;
		}


	public :
		MockFieldValueTypeBool fb;
	};


	TEST_FIXTURE(FieldValueTypeBoolFixture, CheckFieldValueTypeBool)
	{
		CHECK_EQUAL(true, fb.m_bVal);
		CHECK_EQUAL(1, fb.AsInt64());
		CHECK_EQUAL(1, fb.AsLong());
		CHECK_EQUAL(1, fb.AsInt());
		CHECK_EQUAL(1, fb.AsShort());
		CHECK_EQUAL(true, fb.AsBool());
		CHECK_EQUAL(1, fb.AsByte());
		CHECK(L"true" == fb.AsWString());
		fb.m_bVal = false;
		CHECK(L"false" == fb.AsWString());
	}


	TEST_FIXTURE(FieldValueTypeBoolFixture, FieldValueTypeBoolConvertTimestampException)
	{
		CHECK_EQUAL(0, fb.AsTimestamp().year);		
	}
}