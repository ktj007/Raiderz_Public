// Plugin_PhysX.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"

#include "Plugin_PhysX.h"
#include "WS_WorkSpaceManager.h"
#include "WS_PhysX.h"
#include "C_ControlMediator.h"

#include "VR_PhysX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlugin_PhysXApp

BEGIN_MESSAGE_MAP(CPlugin_PhysXApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_PhysXApp 생성

CPlugin_PhysXApp::CPlugin_PhysXApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CPlugin_PhysXApp 개체입니다.

CPlugin_PhysXApp theApp;


// CPlugin_PhysXApp 초기화

BOOL CPlugin_PhysXApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

class PhysXAppPlugin : public CPluginInterface
{
public:
	bool Initialize(CWorkSpaceManager* pWorkSpaceManager) override
	{
		pWorkSpaceManager->RegisterView<CRollupPhysX> (theApp.m_hInstance);
		pWorkSpaceManager->RegisterWorkspace<CWorkSpacePhysX> ();
		return true;
	}

	void Finalize(CWorkSpaceManager* pWorkSpaceManager) override
	{
		pWorkSpaceManager->UnregisterView<CRollupPhysX> ();
		pWorkSpaceManager->UnregisterWorkspace<CWorkSpacePhysX> ();
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
