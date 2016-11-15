#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeNull.h"


SUITE(DBTestFieldValueTypeNull)
{
	class FakeField : public mdb::MDBFieldValueTypeNull
	{
	public :
		FakeField() : MDBFieldValueTypeNull(0) {}
		
	};

	class FieldValueTypeNullFixture
	{
	public :
		FieldValueTypeNullFixture()
		{
			
		}


	public :
		FakeField f;
	};


	TEST_FIXTURE(FieldValueTypeNullFixture, CheckFieldValueTypeNull)
	{
		CHECK_EQUAL(0, f.AsInt64());
		CHECK_EQUAL(0, f.AsLong());
		CHECK_EQUAL(0, f.AsInt());
		CHECK_EQUAL(0, f.AsShort());
		CHECK_EQUAL(0, f.AsByte());
		CHECK_EQUAL(false, f.AsBool());
		CHECK(L"NULL" == f.AsWString());
		CHECK_EQUAL(0, f.AsTimestamp().year);
	}	
}