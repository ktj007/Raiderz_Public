#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
//#include "RAABBTreeSceneNode.h"
#include "RCollision.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"
#include "RSkeleton.h"
#include "RRenderHelper.h"
#include "RIKSkeletonController.h"
#include "RSimpleSphereSceneNode.h"

#include "IK/CNode.h"

#include "d3dx9.h"


const char* ACTOR_MESH = "../Data/Model/Monster/goblin_commander/goblin_commander.elu";
const char* ANIMATION_NAMES[] = { "idle", "atk_l", "atk_r", "die", "guard_idle", "guard_end", "run", "sleep", "roar" };
const int ANIMATION_COUNT = elementsOf(ANIMATION_NAMES);



class myApplication : public sampleApp {
	RActor			*m_pActors;
	RVector			m_targetCenter;
	RSimpleSphereSceneNode		*m_pIKTarget;

	RIKSkeletonController	*m_pIKController;

public:
	myApplication() : sampleApp() { }

	char *GetName() { return "rs3 actor IK sample application"; }
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
		case VK_NUMPAD3: m_pActors->SetAnimation( ANIMATION_NAMES[ 6 ] ); break;
		case VK_NUMPAD4: m_targetCenter-= vecRight;break;
		case VK_NUMPAD6: m_targetCenter+= vecRight;break;
		case VK_NUMPAD2: m_targetCenter-= vecForward;break;
		case VK_NUMPAD8: m_targetCenter+= vecForward;break;
		case VK_NUMPAD7: g_App.GetDevice()->SetFillMode(RFILL_WIRE);break;
		case VK_NUMPAD9: g_App.GetDevice()->SetFillMode(RFILL_SOLID);break;
		case VK_SUBTRACT: m_targetCenter-= vecUp;break;
		case VK_ADD: m_targetCenter+= vecUp;break;
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
	m_pActors->Create(ACTOR_MESH);
	m_pActors->Update();	// aabb 업데이트를 위해
	m_pActors->SetPosition(0,0,0);

	m_fDistToCamera = 200;
	m_cameraTargetPos = RVector( 100, 0, 50 );
	m_fCameraZ = 1.6f;
	m_fCameraX = 2.0f;


// IK Target
	m_pIKTarget = new RSimpleSphereSceneNode;
	GetSceneManager()->AddSceneNode(m_pIKTarget);
	m_pIKTarget->m_fRadius = 2.f;

	m_targetCenter = RVector(100,0,100);

// IK controller
	m_pIKController = new RIKSkeletonController(m_pActors);
	m_pIKController->Create( m_pActors->GetActorNode("Bip01 L UpperArm"),
							 m_pActors->GetActorNode("Bip01 L Hand"));

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

	float fTime = 2*MMath::PI*float(timeGetTime()%1000)*0.001;
	const float fRadius = 0.f;
	
	m_pIKTarget->SetPosition(m_targetCenter + RVector(0,fRadius *cos(fTime),fRadius *sin(fTime)));
	m_pIKTarget->Update();

	m_pIKController->SetTargetPosition(m_pIKTarget->GetPosition());
	m_pIKController->Solve();
}



void myApplication::OnDestroy()
{
	SAFE_DELETE(m_pIKController);

	m_pActors->RemoveFromParent();
	SAFE_DELETE_ARRAY(m_pActors);
	m_pIKTarget->RemoveFromParent();
	SAFE_DELETE(m_pIKTarget);
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	return g_App.Run();
}