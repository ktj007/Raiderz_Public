#include "stdafx.h"

using namespace testing;

//class OutputDebugStringPrinter : public EmptyTestEventListener
//{
//	void OnTestPartResult( const TestPartResult& r )
//	{
//		static char buf[1024];
//		sprintf_s( buf, "%s(%d): error: (%s)\n", r.file_name(), r.line_number(), r.summary() );
//		OutputDebugString( buf );
//	}
//};

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

	//UnitTest& unit_test = *UnitTest::GetInstance();
	//TestEventListeners& listeners = unit_test.listeners();
	//delete listeners.Release( listeners.default_result_printer() );
	//listeners.Append( new OutputDebugStringPrinter );

	//int testResult = unit_test.Run();
	int testResult = RUN_ALL_TESTS();
	//if( unit_test.Failed() )
	//{
	//	__debugbreak(); // 테스트가실패하면중지
	//}

	return testResult;
}