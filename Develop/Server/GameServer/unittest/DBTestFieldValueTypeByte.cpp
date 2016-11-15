#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeByte.h"


SUITE(DBTestFieldValueTypeByte)
{
	class MockFieldValueTypeByte : public mdb::MDBFieldValueTypeByte
	{
	public :
		MockFieldValueTypeByte() : MDBFieldValueTypeByte(0) {}

		using mdb::MDBFieldValueTypeByte::m_nVal;
	};


	class FieldValueTypeByteFixture
	{
	public :
		FieldValueTypeByteFixture()
		{
			fb.m_nVal = 34;
		}


	public :
		MockFieldValueTypeByte fb;
	};


	TEST_FIXTURE(FieldValueTypeByteFixture, CheckFieldValueTypeByte)
	{
		CHECK_EQUAL(34, fb.m_nVal);
		CHECK_EQUAL(34, fb.AsInt64());
		CHECK_EQUAL(34, fb.AsLong());
		CHECK_EQUAL(34, fb.AsInt());
		CHECK_EQUAL(34, fb.AsShort());
		CHECK_EQUAL(34, fb.AsByte());
		CHECK(L"34" == fb.AsWString());
	}


	TEST_FIXTURE(FieldValueTypeByteFixture, FieldValueTypeByteAsBool)
	{
		CHECK_EQUAL(true, fb.AsBool());
	}


	TEST_FIXTURE(FieldValueTypeByteFixture, FieldValueTypeByteConvertTimestampException)
	{
		CHECK_EQUAL(0, fb.AsTimestamp().year);
	}
}