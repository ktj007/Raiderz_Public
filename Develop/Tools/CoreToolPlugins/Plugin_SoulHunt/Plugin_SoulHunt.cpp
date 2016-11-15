// Plugin_SoulHunt.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"

#include "Plugin_SoulHunt.h"
#include "WS_WorkSpaceManager.h"

#include "WS_Zone.h"

#include "WS_Field.h"
#include "VR_EntityObjectList.h"
#include "VR_EntityEnvironment.h"
#include "VR_EntityMaterial.h"
#include "VR_EtcPathFind.h"
#include "VR_EnvFieldInfo.h"

#include "WS_RzDev.h"

#include "VL_MapNew.h"

#include "M_PathFinder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlugin_SoulHuntApp

BEGIN_MESSAGE_MAP(CPlugin_SoulHuntApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_SoulHuntApp 생성

CPlugin_SoulHuntApp::CPlugin_SoulHuntApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CPlugin_SoulHuntApp 개체입니다.

CPlugin_SoulHuntApp theApp;


// CPlugin_SoulHuntApp 초기화

BOOL CPlugin_SoulHuntApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

class SoulHuntAppPlugin : public CPluginInterface
{
public:
	bool Initialize(CWorkSpaceManager* pWorkSpaceManager) override
	{
		// Rollup and workspace
		pWorkSpaceManager->RegisterView<CRollupEntityObjectList>(theApp.m_hInstance);
		pWorkSpaceManager->RegisterView<CRollupEntityEnvironment>(theApp.m_hInstance);
		pWorkSpaceManager->RegisterView<CRollupEntityMaterial>(theApp.m_hInstance);
		pWorkSpaceManager->RegisterView<CRollupEtcPathFind>(theApp.m_hInstance);
		pWorkSpaceManager->RegisterView<CRollupEnvFieldInfo>(theApp.m_hInstance);

		pWorkSpaceManager->RegisterWorkspace<CWorkSpaceField>();
		pWorkSpaceManager->RegisterWorkspace<CWorkSpaceZone>();
		pWorkSpaceManager->RegisterWorkspace<CWorkSpaceRzDev>();

		// Dialog
		pWorkSpaceManager->RegisterView<CDlgMapNew>(theApp.m_hInstance);

		// init path find tester
		g_PathfinderTest.Init();

		return true;
	}

	void Finalize(CWorkSpaceManager* pWorkSpaceManager) override
	{
		// Rollup and workspace
		pWorkSpaceManager->UnregisterView<CRollupEntityObjectList>();
		pWorkSpaceManager->UnregisterView<CRollupEntityEnvironment>();
		pWorkSpaceManager->UnregisterView<CRollupEntityMaterial>();
		pWorkSpaceManager->UnregisterView<CRollupEtcPathFind>();
		pWorkSpaceManager->UnregisterView<CRollupEnvFieldInfo>();

		pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceField>();
		pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceZone>();
		pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceRzDev>();

		// destroy path find tester
		g_PathfinderTest.Destroy();

		// Dialog
		pWorkSpaceManager->UnregisterView<CDlgMapNew>();
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