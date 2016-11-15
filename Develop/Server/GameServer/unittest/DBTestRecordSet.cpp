#include "stdafx.h"
#include "GUnitTest.h"
#include "MDBRecordSet.h"


SUITE(DBRecordSet)
{
	class MockDBRecordSet : public mdb::MDBRecordSet
	{
	public :
		using mdb::MDBRecordSet::m_FieldMap;
		using mdb::MDBRecordSet::m_FieldVec;
	};


	class DBRecordSetFixture
	{
	public :
		DBRecordSetFixture()
		{
			strField	= string("TestField");
			nHash		= rs.MakeHashValue(strField);

			mdb::MDBField f;
			f.SetNull(false);
			
			const mdb::MDBRecordSet::MDB_FIELD_MAP_RETURN ret = rs.m_FieldMap.insert(mdb::MDBRecordSet::MDB_FIELD_MAP::value_type(nHash, f));
			rs.m_FieldVec.push_back(&(ret.first->second));
		}


	public :
		string			strField;
		size_t			nHash;
		MockDBRecordSet rs;
	};


	//TEST_FIXTURE(DBRecordSetFixture, CheckRecordSetFieldName)
	//{
	//	CHECK(!rs.Field(strField).IsNull());		
	//}


	//TEST_FIXTURE(DBRecordSetFixture, CheckRecordSetFieldInvalidName)
	//{
	//	const string strInvalid("invalid");

	//	CHECK(rs.Field(strInvalid).IsNull());
	//	CHECK_EQUAL(0, rs.Field(strInvalid).AsInt64());
	//	CHECK_EQUAL(0, rs.Field(strInvalid).AsLong());
	//	CHECK_EQUAL(0, rs.Field(strInvalid).AsInt());
	//	CHECK_EQUAL(0, rs.Field(strInvalid).AsShort());
	//	CHECK_EQUAL(0, rs.Field(strInvalid).AsByte());
	//	CHECK_EQUAL(false, rs.Field(strInvalid).AsBool());
	//	CHECK(rs.Field(strInvalid).AsString().empty());
	//	CHECK_EQUAL(0, rs.Field(strInvalid).AsTimestamp().year);
	//}


	//TEST_FIXTURE(DBRecordSetFixture, CheckRecordSetFieldIndex)
	//{
	//	CHECK(!rs.Field(0).IsNull());		
	//}


	//TEST_FIXTURE(DBRecordSetFixture, CheckRecordSetFieldInvalidIndex)
	//{
	//	const size_t nInvalid = 100;

	//	CHECK(rs.Field(nInvalid).IsNull());
	//	CHECK_EQUAL(0, rs.Field(nInvalid).AsInt64());
	//	CHECK_EQUAL(0, rs.Field(nInvalid).AsLong());
	//	CHECK_EQUAL(0, rs.Field(nInvalid).AsInt());
	//	CHECK_EQUAL(0, rs.Field(nInvalid).AsShort());
	//	CHECK_EQUAL(0, rs.Field(nInvalid).AsByte());
	//	CHECK_EQUAL(false, rs.Field(nInvalid).AsBool());
	//	CHECK(rs.Field(nInvalid).AsString().empty());
	//	CHECK_EQUAL(0, rs.Field(nInvalid).AsTimestamp().year);
	//}


	//TEST_FIXTURE(DBRecordSetFixture, CheckRecordSetFieldHash)
	//{
	//	CHECK(!rs.FieldHash(nHash).IsNull());		
	//}


	//TEST_FIXTURE(DBRecordSetFixture, CheckRecordSetFieldInvalidHash)
	//{
	//	const size_t nInvalid = 0;

	//	CHECK(rs.FieldHash(nInvalid).IsNull());
	//	CHECK_EQUAL(0, rs.FieldHash(nInvalid).AsInt64());
	//	CHECK_EQUAL(0, rs.FieldHash(nInvalid).AsLong());
	//	CHECK_EQUAL(0, rs.FieldHash(nInvalid).AsInt());
	//	CHECK_EQUAL(0, rs.FieldHash(nInvalid).AsShort());
	//	CHECK_EQUAL(0, rs.FieldHash(nInvalid).AsByte());
	//	CHECK_EQUAL(false, rs.FieldHash(nInvalid).AsBool());
	//	CHECK(rs.FieldHash(nInvalid).AsString().empty());
	//	CHECK_EQUAL(0, rs.FieldHash(nInvalid).AsTimestamp().year);		
	//}	
}