#include "stdafx.h"
#include "GMemory.h"
#include "GCommandTable.h"
#include "GConst.h"
#include "GConfig_Arg.h"
#include "SUnitTest.h"
#include "GConfig.h"
#include "GDef.h"
#include "SAppHelper.h"
#include "SProfileScope.h"
#include "GUnitTestRunner.h"
#include "tbb/task_scheduler_init.h"
#include "SGoogleMockListener.h"
#include "GCrashReporter.h"
#include "MFileSystem.h"
#include "CSFormatString.h"


#define FILENAME_LOG_FOR_UNITTEST		L"mlog_unittest.txt"

int RunOnlyUnitTest(LPWSTR cmdline)
{
	int ret = GUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	// Stage2를 위해 App 실행
	GConfig::m_bWindowVisible = false;
	GConfig::m_nPort = 7407;	// UnitTest일 경우 포트 충돌이 나면 안되므로

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

	GUnitTestRunner::GetInst();

	// stage1 --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE1_NAME))
	{
		if (unittest_config.m_nRepeat > 1)
		{
			mlog("UnitTest Repeat Count = %d\n", unittest_config.m_nRepeat);
		}

		if (GUnitTestRunner::GetInst().RunStage1() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	// stage2 --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE2_NAME))
	{
		if (GUnitTestRunner::GetInst().RunStage2() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	// stage3 --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE3_NAME))
	{
		if (GUnitTestRunner::GetInst().RunStage3() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	// stage4 --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE4_NAME))
	{
		if (GUnitTestRunner::GetInst().RunStage4() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	// accountdb --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE_ACCDB_NAME)
		&& unittest_config.m_bExecuteDB)
	{
		if (GUnitTestRunner::GetInst().RunAccDBUnitTest() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	// gamedb --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE_GAMEDB_NAME)
		&& unittest_config.m_bExecuteDB)
	{
		if (GUnitTestRunner::GetInst().RunGameDBUnitTest() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	// logdb --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE_LOGDB_NAME)
		&& unittest_config.m_bExecuteDB)
	{
		if (GUnitTestRunner::GetInst().RunLogDBUnitTest() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}


	return ret;
}

int RunOnlyDBUnitTest(LPWSTR cmdline)
{
	int ret = GUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	GConfig::m_bWindowVisible = false;
	GConfig::m_nPort = 7409;	// UnitTest일 경우 포트 충돌이 나면 안되므로

	MInitStatisticsProfile(true);

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	if (unittest_config.m_nRepeat > 1)
	{
		mlog("UnitTest Repeat Count = %d\n", unittest_config.m_nRepeat);
	}

	GUnitTestRunner	unitTestRunner;
	unitTestRunner.GetConfig().m_strStageFilter = "all";
	if (unitTestRunner.RunGameDBUnitTest() == false) 
	{
		ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
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

void InitCrashDump()
{
	bool bFullMemoryDump = false;

	SAppHelper::InitCrashDump(GConfig::m_strServerName.c_str(), bFullMemoryDump, GCrashReporter::CrashDumpHandler);
}

void InitGoogleMock()
{
	int argc = 0;
	char** argv = NULL;
	::testing::GTEST_FLAG(throw_on_failure) = false;
	::testing::InitGoogleMock(&argc, argv);

	// GoogleMock 실패 메시지, UnitTest++ 결과로 출력
	SGoogleMockListener::InitListener();
}

int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	tbb::task_scheduler_init init;

	InitGoogleMock();

	GMinetInitScope MinetInitScope;
	GScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	SAppHelper::InitLog(FILENAME_LOG_FOR_UNITTEST);
	GConfig::Init_INI();
	InitCrashDump();

	GConst::RUN_FOR_TEST = true;
	GConst::INCLUDE_DEV_FIELD_LIST = true;


#ifdef _DBTEST
	{
		return RunOnlyDBUnitTest(cmdline);
	}
#endif

	return RunOnlyUnitTest(cmdline);
}

///////////////////////////////////////////////////////////////////

