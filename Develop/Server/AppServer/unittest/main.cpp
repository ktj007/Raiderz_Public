#include "stdafx.h"
#include "MBaseUnitTest.h"
#include "PCommandTable.h"
#include "SAppHelper.h"
#include "PUnitTestRunner.h"
#include "PUnitTest.h"
#include "PConfig.h"
#include "MProfiler.h"
#include "MDebug.h"
#include "PMemory.h"
#include "SProfileScope.h"
#include "PDef.h"

int RunOnlyUnitTest(LPSTR cmdline)
{
	int ret = PUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	// Stage2를 위해 App 실행
	PConfig::m_bWindowVisible = false;
	PConfig::m_nPort = 7507;	// UnitTest일 경우 포트 충돌이 나면 안되므로

	MInitStatisticsProfile(true);

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	int nUnitTestCount = 1;

	// 유닛 테스트 회수 설정
	if (strlen(cmdline) > 0)
	{
		wchar_t arg_unittest_count[256] = L"";

		int ret = sscanf(cmdline, "%s", arg_unittest_count);
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

		PUnitTestRunner	unitTestRunner;
		if (unitTestRunner.RunStage1() == false) 
		{
			ret = PUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}


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
	void ReportFailure(TestDetails const& test, wchar_t const* failure)
	{
		wchar_t writeBuffer[1024];
		swprintf_s( writeBuffer, L"%d,%s,%s", test.lineNumber, test.filename, failure );
		DWORD dwWrite;
		if( WriteFile( m_WritePipe, writeBuffer, 1024, &dwWrite, NULL ) == false || dwWrite != 1024 )
			exit(-1);
	}
	void ReportTestFinish(TestDetails const& test, float secondsElapsed) {}
	void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed) {}
};


int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	PMinetInitScope MinetInitScope;
	PScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	SAppHelper::InitLog(FILENAME_LOG);	
	PConfig::Init_INI();

	return RunOnlyUnitTest("");
}

