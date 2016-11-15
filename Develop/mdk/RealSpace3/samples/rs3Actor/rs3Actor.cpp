#include <windows.h>

#include "MDebug.h"
#pragma warning( disable : 4996 )

#include "mmsystem.h"
#include "../sampleApp.h"

#include "RDeviceD3D.h"
#include "MXml.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
#include "RCollision.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "RSkeleton.h"
#include "RRenderHelper.h"
#include "ROverlayAnimationController.h"
#include "RBlendAnimationController.h"
#include "RDirectionalLightSceneNode.h"
#include "RActorPassRenderer.h"
#include "RUtil.h"

#include "MProfiler.h"

using namespace rs3;

#include "d3dx9.h"

const char* ACTOR_MESH = "goblin_commander/goblin_commander.elu";
const char* ANIMATION_NAMES[] = { "idle", "atk_l", "atk_r", "die", "guard_idle", "guard_end", "run", "sleep", "roar" };
const int ANIMATION_COUNT = _countof(ANIMATION_NAMES);

class myApplication : public sampleApp
{
	RCollisionSphere *m_pCollision;
	RActor			*m_pActors;
	RActor			*m_pWeapon;
	bool			m_bShowBones;
	bool			m_bUsingPassRenderer;

	RDirectionalLightSceneNode *m_pDirLight;

	bool			m_bBlend;

public:
	myApplication() : sampleApp() , m_pCollision(NULL),m_bShowBones(false),m_bBlend(true),m_pWeapon(NULL),m_bUsingPassRenderer(true) { }

	char *GetName() { return "rs3 actor sample application"; }
	bool OnInit() {	REngine::GetConfig().SetPath(RP_MESH, "../Data/Model"); return sampleApp::OnInit(); }

	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	void OnRender();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	void OnToggleUsingPassrenderer();
} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'B'	:	m_bShowBones=!m_bShowBones;break;
		case 'P'	:	OnToggleUsingPassrenderer();break;
		case 'T'	:	m_bBlend=!m_bBlend;break;
		}
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

//#define ACTOR_ROW_COUNT	20
#define ACTOR_ROW_COUNT	1
#define ACTOR_GAP		200
const int ACTOR_COUNT = ACTOR_ROW_COUNT * ACTOR_ROW_COUNT;

bool myApplication::OnCreate()
{
	// 액터생성
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	// light
	m_pDirLight = new RDirectionalLightSceneNode;
	GetSceneManager()->AddSceneNode(m_pDirLight);
	m_pDirLight->SetDirection(RVector(1,0,0));

	m_pActors = new RActor[ACTOR_COUNT];
	for(int i=0;i<ACTOR_COUNT;i++)
	{
		// 테스트용 모델
		pSceneManager->AddSceneNode(&m_pActors[i]);
		m_pActors[i].Create(ACTOR_MESH);
		m_pActors[i].SetPosition(RVector((float)(i % ACTOR_ROW_COUNT)*ACTOR_GAP,(float)(i / ACTOR_ROW_COUNT)*ACTOR_GAP,0));

		RSceneNodeUpdateVisitor updateVisitor;
		m_pActors[i].Traverse( &updateVisitor );

		// 패스 랜더러 사용여부
		m_pActors[i].SetUsingPassRenderer(m_bUsingPassRenderer);
		m_pActors[i].SetAnimation( ANIMATION_NAMES[ rand() % ANIMATION_COUNT ] );
	}

	m_pActors[0].SetPosition(100,0,0);

//	float fCenter = (float)(ACTOR_ROW_COUNT-1) * ACTOR_GAP * 0.5f;
//	m_cameraTargetPos = RVector( fCenter, fCenter, 0.f );
//	m_fDistToCamera = (float)ACTOR_ROW_COUNT * (float)ACTOR_GAP;

	//m_fDistToCamera = 0;
	//m_cameraTargetPos = RVector( 800, -500, 270 );
	m_fDistToCamera = 200;
	m_cameraTargetPos = RVector( 100, 0, 50 );
	m_fCameraZ = 1.6f;
	m_fCameraX = 2.0f;

	return true;
}

void myApplication::OnToggleUsingPassrenderer()
{
	m_bUsingPassRenderer = !m_bUsingPassRenderer;
	for(int i=0;i<ACTOR_COUNT;i++)
	{
		// 패스 랜더러 사용여부
		m_pActors[i].SetUsingPassRenderer(m_bUsingPassRenderer);
	}
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();

	char szBuffer[256] = "";
	sprintf(szBuffer,"FPS %3.1f  using passrenderer %s T : Transition Blending(%s) ", GetDevice()->GetFrameRate(), 
		m_bUsingPassRenderer ? "o" : "x",
		m_bBlend ? "o" : "x");
	SetText(szBuffer);

	for(int i=0;i<ACTOR_COUNT;i++)
	{
		if(m_pActors[i].IsOncePlayDone())
			m_pActors[i].SetAnimation( ANIMATION_NAMES[ rand() % ANIMATION_COUNT ] );
	}


}

void myApplication::OnRender()
{
	if(m_bShowBones)
	{
		m_pDevice->SetDepthEnable(false,false);

		m_pActors[0].RenderSkeleton();
		m_pDevice->SetDepthEnable(true);

		RRenderHelper::RenderBox(RMatrix::IDENTITY,m_pActors[0].GetAABB());
	}
}

void myApplication::OnDestroy()
{
	if(m_pWeapon)
		m_pWeapon->RemoveFromParent();

	if(m_pActors)
	{
		for(int i=0;i<ACTOR_COUNT;i++)
		{
			m_pActors[i].RemoveFromParent();
		}
	}

	if(m_pDirLight)
		m_pDirLight->RemoveFromParent();

	SAFE_DELETE(m_pWeapon);
	SAFE_DELETE_ARRAY(m_pActors);

	SAFE_DELETE(m_pDirLight);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	g_App.Run();

	PFC_FINALANALYSIS("rs3actor.txt");
}