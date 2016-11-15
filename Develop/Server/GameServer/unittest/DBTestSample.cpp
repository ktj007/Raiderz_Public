#include "stdafx.h"
#include "SUnitTest.h"

SUITE(DBTestSample)
{
	GAMEDB_TEST(TestCreateCharacter)
	{
		CHECK(true);
	}


struct FSampleFixture
{
	FSampleFixture()
	{

	}
	~FSampleFixture()
	{

	}
};

	GAMEDB_TEST_FIXTURE(FSampleFixture, TestSampleFixture)
	{
		CHECK_EQUAL(1,1);
	}
}