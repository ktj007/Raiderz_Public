#include "stdafx.h"
#include "XWhoISRequester.h"

SUITE(WhoIsRequester)
{
	class TestWhoIsRequester : public XWhoIsRequester
	{
	public:
		TestWhoIsRequester() {}

		using XWhoIsRequester::IsExist;
	};

	class FWhoIsRequester
	{
	public:
		FWhoIsRequester() {}
		~FWhoIsRequester() {}
		TestWhoIsRequester	req;
	};

	TEST_FIXTURE(FWhoIsRequester, TestWhoIsRequester_Request_Simple)
	{
		req.Request(1);
		CHECK_EQUAL(1, req.GetSize());
		CHECK_EQUAL(true, req.IsExist(1));
	}

	TEST_FIXTURE(FWhoIsRequester, TestWhoIsRequester_Request)
	{
		req.Request(1);
		req.Request(2000);
		req.Request(999);

		CHECK_EQUAL(3, req.GetSize());
		CHECK_EQUAL(true, req.IsExist(1));
		CHECK_EQUAL(true, req.IsExist(999));
		CHECK_EQUAL(true, req.IsExist(2000));
	}


	TEST_FIXTURE(FWhoIsRequester, TestWhoIsRequester_Erase)
	{
		req.Request(1);
		req.Request(2);
		req.Request(300);

		// erase
		req.Erase(300);

		CHECK_EQUAL(2, req.GetSize());
		CHECK_EQUAL(true, req.IsExist(1));
		CHECK_EQUAL(true, req.IsExist(2));
		CHECK_EQUAL(false, req.IsExist(300));
	}

}