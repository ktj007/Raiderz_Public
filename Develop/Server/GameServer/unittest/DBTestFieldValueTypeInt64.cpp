#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeInt64.h"


SUITE(DBTestFieldValueTypeInt64)
{
	class MockFieldValueTypeInt64 : public mdb::MDBFieldValueTypeInt64
	{
	public :
		MockFieldValueTypeInt64() : MDBFieldValueTypeInt64(0) {}

		using mdb::MDBFieldValueTypeInt64::m_nVal;
	};


	class FieldValueTypeInt64Fixture
	{
	public :
		FieldValueTypeInt64Fixture()
		{
			nVal		= 123456789101112;
			fi.m_nVal	= nVal;
		}


	public :
		int64					nVal;
		MockFieldValueTypeInt64 fi;
	};


	TEST_FIXTURE(FieldValueTypeInt64Fixture, CheckFieldValueTypeInt64)
	{
		CHECK_EQUAL(nVal, fi.m_nVal);
		CHECK_EQUAL(nVal, fi.AsInt64());
		CHECK(nVal != fi.AsLong());
		CHECK(nVal != fi.AsInt());
		CHECK(nVal != fi.AsShort());
		CHECK(nVal != fi.AsByte());
		CHECK(L"123456789101112" == fi.AsWString());
	}


	TEST_FIXTURE(FieldValueTypeInt64Fixture, FieldValueTypeInt64ConvertTimestampException)
	{
		CHECK_EQUAL(0, fi.AsTimestamp().year);		
	}


	TEST_FIXTURE(FieldValueTypeInt64Fixture, FieldValueTypeInt64AsBool)
	{
		CHECK_EQUAL(true, fi.AsBool());
	}
}