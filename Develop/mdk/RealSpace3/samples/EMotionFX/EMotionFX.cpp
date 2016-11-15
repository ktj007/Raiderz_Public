#define NOMINMAX

// include Ageia PhysX SDK and the PhysX / EMotionFX controller
#include "../../../../../sdk/PhysX/v2.8.1/SDKs/Physics/include/NxPhysics.h"
#include "EmotionFX/PhysX/PhysXController.h"
#include "EmotionFX/PhysX/PhysXUtilities.h"

// link to the Ageia PhysX libraries
#pragma comment( lib, "../../../../../sdk/PhysX/v2.8.1/SDKs/lib/win32/NxCharacter.lib" )
#pragma comment( lib, "../../../../../sdk/PhysX/v2.8.1/SDKs/lib/win32/NxCooking.lib" )
#pragma comment( lib, "../../../../../sdk/PhysX/v2.8.1/SDKs/lib/win32/PhysXLoader.lib" )


#include "EMotionFXManager.h"

#ifdef _DEBUG
#pragma comment (lib, "EmotionFX/lib/EMotionFX_x86DebugMT_VS2005.lib")
#pragma comment (lib, "EmotionFX/lib/MCore_x86WindowsDebugMT_VS2005.lib")
#else
#pragma comment (lib, "EmotionFX/lib/EMotionFX_x86ReleaseMT_VS2005.lib")
#pragma comment (lib, "EmotionFX/lib/MCore_x86WindowsReleaseMT_VS2005.lib")
#endif



#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
#include "RCollision.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"
#include "RSkeleton.h"
#include "RRenderHelper.h"
#include "RSimpleSphereSceneNode.h"


#include "d3dx9.h"

//#include "vld.h"
//#pragma comment (lib, "vldmt.lib")


// physics
NxPhysicsSDK*					gPhysicsSDK					= NULL;	// the physics main object
NxScene*						gScene						= NULL;	// the physics scene where our ragdoll is in
bool							gHardwareSimulationSupported;		// indicates if hardware physics simulation is supported by the current machine

bool							gUsePhysics					= false;// flag whether physics controller is active or not
NXU::NxuPhysicsCollection*		gCollection					= NULL;	// the physics collection, this is the preloaded ragdoll information which will be used to construct the PhysX ragdoll
PhysXController*				gPhysicsController			= NULL;	// a pointer to our PhysX controller for the actor





class myApplication : public sampleApp {
	RActor			*m_pActors;
	MActorInstance	m_EMotionActor;
public:
	myApplication() : sampleApp() { }

	char *GetName() { return "EMotionFX"; }
	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	void OnRender();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
} g_App;


bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	const RVector vecRight	= RVector(10,0,0);
	const RVector vecForward = RVector(0,10,0);
	const RVector vecUp		= RVector(0,0,10);

	switch (message)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_NUMPAD1 : m_EMotionActor.PlayMotion("testani.xsm"); break;
		case VK_NUMPAD2 : m_EMotionActor.PlayMotion("testRun.xsm"); break; 
		case VK_NUMPAD3 : 
			{
				PlayBackInfo playbackInfo;
				playbackInfo.mStartNodeIndex = 11;
				playbackInfo.mMix		= true;
				playbackInfo.mNumLoops	= EMFX_LOOPFOREVER;
				playbackInfo.mTargetWeight = 1;
				m_EMotionActor.PlayMotion("testani.xsm", &playbackInfo); 
			}
			break; 
		case VK_NUMPAD5 : 
			m_EMotionActor.StopMotion();
			m_EMotionActor.StopIK();
			m_EMotionActor.DeactivateLookAt();
			break;
		case VK_NUMPAD6	:
			m_EMotionActor.StartIK();
			m_EMotionActor.SetIKGoal(RVector(rand()%100, rand()%100, rand()%100 - 50 )); 
			break;
		case VK_NUMPAD7	: 
			m_EMotionActor.AtivateLookAt();
			break;
		case VK_NUMPAD8	: g_App.GetDevice()->SetFillMode(RFILL_SOLID);break;
		}break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}



bool myApplication::OnCreate()
{
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	// 테스트용 모델 생성
	m_pActors = new RActor;
	pSceneManager->AddSceneNode(m_pActors);
	m_pActors->Create("Mesh/goblin_commander/goblin_commander.elu");
	m_pActors->Update();	// aabb 업데이트를 위해
	m_pActors->SetPosition(0,0,0);

	m_fDistToCamera = 200;
	m_cameraTargetPos = RVector( 100, 0, 50 );
	m_fCameraZ = 1.6f;
	m_fCameraX = 2.0f;

	GET_EMOTION_MGR.Init();
	GET_EMOTION_MGR.LoadMesh("Mesh/EmotionFX/testOrg.xac");

	m_EMotionActor.Create("testOrg.xac", m_pActors);
	GET_EMOTION_MGR.BindActor(m_pActors, &m_EMotionActor, "testOrg.xac");
	m_EMotionActor.LoadMotion("Mesh/EmotionFX/testani.xsm");
	m_EMotionActor.LoadMotion("Mesh/EmotionFX/testRun.xsm");
	m_EMotionActor.CreateSolver("Bip01 L UpperArm", "Bip01 L Hand");
	m_EMotionActor.StopIK();

	m_EMotionActor.SetupLookAtControllers();

	return true;
}


void myApplication::OnRender()
{
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,20,50,50,0x80808080);
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,100,10,10,0xffffffff);
}


void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();
	
	EMFX_ACTORMANAGER.Process( GetDevice()->GetLastElapsedTime() * 0.001f );

	// update the goal in all lookat controllers
	m_EMotionActor.UpdateLookAt( GetCameraPos() );
	m_EMotionActor.UpdateAnimation();
}



void myApplication::OnDestroy()
{
	GET_EMOTION_MGR.Shutdwon();

	m_pActors->RemoveFromParent();
	SAFE_DELETE_ARRAY(m_pActors);
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	return g_App.Run();
}