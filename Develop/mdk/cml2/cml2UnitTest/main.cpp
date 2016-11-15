
#include <stdio.h>
#include <tchar.h>
#include "gtest/gtest.h"

using namespace testing;

int _tmain( int argc, _TCHAR* argv[] )
{
	// visual studio output 창으로 보여주기위해
	// Send all reports to STDOUT
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

	std::cout << "Running main() from RS3UnitTest.cpp\n";
	InitGoogleTest( &argc, argv );
	int testResult = RUN_ALL_TESTS();

	return testResult;
}