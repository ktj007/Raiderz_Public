// Plugin_CutScene.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"

#include "Plugin_CutScene.h"
#include "WS_WorkSpaceManager.h"
#include "WS_CutScene.h"
#include "C_ControlMediator.h"

#include "VR_CutSceneObjectList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlugin_CutSceneApp

BEGIN_MESSAGE_MAP(CPlugin_CutSceneApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_CutSceneApp 생성

CPlugin_CutSceneApp::CPlugin_CutSceneApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CPlugin_CutSceneApp 개체입니다.

CPlugin_CutSceneApp theApp;


// CPlugin_CutSceneApp 초기화

BOOL CPlugin_CutSceneApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

class CutSceneAppPlugin : public CPluginInterface
{
public:
	bool Initialize(CWorkSpaceManager* pWorkSpaceManager) override
	{
		pWorkSpaceManager->RegisterView<CRollupCutSceneObjectList> (theApp.m_hInstance);
		pWorkSpaceManager->RegisterWorkspace<CWorkSpaceCutScene> ();
		return true;
	}

	void Finalize(CWorkSpaceManager* pWorkSpaceManager) override
	{
		pWorkSpaceManager->UnregisterView<CRollupCutSceneObjectList> ();
		pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceCutScene> ();
	}

	HINSTANCE GetInstanceHandle()
	{
		return theApp.m_hInstance;
	}
} g_pluginInstance;

extern "C" __declspec(dllexport) CPluginInterface* CreatePlugin()
{
	return &g_pluginInstance;
}