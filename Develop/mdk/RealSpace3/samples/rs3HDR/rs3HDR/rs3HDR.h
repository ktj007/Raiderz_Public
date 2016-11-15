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
#include "RGlobalHDRSceneNode.h"
#include "MDebug.h"

#include "RMeshMgr.h"
#include "RActor.h"
#include "RActorNode.h"
#include "../../sampleApp.h"

#include<winuser.h>
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

using namespace rs3;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

//#include "vld.h"
//#pragma comment( lib,"vldmt.lib")



class REnvironmentSet;
class MyApplication : public sampleApp 
{
public:
	MyApplication() : sampleApp(), m_pEnvironmentSet(NULL) {}
	RVector m_cameraPos;

	char *GetName() { return "HDR Sample"; }
	bool Create(); 
	bool OnCreate();
	void CreateScene();
	void OnUpdate();
	void OnRender();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

private:

	rs3::REnvironmentSet* m_pEnvironmentSet;

} g_App;