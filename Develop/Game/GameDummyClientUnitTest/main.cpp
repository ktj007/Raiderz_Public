// GameDummyClientUnitTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "DummyClientTestRunner.h"

int RunOnlyUnitTest(LPSTR cmdline)
{
	int ret = DummyClientTestRunner::CCNET_UNITTEST_SUCCESS;

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	int nUnitTestCount = 1;


	// stage1 --
	if (unittest_config.CheckStageFilter("Stage1"))
	{
		if (unittest_config.m_nRepeat > 1)
		{
			mlog("UnitTest Repeat Count = %d\n", unittest_config.m_nRepeat);
		}

		DummyClientTestRunner	unitTestRunner;
		if (unitTestRunner.RunStage1() == false) 
		{
			ret = DummyClientTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	return ret;
}

void SetupCurrentDirectory()
{
	TCHAR szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetupCurrentDirectory();
	MInitLog();

	return RunOnlyUnitTest("");
}

