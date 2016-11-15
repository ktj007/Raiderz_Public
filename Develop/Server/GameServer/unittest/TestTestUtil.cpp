#include "StdAfx.h"
#include "GTestForward.h"
#include "GFieldMgr.h"

class TestClass_ForCallCounter : public TestCallCounter
{
public:
	void SomeFunc()		{ INCREASE_CALLCOUNTER; }
};

SUITE(TestTestUtil)
{
	TEST(NewID)
	{
		CHECK(SUnitTestUtil::NewID() != INVALID_ID);
		CHECK(SUnitTestUtil::NewID(777) == 777);
	}

	TEST(NewUID)
	{
		CHECK(SUnitTestUtil::NewUID().IsValid());
		CHECK(SUnitTestUtil::NewUID(MUID(123,456)) == MUID(123,456));
	}

	TEST(CallCounter)
	{
		TestClass_ForCallCounter testObj;
		CHECK_EQUAL(0, testObj.GetCallCounter(L"TestClass_ForCallCounter::SomeFunc"));
		testObj.SomeFunc();
		CHECK_EQUAL(1, testObj.GetCallCounter(L"TestClass_ForCallCounter::SomeFunc"));
		testObj.SomeFunc();
		CHECK_EQUAL(2, testObj.GetCallCounter(L"TestClass_ForCallCounter::SomeFunc"));
	}
}