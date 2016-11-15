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
#include "RActorNodeConstraintController.h"
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

const char* ACTOR_MESH = "hf/hf.elu";
const char* FACE_MESH = "hf/hf_face_01.elu";
const char* HAIR_MESH = "hf/hf_hair_01.elu";
const char* WEAPON_MESH = "weapons/1hd_03.elu";
const char* SHIELD_MESH = "weapons/shield_03.elu";

const char* ANIMATION_NAMES[] = { "1hs_atk1", "1hs_atk1_r", "1hs_atk1f", "1hs_atk1f_r" };
const int ANIMATION_COUNT = _countof(ANIMATION_NAMES);

class myApplication : public sampleApp
{
	RCollisionSphere *m_pCollision;

	RActor			*m_pActors;

	RActor			*m_pWeapon;
	RActorNodeConstraintController	*m_pWeaponController;
	RActor			*m_pWeapon2;
	RActorNodeConstraintController *m_pWeapon2Controller;

	bool			m_bShowBones;
	bool			m_bUsingPassRenderer;

	RDirectionalLightSceneNode *m_pDirLight;

	bool			m_bBlend;

public:
	myApplication() : sampleApp() , m_pCollision(NULL),m_bShowBones(false),m_bBlend(true),m_pWeapon(NULL), m_bUsingPassRenderer(true) { }

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

		case '1'	:
			{
				if( m_pWeapon )
				{
					RActorNode* actorNode = m_pActors[0].GetActorNode( "dummy_1hs_r" );
					if( actorNode )
					{
						actorNode->AddChild( m_pWeapon );
					}
				}
			}
			break;

		case '2'	:	if( m_pWeapon ) { m_pWeapon->RemoveFromParent(); } break;

		case '3'	:
			{
				if( m_pWeapon2 )
				{
					RActorNode* actorNode = m_pActors[0].GetActorNode( "dummy_1hs_r" );
					if( actorNode )
					{
						actorNode->AddChild( m_pWeapon2 );
					}
				}
			}
			break;

		case '4'	:	if( m_pWeapon2 ) { m_pWeapon2->RemoveFromParent(); } break;

		case '5'	:	if( m_pWeapon ) { m_pWeapon->RemoveFromParent(); SAFE_DELETE( m_pWeapon );  } break;
		case '6'	:	if( m_pWeapon2 ) { m_pWeapon2->RemoveFromParent(); SAFE_DELETE( m_pWeapon2 );  } break;
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

	

//	m_pWeapon = new RActor;
//	m_pWeapon->Create( "../Data/Model/weapons/longsword_02.elu" );
//	m_pWeapon->Update();
//	m_pWeapon->SetUsingPassRenderer(m_bUsingPassRenderer);
////	m_pActors[0].AttachChildSceneNodeToActorNode( "dummy_1hs_r", m_pWeapon );
//	RActorNode* actorNode = m_pActors[0].GetActorNode( "dummy_1hs_r" );
//	if( actorNode )
//	{
//		actorNode->AddChild( m_pWeapon );
//		m_pWeaponController = new RActorNodeConstraintController( actorNode );
//		m_pWeapon->RSceneNode::AddController( m_pWeaponController );
//	}
//
//	m_pWeapon2 = new RActor;
//	m_pWeapon2->Create( "../Data/Model/weapons/Shield_01.elu" );
//	m_pWeapon2->Update();
//	m_pWeapon2->SetUsingPassRenderer(m_bUsingPassRenderer);
////	m_pActors[0].AttachChildSceneNodeToActorNode( "dummy_1hs_r", m_pWeapon2 );
//	actorNode = m_pActors[0].GetActorNode( "dummy_1hs_r" );
//	if( actorNode )
//	{
//		actorNode->AddChild( m_pWeapon2 );
//		m_pWeapon2Controller = new RActorNodeConstraintController( actorNode );
//		m_pWeapon2->RSceneNode::AddController( m_pWeapon2Controller );
//	}

	RMesh* pMesh;

	pMesh = REngine::GetSceneManager().CreateResource< RMesh >( FACE_MESH );
	if( pMesh->Create() )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( "face" );
		if( pMeshNode )
		{
			m_pActors[0].AddMeshNode( pMeshNode, NULL );
		}
	}
	else
	{
		REngine::GetSceneManager().ReleaseResource( pMesh );
	}

	pMesh = REngine::GetSceneManager().CreateResource< RMesh >( HAIR_MESH );
	if( pMesh->Create() )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( "hair" );
		if( pMeshNode )
		{
			m_pActors[0].AddMeshNode( pMeshNode, NULL );
		}
	}
	else
	{
		REngine::GetSceneManager().ReleaseResource( pMesh );
	}

	pMesh = REngine::GetSceneManager().CreateResource< RMesh >( WEAPON_MESH  );
	if( pMesh->Create() )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( "1hd_03" );
		if( pMeshNode )
		{
			m_pActors[0].AddMeshNode( pMeshNode, m_pActors[0].GetActorNode( "dummy_1hs_r" ) );
		}
	}
	else
	{
		REngine::GetSceneManager().ReleaseResource( pMesh );
	}

	pMesh = REngine::GetSceneManager().CreateResource< RMesh >( SHIELD_MESH );
	if( pMesh->Create() )
	{
		RMeshNode* pMeshNode = pMesh->GetNode( "shield_03" );
		if( pMeshNode )
		{
			m_pActors[0].AddMeshNode( pMeshNode, m_pActors[0].GetActorNode( "dummy_shield" ) );
		}
	}
	else
	{
		REngine::GetSceneManager().ReleaseResource( pMesh );
	}

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

	//if( m_pWeapon )
	//	m_pWeapon->Update();

	//if( m_pWeapon2 )
	//	m_pWeapon2->Update();

	for(int i=0;i<ACTOR_COUNT;i++)
	{
		RActorNode* pActorNode = m_pActors[i].GetActorNode("dummy_1hs_r");
		if(pActorNode)
			pActorNode->ForceUpdateAnimation();

		if(m_pActors[i].IsOncePlayDone())
			m_pActors[i].SetAnimation( ANIMATION_NAMES[ rand() % ANIMATION_COUNT ] );
	}

	//if( m_pWeapon )
	//	m_pWeapon->Update();

	//if( m_pWeapon2 )
	//	m_pWeapon2->Update();

// 	for( int i = 0; i < 50000000; ++i )
// 	{
// 		int t = 0;
// 	}

	char szBuffer[MAX_PATH] = "";
	sprintf(szBuffer,"FPS %3.1f", GetDevice()->GetFrameRate() );
	SetText(szBuffer);
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

	if(m_pWeapon2)
		m_pWeapon2->RemoveFromParent();

	if(m_pActors)
	{
		for(int i=0;i<ACTOR_COUNT;i++)
		{
			m_pActors[i].RemoveFromParent();
		}
	}

	if(m_pDirLight)
		m_pDirLight->RemoveFromParent();
	
	SAFE_DELETE_ARRAY(m_pActors);
	SAFE_DELETE(m_pWeaponController);
	SAFE_DELETE(m_pWeapon);
	SAFE_DELETE(m_pWeapon2);
	SAFE_DELETE(m_pWeapon2Controller);

	SAFE_DELETE(m_pDirLight);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	g_App.Run();

	PFC_FINALANALYSIS("rs3actor3.txt");
}