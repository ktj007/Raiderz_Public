//----------------------------------------------------------------------------------------------------
// 파일명 : PhysXRagDoll.h
// 작성자 : MAIET Entertainment
// 날  짜 : 2008. 10. 23
// 설  명 : MPhysX RagDoll
//
// Copyright (c) 2002 All rights reserved by MAIET Entertainment
//----------------------------------------------------------------------------------------------------
#pragma once

#ifdef _DEBUG
#pragma comment (lib,"../lib/MPhysXd.lib")
#pragma comment (lib,"../../../../sdk/tinyxml/lib/tinyxmld_STL.lib")

#else
#pragma comment (lib,"../lib/MPhysX.lib")
#pragma comment (lib,"../../../../sdk/tinyxml/lib/tinyxml_STL.lib")

#endif

#pragma comment (lib,"../../../../sdk/zlib/lib/zlib.lib")

#ifdef _DEBUG
#pragma comment ( lib, "../../../../sdk/UnitTest++/lib/UnitTest++.vsnet2005d.lib")
#else
#pragma comment ( lib, "../../../../sdk/UnitTest++/lib/UnitTest++.vsnet2005.lib")
#endif

#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "MPhysX.h"
#include "RD3DApplication.h"
#include "RFont.h"
#include "RCameraSceneNode.h"
#include "RCollisionTree.h"
#include "RTextSceneNode.h"
//#include "RAABBTreeSceneNode.h"
#include "RDeviceD3D.h"
#include "RLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "MDebug.h"

#include "RMeshMgr.h"
#include "RActor.h"
#include "samples/sampleApp.h"

// UnitTest++
#include "UnitTest++.h"
#include "TestDetails.h"
#include "TestReporter.h"
#include "MCrashDump.h"

#include<winuser.h>
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

//#include "vld.h"

using namespace rs3;

namespace UnitTest 
{
	class TestReporterGame : public TestReporter
	{
	private:
		virtual void ReportTestStart(TestDetails const& test);
		virtual void ReportFailure(TestDetails const& test, char const* failure);
		virtual void ReportTestFinish(TestDetails const& test, float secondsElapsed);
		virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
	};

	void TestReporterGame::ReportFailure(TestDetails const& details, char const* failure)
	{
		char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";

		printf(errorFormat, details.filename, details.lineNumber, details.testName, failure);

		mlog("\n =============== UnitTest Failure ===============\n");
		mlog(errorFormat, details.filename, details.lineNumber, details.testName, failure);
	}

	void TestReporterGame::ReportTestStart(TestDetails const& test)
	{
		mlog("\n ===== UnitTest Start : %s ====\n", test.testName);
	}

	void TestReporterGame::ReportTestFinish(TestDetails const& /*test*/, float)
	{
		mlog("\n =============== UnitTest Finish ===============\n");
	}

	void TestReporterGame::ReportSummary(int const totalTestCount, int const failedTestCount,
		int const failureCount, float secondsElapsed)
	{
		if (failureCount > 0)
		{
			printf("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
			mlog("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
		}
		else
		{
			printf("Success: %d tests passed.\n", totalTestCount);
			mlog("Success: %d tests passed.\n", totalTestCount);
		}
		printf("Test time: %.2f seconds.\n", secondsElapsed);
		mlog("Test time: %.2f seconds.\n", secondsElapsed);
	}

}

bool RunUnitTest()
{
	UnitTest::TestReporterGame test_report;
	int failure_count = UnitTest::RunAllTests(test_report, UnitTest::Test::GetTestList(), 0);

	if (failure_count > 0)
	{
		return false;
	}

	return true;
}

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

namespace rs3
{
	class REnvironmentSet;
}

class RagDollApp : public sampleApp 
{
public:
	RagDollApp() : sampleApp(), m_pEnvironmentSet(NULL) {}

	virtual char *GetName() { return "RagDollSample"; }
	bool Create();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	virtual void Update(){ sampleApp::Update(); }
	void CheckMovieCapture();
	RFont* GetFont(){ return m_pFont; }

	virtual RSceneManager* CreateSceneManager();

private:

	rs3::REnvironmentSet*	m_pEnvironmentSet;
	rs3::RCollisionTree*	m_pCollisionTree;

} g_App;