#include "stdafx.h"
#include "MCommandLine.h"

using namespace cml2;

SUITE(CommandLine)
{
	TEST(TestCommandLine_GetSwitchValue)
	{
		MCommandLine cl;
		cl.ParseFromString("prog.exe -aaa=1 -bbb=하하하 -ccc=\"무궁화 꽃이\" --ddd");

		string strSwitchValue;
		strSwitchValue = cl.GetSwitchValue("aaa");
		CHECK_EQUAL("1", strSwitchValue);

		strSwitchValue = cl.GetSwitchValue("bbb");
		CHECK_EQUAL("하하하", strSwitchValue);

		strSwitchValue = cl.GetSwitchValue("ccc");
		CHECK_EQUAL("무궁화 꽃이", strSwitchValue);

		strSwitchValue = cl.GetSwitchValue("ddd");
		CHECK_EQUAL("", strSwitchValue);

	}

	TEST(TestCommandLine_GetSwitchValueNegative)
	{
		MCommandLine cl;
		cl.ParseFromString("prog.exe -aaa -b bb=하하하 -ccc=\"무궁화 꽃이");

		string strSwitchValue;
		strSwitchValue = cl.GetSwitchValue("aaa");
		CHECK_EQUAL("", strSwitchValue);

		strSwitchValue = cl.GetSwitchValue("bbb");
		CHECK_EQUAL("", strSwitchValue);

		strSwitchValue = cl.GetSwitchValue("b");
		CHECK_EQUAL("", strSwitchValue);
	}

	TEST(TestCommandLine_HasSwitch)
	{
		MCommandLine cl;
		cl.ParseFromString("prog.exe -aaa=1 -bbb=하하하 -ccc=\"무궁화 꽃이\" --ddd -무궁화=111 fff ggg hhh");

		CHECK_EQUAL(true, cl.HasSwitch("aaa"));
		CHECK_EQUAL(true, cl.HasSwitch("bbb"));
		CHECK_EQUAL(true, cl.HasSwitch("ccc"));
		CHECK_EQUAL(true, cl.HasSwitch("ddd"));		// "--" 가 붙은 것도 스위치
		CHECK_EQUAL(true, cl.HasSwitch("무궁화"));

		CHECK_EQUAL(false, cl.HasSwitch("fff"));


	}

}