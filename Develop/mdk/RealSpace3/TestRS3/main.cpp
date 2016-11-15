#include "stdafx.h"

#include <vector>
#include <string>
#include "MFileSystem.h"
#include "MCrashDump.h"

// 디버그 모드에서 실행 할 때 너무 느릴 경우 주석 처리하시면 됩니다.
#include "vld.h"
#pragma comment(lib,"vldmt.lib")

#ifdef _DEBUG
#pragma comment ( lib, "UnitTest++.vsnet2008d_NoException.lib" )
#else
#pragma comment ( lib, "UnitTest++.vsnet2008.lib" )
#endif

//#define _USE_WINDOW

class RRealSpaceUnitTestReporter : public UnitTest::TestReporter
{
private:

public:
	RRealSpaceUnitTestReporter()
	{
	}
	virtual ~RRealSpaceUnitTestReporter()
	{
	}

	virtual void ReportTestStart(UnitTest::TestDetails const& test)
	{
		printf("%s , %s \n{\n", test.suiteName, test.testName);
		mlog("%s , %s \n{\n", test.suiteName, test.testName);
	}

	virtual void ReportFailure(UnitTest::TestDetails const& test, char const* failure)
	{
		printf("\t %s(%d): error %s in %s\n", test.filename, test.lineNumber, failure, test.testName);
		mlog("\t %s(%d): error %s in %s\n", test.filename, test.lineNumber, failure, test.testName);
	}
	virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed)
	{
		printf("\t %s Test time : %5.2f sec\n", test.testName, secondsElapsed);
		printf("}\n");

		mlog("\t %s Test time : %5.2f sec\n", test.testName, secondsElapsed);
		mlog("}\n");
	}
	virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
	{
		printf("Suit Total count : %d, ", totalTestCount);
		printf("Suit Failed Test count : %d, ", failedTestCount);
		printf("Suit Failure count : %d\n", failureCount);
		printf("Suit Test time : %5.2f sec\n", secondsElapsed);

		mlog("Suit Total count : %d, ", totalTestCount);
		mlog("Suit Failed Test count : %d, ", failedTestCount);
		mlog("Suit Failure count : %d\n", failureCount);
		mlog("Suit Test time : %5.2f sec\n", secondsElapsed);
	}
};

int RunUnitTests(const std::vector< std::string >& _rTestList)
{
	RRealSpaceUnitTestReporter reporter;

	int failure_count = 0;
	for (std::vector< std::string >::const_iterator itr = _rTestList.begin(); itr != _rTestList.end(); ++itr )
	{
		RUnitTestRunner::GetInstance().InitLog("TestRS3/logs/", itr->c_str() );
		failure_count += UnitTest::RunAllTests(reporter, UnitTest::Test::GetTestList(), itr->c_str(), 0); 
		RUnitTestRunner::GetInstance().FinalizeLog();
	}

	return failure_count;
}

#ifdef _USE_WINDOW
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Handle messages
	switch (message)
	{
	case WM_SYSCHAR:
	case WM_SYSCOMMAND:
		break;

	case WM_ACTIVATEAPP:
		break;
	case WM_DESTROY:
		break;

	case WM_CLOSE:
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

#endif

#ifdef _USE_WINDOW
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	const char* Win32ClassName = "UnitTest";

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= Win32ClassName;
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;

	HWND hWnd = CreateWindow( Win32ClassName, "UnitTest Win32",
		style, 100, 100, REngine::GetConfig().m_nWidth, REngine::GetConfig().m_nHeight,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow( hWnd );

	bool bConsole = false;
#else

int main( int argc, char *argv[] )
{
	MCreateFileSystem( MFILEACCESS_GENERIC, "../../EngineRes;../Data");

	RUnitTestRunner::GetInstance().InitLog("../logs/", "TestRS3" );

#ifndef _DEBUG
	MCrashDump::Init(NULL, NULL, false);
#endif

	bool bMakeReferrenceImage = false;

	if ( (argc == 2) && _stricmp( argv[1], "/r" ) == 0 )
		bMakeReferrenceImage = true;

	RUnitTestRunner::GetInstance().Init("TestRS3", "..", "../Results_ScreenShotTest.xml", bMakeReferrenceImage);

	//HWND hWnd = GetConsoleWindow();
	HWND hWnd = GetDesktopWindow();
	bool bConsole = true;

#endif

	//bool bGrabScreenShot = false;
	bool bGrabScreenShot = true;

	std::vector< std::string > aTestList;
	aTestList.push_back( "Environment" );
	aTestList.push_back( "Map" );
	aTestList.push_back( "Actor" );
	aTestList.push_back( "Effect" );
	aTestList.push_back( "AttachEffect" );
	aTestList.push_back( "Shadow" );
	aTestList.push_back( "Tree" );
	aTestList.push_back( "Water" );
	aTestList.push_back( "PostEffect" );
	aTestList.push_back( "DECAL" );
	aTestList.push_back( "Terrain" );
	aTestList.push_back( "ETC" );

	REngine::GetConfig().m_nWidth = 800;
	REngine::GetConfig().m_nHeight = 600;
	REngine::GetConfig().m_bFullScreen = false;
	REngine::GetConfig().m_bUsingShader = true;

	bool bInitialized = RUnitTestRunner::GetInstance().InitEngine(hWnd, bConsole, bGrabScreenShot);
	if(!bInitialized)
	{
		mlog("Engine Initialize failed\n");
		return 1;
	}

	RUnitTestRunner::GetInstance().FinalizeLog();

	int nResult = RunUnitTests(aTestList);

	nResult += RUnitTestRunner::GetInstance().GetFailedShotCount();

	RUnitTestRunner::GetInstance().HaltEngine();

	RUnitTestRunner::GetInstance().Destroy();

	return nResult;
}
