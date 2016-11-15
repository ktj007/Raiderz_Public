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


#define FILENAME_LOG_FOR_UNITTEST		"mlog_unittest.txt"

int RunOnlyUnitTest(LPSTR cmdline)
{
	int ret = GUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	// Stage2를 위해 App 실행
	GConfig::m_bWindowVisible = false;
	GConfig::m_nPort = 7407;	// UnitTest일 경우 포트 충돌이 나면 안되므로

	MInitProfile(true);

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	int nUnitTestCount = 1;

	// 유닛 테스트 회수 설정
	if (strlen(cmdline) > 0)
	{
		char arg_unittest_count[256]="";

		int ret = sscanf(cmdline, "%s", arg_unittest_count);
		if (ret == 1)
		{
			nUnitTestCount = atoi(arg_unittest_count);
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

		GUnitTestRunner	unitTestRunner;
		if (unitTestRunner.RunStage1() == false) 
		{
			ret = GUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	return ret;
}


void InitGoogleMock()
{
	int argc = 0;
	char** argv = NULL;

	::testing::GTEST_FLAG(throw_on_failure) = false;
	::testing::InitGoogleMock(&argc, argv);
}

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	tbb::task_scheduler_init init;

	InitGoogleMock();

	GMinetInitScope MinetInitScope;
	SProfileScope profileScope(FILENAME_PROFILE);

	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	SAppHelper::InitLog(FILENAME_LOG_FOR_UNITTEST);
	GConfig::Init_INI();


	return RunOnlyUnitTest(cmdline);
}

///////////////////////////////////////////////////////////////////

