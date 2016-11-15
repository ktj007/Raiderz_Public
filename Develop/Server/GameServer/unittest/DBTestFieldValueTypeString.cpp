#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBFieldValueTypeString.h"


SUITE(DBTestFieldValueTypeString)
{
	class FakeField : public mdb::MDBFieldValueTypeString
	{
	public :
		FakeField() : MDBFieldValueTypeString(0)
		{
			m_strVal = "test";
		}
		
	};

	class FieldValueTypeStringFixture
	{
	public :
		FieldValueTypeStringFixture()
		{
		}


	public :
		FakeField f;
	};


	TEST_FIXTURE(FieldValueTypeStringFixture, CheckFieldValueTypeString)
	{
		CHECK_EQUAL(0, f.AsInt64());
		CHECK_EQUAL(0, f.AsLong());
		CHECK_EQUAL(0, f.AsInt());
		CHECK_EQUAL(0, f.AsShort());
		CHECK_EQUAL(0, f.AsByte());
		CHECK_EQUAL(false, f.AsBool());
		CHECK(L"test" == f.AsWString());
		CHECK_EQUAL(0, f.AsTimestamp().year);
	}
}