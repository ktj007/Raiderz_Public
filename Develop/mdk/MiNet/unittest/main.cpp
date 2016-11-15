#include "stdafx.h"
#include "MDebug.h"
#include "MBaseUnitTest.h"
#include "MiNetUnitTest.h"
#include "MiNetUnitTestRunner.h"

int RunOnlyUnitTest(LPSTR cmdline)
{
	int ret = MiNetUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	int nUnitTestCount = 1;


	// stage1 --
	if (unittest_config.CheckStageFilter(UNITTEST_STAGE1_NAME))
	{
		if (unittest_config.m_nRepeat > 1)
		{
			mlog("UnitTest Repeat Count = %d\n", unittest_config.m_nRepeat);
		}

		MiNetUnitTestRunner	unitTestRunner;
		if (unitTestRunner.RunStage1() == false) 
		{
			ret = MiNetUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	return ret;
}


void SetupCurrentDirectory()
{
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
}

using namespace UnitTest;


int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	SetupCurrentDirectory();
	MInitLog();

	minet::Init(100);	// 커맨드 테이블 버전은, 현재 테스트 범위가 아니기 때문에 임의의 값(100)으로 설정

	int result = RunOnlyUnitTest("");

	minet::Fini();

	return result;
}

