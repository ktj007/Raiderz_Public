#include "stdafx.h"
#include "MBaseUnitTest.h"
#include "LCommandTable.h"
#include "SAppHelper.h"
#include "LUnitTestRunner.h"
#include "SUnitTest.h"
#include "LConfig.h"
#include "MProfiler.h"
#include "MDebug.h"
#include "LMemory.h"
#include "SProfileScope.h"
#include "LDef.h"

int RunOnlyUnitTest(LPWSTR cmdline)
{
	int ret = LUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	// Stage2를 위해 App 실행
	LConfig::m_bWindowVisible = false;
	LConfig::m_nPort = 7507;	// UnitTest일 경우 포트 충돌이 나면 안되므로

	MInitStatisticsProfile(true);

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	int nUnitTestCount = 1;

	// 유닛 테스트 회수 설정
	if (wcslen(cmdline) > 0)
	{
		wchar_t arg_unittest_count[256] = L"";

		int ret = swscanf(cmdline, L"%s", arg_unittest_count);
		if (ret == 1)
		{
			nUnitTestCount = _wtoi(arg_unittest_count);
			if (nUnitTestCount <= 0) nUnitTestCount = 1;

			unittest_config.m_nRepeat = nUnitTestCount;
		}
	}

	// stage1 --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE1_NAME))
	{
		if (unittest_config.m_nRepeat > 1)
		{
			mlog("UnitTest Repeat Count = %d\n", unittest_config.m_nRepeat);
		}

		LUnitTestRunner	unitTestRunner;
		if (unitTestRunner.RunStage1() == false) 
		{
			ret = LUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}


	//// gamedb --
	//if (unittest_config.CheckStageFilter(UNITTEST_STAGE_GAMEDB_NAME)
	//	&& unittest_config.m_bExecuteDB)
	//{
	//	LUnitTestRunner	unitTestRunner;
	//	if (unitTestRunner.RunGameDBUnitTest() == false) 
	//	{
	//		ret = LUnitTestRunner	::CCNET_UNITTEST_FAILED;
	//	}
	//}

	return ret;
}



using namespace UnitTest;

class VUTPP_Reporter : public TestReporter
{
private:
	HANDLE m_WritePipe;

public:
	VUTPP_Reporter(HANDLE writePipe) : m_WritePipe(writePipe){}
	~VUTPP_Reporter() {}

	void ReportTestStart(TestDetails const& test) {}
	void ReportFailure(TestDetails const& test, char const* failure)
	{
		char writeBuffer[1024];
		sprintf( writeBuffer, "%d,%s,%s", test.lineNumber, test.filename, failure );
		DWORD dwWrite;
		if( WriteFile( m_WritePipe, writeBuffer, 1024, &dwWrite, NULL ) == false || dwWrite != 1024 )
			exit(-1);
	}
	void ReportTestFinish(TestDetails const& test, float secondsElapsed) {}
	void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed) {}
};

int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	LMinetInitScope MinetInitScope;
	LScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	SAppHelper::InitLog(FILENAME_LOG);
	LConfig::Init_INI();

	return RunOnlyUnitTest(L"");
}
