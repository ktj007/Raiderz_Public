#pragma once

#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "RD3DApplication.h"
#include "RFont.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
#include "RDeviceD3D.h"
#include "RLightSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "MDebug.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "RActorNode.h"

#include "../sampleApp.h"

#include <list>


#include<winuser.h>
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

using namespace rs3;
//#include "vld.h"
//#pragma comment( lib,"vldmt.lib")



class REnvironmentSet;
class MyApplication : public sampleApp 
{
public:
	MyApplication() : sampleApp(), m_pEnvironmentSet(NULL),m_bTestingLoader(false) {}
	RVector m_cameraPos;

	int nActorCnt;
	int nEffectCnt;
	int nLightCnt;
	int nShadowMode;
	bool bSort;

	enum NODE_ADD_CTRL_MODE {
		NODE_ADD_ACTOR = 0,
		NODE_ADD_EFFECT,
		NODE_ADD_LIGHT,
		NODE_ADD_CNT
	};
	int nCtrlMode;

	std::vector<RActor*>	m_vecActor;
	std::vector<REffectSceneNode*>	m_vecEffect;
	std::vector<RLightSceneNode*>	m_vecLight;

	char *GetName() { return "Profiling Sample"; }
	bool Create(); 
	bool OnCreate();
	void CreateScene();
	void OnUpdate();
	void OnRender();
	void OnDestroy();
	
	void AddActor();
	void RemoveActor();
	
	void AddEffect();
	void AddEffect( const RVector& pos );
	void RemoveEffect();

	void AddLight();
	void RemoveLight();

	void ActivateTestingLoader();
	void DeActivateTestingLoader();
	void OnUpdateTestingLoader();

	void ToggleHDREnable();
	void ToggleDOFEnable();
	void ToggleSSAOEnable();
	void ToggleShadow();

	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

private:

	rs3::REnvironmentSet* m_pEnvironmentSet;

	bool	m_bTestingLoader;
	std::vector<std::string>	m_EluFileList;
	std::vector<RActor*>		m_CreatedActorList;

} g_App;

