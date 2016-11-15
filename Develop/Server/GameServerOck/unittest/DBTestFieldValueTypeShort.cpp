#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeShort.h"


SUITE(DBTestFieldValueTypeShort)
{
	class MockFieldValueTypeShort : public mdb::MDBFieldValueTypeShort
	{
	public :
		MockFieldValueTypeShort() : MDBFieldValueTypeShort(0) {}

		using mdb::MDBFieldValueTypeShort::m_nVal;		
	};


	class FieldValueTypeShortFixture
	{
	public :
		FieldValueTypeShortFixture()
		{
			nVal		= 12345;
			fs.m_nVal	= nVal;
		}


	public :
		short					nVal;
		MockFieldValueTypeShort fs;
	};


	TEST_FIXTURE(FieldValueTypeShortFixture, CheckFieldValueTypeShort)
	{
		CHECK_EQUAL(nVal, fs.m_nVal);
		CHECK_EQUAL(nVal, fs.AsInt64());
		CHECK_EQUAL(nVal, fs.AsLong());
		CHECK_EQUAL(nVal, fs.AsInt());
		CHECK_EQUAL(nVal, fs.AsShort());
		CHECK(nVal != fs.AsByte());
		CHECK_EQUAL(true, fs.AsBool());
		CHECK(L"12345" == fs.AsWString());
		CHECK_EQUAL(0, fs.AsTimestamp().year);
	}
}