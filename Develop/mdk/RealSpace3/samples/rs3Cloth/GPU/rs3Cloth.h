//----------------------------------------------------------------------------------------------------
// 파일명 : rs3Cloth.h
// 작성자 : MAIET Entertainment
// 날  짜 : 2007. 09. 27 ~
// 설  명 : Cloth Animation 예제 Application 클래스
//
// Copyright (c) 2002 All rights reserved by MAIET Entertainment
//----------------------------------------------------------------------------------------------------
#pragma once

#include "Resource.h"

#ifdef _DEBUG
#pragma comment ( lib, "../../../../../../sdk/UnitTest++/lib/UnitTest++.vsnet2005d.lib")
#else
#pragma comment ( lib, "../../../../../../sdk/UnitTest++/lib/UnitTest++.vsnet2005.lib")
#endif

#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "rs3Cloth.h"
#include "RD3DApplication.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
//#include "RMeshSceneNode.h"
//#include "RAABBTreeSceneNode.h"
#include "RDeviceD3D.h"
#include "RLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "MDebug.h"

#include "RMeshMgr.h"
#include "RActor.h"
#include "../../sampleApp.h"

#include "RClothNode.h"
//#include "Scene.h"

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

/**
@brief Cloth Animation 장면 관리자
*/
class mySceneNode : public RSceneNode 
{
public:
	//////////////////////////////////////////////////////////////////////////
	// Device
	RDeviceD3D*		m_lpDevice;					///< Rs3 3D Device Pointer
	//CJHCamera*		m_pCamera;				///< 임시 카메라 클래스

	//////////////////////////////////////////////////////////////////////////
	// Collision Object
	bool			m_bEnvironmentHasChanged;	///< 상태 변화가 있는가

	//////////////////////////////////////////////////////////////////////////
	// State
	bool			m_bIsPaused;				///< Pause
	bool			m_bWireframe;				///< Render Wireframe
	unsigned short  m_MouseID;					///< Picking ID
	float			m_fMouseIDX;				///< 
	float			m_fMouseIDY;				///< 

	//////////////////////////////////////////////////////////////////////////
	// 07.10.29
	RActor			m_Actor;					///< elu 모델
	RAnimation*		m_pAnimation;				///< Animation
	int				m_nStepSimulation;			///< 디버깅용 프레임 스텝

private:

public:
	mySceneNode();
	~mySceneNode();


	//////////////////////////////////////////////////////////////////////////
	// Create
	void Init();
	bool OnCreate();
	bool LoadFile();
	
	//////////////////////////////////////////////////////////////////////////
	// Update
	void OnUpdate();

	//////////////////////////////////////////////////////////////////////////
	// Render	
	void OnRender();
	void Render();

	//////////////////////////////////////////////////////////////////////////
	// Destroy
	bool OnDestroy();
	void DeleteTexture();
	void DestroyShader();

	//////////////////////////////////////////////////////////////////////////
	// Get / Set
	void	Step(int nCount);
};

/**
@brief Cloth Animation 예제 Application 클래스
*/
class myApplication : public sampleApp 
{
public:
	mySceneNode* m_pSceneNode;

	void UpdateCameraDir()
	{
		m_cameraDir = RVector(cosf(m_fCameraZ)*sinf(m_fCameraX),
			sinf(m_fCameraZ)*sinf(m_fCameraX),
			cosf(m_fCameraX));
	}

public:
	myApplication() : sampleApp() {}

	char *GetName() { return "Cloth Sample Season 2"; }
	bool Create();
	void OnDestroy();
	void OnRender();
	void OnUpdate();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

} g_App;