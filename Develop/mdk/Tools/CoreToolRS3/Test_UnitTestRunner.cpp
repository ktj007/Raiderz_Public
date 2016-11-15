#include "StdAfx.h"
#include "RUnitTestRunner.h"
#include "RUnitTestReporter.h"

#include "Test_UnitTestRunner.h"
#include "UnitTest++.h"

#ifdef _DEBUG
	#pragma comment ( lib, "UnitTest++.vsnet2008MDd_NoException.lib" )
#else
	#pragma comment ( lib, "UnitTest++.vsnet2008MD.lib" )
#endif

int RunUnitTest()
{
	rs3::RUnitTestRunner::GetInstance().InitMockEngine();

	mlog("\n=============== UnitTest ===============\n");

	const char* TEST_XML_FILENAME = "Result_UnitTest.xml";

	std::ofstream f(TEST_XML_FILENAME);
	RXmlAndStdTestReporter reporter(f);  
	int failure_count = UnitTest::RunAllTests(reporter, UnitTest::Test::GetTestList(), NULL, 0); 

	rs3::RUnitTestRunner::GetInstance().HaltMockEngine();

	return failure_count;
}