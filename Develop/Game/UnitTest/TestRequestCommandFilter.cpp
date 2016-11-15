#include "stdafx.h"
#include "XNetClient.h"
#include "XRequestCommandFilter.h"

SUITE(RequestCommandFilter)
{
	TEST(TestRequestCommandFilter__Success)
	{
		XRequestCommandFilter filter;

		filter.AddFilter(MC_CHAR_REQUEST_SEL_MYCHAR, MC_CHAR_RESPONSE_SEL_MYCHAR);

		CHECK_EQUAL(true, filter.CheckRequestEnable(MC_CHAR_REQUEST_SEL_MYCHAR));

		// 이미 한번 보냈으니까 그 다음번에는 보낼 수 없다.
		CHECK_EQUAL(false, filter.CheckRequestEnable(MC_CHAR_REQUEST_SEL_MYCHAR));

		// Response 커맨드 받음
		filter.OnResponse(MC_CHAR_RESPONSE_SEL_MYCHAR);

		// 이제 다시 request를 보낼 수 있다.
		CHECK_EQUAL(true, filter.CheckRequestEnable(MC_CHAR_REQUEST_SEL_MYCHAR));
	}

	TEST(TestRequestCommandFilter__CheckRequestEnable)
	{
		XRequestCommandFilter filter;

		filter.AddFilter(MC_CHAR_REQUEST_SEL_MYCHAR, MC_CHAR_RESPONSE_SEL_MYCHAR);

		// 필터에 없는 커맨드는 무조건 Request 가능
		CHECK_EQUAL(true, filter.CheckRequestEnable(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST));
	}

	TEST(TestRequestCommandFilter__ClearSendRequestCommands)
	{
		XRequestCommandFilter filter;

		filter.AddFilter(MC_CHAR_REQUEST_SEL_MYCHAR, MC_CHAR_RESPONSE_SEL_MYCHAR);

		CHECK_EQUAL(true, filter.CheckRequestEnable(MC_CHAR_REQUEST_SEL_MYCHAR));
		CHECK_EQUAL(false, filter.CheckRequestEnable(MC_CHAR_REQUEST_SEL_MYCHAR));

		filter.ClearSendRequestCommands();

		CHECK_EQUAL(true, filter.CheckRequestEnable(MC_CHAR_REQUEST_SEL_MYCHAR));
	}

}