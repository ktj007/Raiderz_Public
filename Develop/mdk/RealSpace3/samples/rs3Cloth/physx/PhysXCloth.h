//----------------------------------------------------------------------------------------------------
// 파일명 : PhysXSimple
// 작성자 : MAIET Entertainment
// 날  짜 : 2008. 04. 18 ~
// 설  명 : PhysX 간단한 예제 Application 클래스
//
// Copyright (c) 2002 All rights reserved by MAIET Entertainment
//----------------------------------------------------------------------------------------------------
#pragma once

#ifdef _DEBUG
#pragma comment (lib,"../../../../cml2/lib/cml2d.lib")
#pragma comment (lib,"../../../../../../sdk/lua50/lib/lua50d.lib")
#pragma comment (lib,"../../../../../../sdk/tinyxml/lib/tinyxmld_STL.lib")

#else
#pragma comment (lib,"../../../../cml2/lib/cml2.lib")
#pragma comment (lib,"../../../../../../sdk/lua50/lib/lua50.lib")
#pragma comment (lib,"../../../../../../sdk/tinyxml/lib/tinyxml_STL.lib")

#endif

#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "MPhysX.h"
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
#include "RPhysXActorNode.h"
#include "RPhysXActorCollision.h"
#include "../../sampleApp.h"

// UnitTest++
//#include "UnitTest++.h"
//#include "TestDetails.h"
//#include "TestReporter.h"
//#include "MCrashDump.h"

#include<winuser.h>
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

using namespace rs3;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

//#include "vld.h"
//#pragma comment( lib,"vldmt.lib")

/**
@brief Cloth Animation 장면 관리자
*/
class HitReportTest : public NxUserControllerHitReport
{
	NxScene* mScene;


public:
	HitReportTest(NxScene* pScene) { mScene = pScene; bColl = false; }
	~HitReportTest() {}

	bool bColl;
	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
	{
		if(hit.shape/* && hit.shape->is(NX_SHAPE_CAPSULE)*/)
		{
			if(hit.shape->isCapsule())
			{
				//mScene->releaseActor(hit.shape->getActor());
				mlog("Actor 충돌\n");
				return NX_ACTION_PUSH;
			}
		}

		return NX_ACTION_NONE;
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}
};

namespace rs3
{
	class REnvironmentSet;
}

NxPhysicsSDK*			g_pSDK = NULL;
NxRemoteDebugger*		g_pRemoteDebugger = NULL;
NxScene*				g_pScene = NULL;			// Scene
NxCookingInterface*		g_pCooking = NULL;

class myApplication : public sampleApp 
{
public:
	myApplication() : sampleApp(), m_pEnvironmentSet(NULL) {}

	char *GetName() { return "PhysX Simple Sample"; }
	bool Create();
	void CreateActor(char* szFilename);
	void CreateScene();
	void OnUpdate();

	void GameUpdate(float _fElapsed);
	void GameRender();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);



	RActor* m_pActor;

	virtual RSceneManager* CreateSceneManager()
	{
		class CUSTOM_SCENEMANAGER : public RSceneManager
		{
		public:
			virtual RSceneNode* CreateSceneNode(const char* pChar)
			{
				if (_stricmp(pChar, "PHYSX_CLOTH") == 0)
				{
					return new RPhysXActorNode( g_pSDK, g_pScene, g_pCooking );
				}
				else
				{
					return RSceneManager::CreateSceneNode(pChar);
				}
			}

			virtual RSceneNode* CreateCollision(const char* pChar)
			{
				if (_stricmp(pChar, "PHYSX_ACTORCOLLISION") == 0)
				{
					return new RPhysXActorCollision( g_pSDK );
				}
				else
				{
					return RSceneManager::CreateCollision(pChar);
				}
			}
		};

		return new CUSTOM_SCENEMANAGER;
	}


private:

	rs3::REnvironmentSet* m_pEnvironmentSet;

} g_App;