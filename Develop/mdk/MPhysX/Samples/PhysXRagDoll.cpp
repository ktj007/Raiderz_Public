//----------------------------------------------------------------------------------------------------
// 파일명 : PhysXRagDoll.cpp
// 작성자 : MAIET Entertainment
// 날  짜 : 2008. 10. 23
// 설  명 : 응용 프로그램에 대한 진입점을 정의합니다.
//
// Copyright (c) 2002 All rights reserved by MAIET Entertainment
//----------------------------------------------------------------------------------------------------

#include "REngine.h"

#include "PhysXRagDoll.h"
#include "RRenderHelper.h"
#include "RDebugPassRenderer.h"
#include "REnvironmentSet.h"
#include "RTerrain.h"
#include "RFont.h"

#include "MPhysX.h"
#include "MPxObjStatic.h"
#include "MPxObjRagDoll.h"

#include "../MPhysX4R3/MPhysX4R3.h"

#include "../MPhysX4R3/RPhysXActorController.h"
#include "../MPhysX4R3/RPhysXRagDollBuilder.h"

using namespace rs3;
using namespace physx;

MPhysX*					phx = NULL;
MPxObjStatic*			pPxTerrain = NULL;
MPxObjRagDoll*			pPxRagDoll = NULL;

const std::string		ZONE_FILE("../Data/Zone/BirdTown/BirdTown");
RActor*					g_pActor = NULL;
bool					g_bShowBone = false;
//bool					g_bShowBone = true;

RPhysXActorController*	g_pPhysXActorController = NULL;
RPhysXRagDollBuilder	g_ragdollBuilder;

bool RagDollApp::Create()
{
	// 리소스 패쓰 설정. 주의 : 장치 생성 이전에 해준다.
	REngine::GetConfig().SetPath(RP_SHADER, "../rs3/Shader/;../rs3/Shader/components/");
	REngine::GetConfig().SetPath(RP_TREE,	"../Data/Model/SpeedTree");
	REngine::GetConfig().SetPath(RP_MESH, "../Data/Model/MapObject;../Data/Model/Sky;../Data/Model/Player/hf;../Data/Model/weapons;../Data/SFX;../Data/Model/Monster/goblin_mage");
	REngine::GetConfig().SetPath(RP_TEXTURE, "../Data/Texture/Character;../Data/Texture/Map;../Data/Texture/SFX;../Data/Texture/SpeedTree;../Data/Texture/Tile;../Data/Texture/Weapon;../Data/Texture/Sky;../Data/Texture;");
	REngine::GetConfig().SetPath(RP_EFFECT, "../Data/SFX");
	REngine::GetConfig().SetDataPath("../Data/");

	// init physx
	phx = new MPhysX;
	phx->Init(10.f, 0, "localhost");
	phx->SetSceneGravity(-5000.f);
	//phx->SetLOD(MPhysX::PL_NONE);

	// 장치, 씬매니저, 백그라운드 워커 만들기
	sampleApp::Create();

	// 존 로딩
	bool bResult = false;
	bool bBackgroundLoad = false;
	bResult = GetSceneManager()->LoadRootSceneFromXML( (ZONE_FILE + ".zone.xml").c_str() , bBackgroundLoad);

	// 충돌 로딩
	m_pCollisionTree = new RCollisionTree;
	m_pCollisionTree->Load( (ZONE_FILE + ".cl2").c_str() );

	// 환경 로딩
	m_pEnvironmentSet = new REnvironmentSet;
	bResult = m_pEnvironmentSet->CreateFromFile( (ZONE_FILE + ".zone.env.xml").c_str(), false );
	GetSceneManager()->AddSceneNode(m_pEnvironmentSet);
	bResult = m_pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY);

	// 카메라 설정
	RVector3 vCamPos(23090.0f, 32095.0f, 875.0f);
	SetCameraPos(vCamPos);

	// MPhysX, Static Object Creation
	pPxTerrain =  phx->CreatePxObject<MPxObjStatic>();
	RPxObjStaticSubActorBuilder rs3MeshBuilder;
	rs3MeshBuilder.SetTerrain( GetSceneManager()->GetCurrentTerrain(), 64 );

	if (m_pCollisionTree->GetRootNode())
	{
		if (m_pCollisionTree->GetRootNode()->GetLeftNode() || m_pCollisionTree->GetRootNode()->GetRightNode())
		{
			rs3MeshBuilder.SetCollisionTreeRootNode( m_pCollisionTree->GetRootNode(), rs3MeshBuilder.GetCubeSizeFromTerrainInfo());
		}
	}
	pPxTerrain->AddStaticSubActorsFromBuilder(rs3MeshBuilder);
	//pPxTerrain->ActivateBodyStaticShapesAll();

	// MPhysX, RagDoll
	pPxRagDoll = phx->CreatePxObject<MPxObjRagDoll>();
	pPxRagDoll->CreateSampleRagDoll( vCamPos + RVector3(0, 0, 500) );

	// RActor
	RMesh* pMesh = GetSceneManager()->CreateResource< RMesh >("hf_rendel_royal_knight.elu");
	if( !pMesh->BuildAll() )
	{
		GetSceneManager()->ReleaseResource( pMesh );
		return false;
	}
	g_pActor = new RActor;
	g_pActor->Create(pMesh);
	g_pActor->SetAnimation("none_walk");
	g_pActor->Play();
	g_pActor->SetPosition(vCamPos);
	GetSceneManager()->AddSceneNode(g_pActor, true);

	// 디버깅 렌더링될 본과 색깔 지정
	g_pActor->m_dwBoneColor[RBIPID_RCLAVICLE] = 0xffffff00;
	g_pActor->m_dwBoneColor[RBIPID_RUPPERARM] = 0xffff0000;
	class DebugRenderingBoneCullingVisitor : public RSceneNodeVisitor
	{
	public:
		DebugRenderingBoneCullingVisitor(){}
		~DebugRenderingBoneCullingVisitor(){}

		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode )
		{
			const std::string& rNodeName = pSceneNode->GetNodeName();
			if (rNodeName.find("Finger") != std::string::npos
				|| rNodeName.find("Bip") == std::string::npos
				|| rNodeName.find("Twist") != std::string::npos
				)
			{
				pSceneNode->AddAttribute(RSNA_NO_DEBUG_RENDERING);
			}

			return true;
		}
		virtual void OnPostOrderVisit( RSceneNode* pSceneNode ){}

	} debugRenderingVisitor;
	g_pActor->Traverse(&debugRenderingVisitor);


	// MPhysX, Camera Around Capsule
	//MCapsule targetCap(RVector3(0,0,50), RVector3(0,0,150), 50);
	//MMatrix matTrans(MMatrix::IDENTITY);
	//matTrans.SetTranslation(m_cameraTargetPos);

	// Unit Test
	//if (RunUnitTest() == false) 
	//	return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 적절한 씬 매니저 생성
//
RSceneManager* RagDollApp::CreateSceneManager()
{
	class CCustomSceneManager : public RSceneManager
	{
	public:
		virtual RSceneNode* CreateSceneNode( const char* pChar )
		{
			RSceneNode* pSceneNode = NULL;
			pSceneNode = physx::CreateRPhysXSceneNode(pChar);
			if (NULL == pSceneNode)
			{
				pSceneNode = RSceneManager::CreateSceneNode(pChar);
			}

			return pSceneNode;
		}

		virtual RActorController* CreateActorController(const char* _pSzControllerName)
		{
			RActorController* pController = NULL;
			pController = physx::CreateRPhysXController(_pSzControllerName);
			if (NULL == pController)
			{
				pController = RSceneManager::CreateActorController(_pSzControllerName);
			}

			return pController;
		}
	};

	return new CCustomSceneManager;
}

void RagDollApp::OnDestroy()
{
	phx->RemovePxObject(pPxTerrain);
	phx->RemovePxObject(pPxRagDoll);

	if (g_pPhysXActorController)
	{
		g_pActor->RemoveController(g_pPhysXActorController);
		g_pPhysXActorController->ReleaseNxObject();
		SAFE_DELETE( g_pPhysXActorController );
	}

	if (m_pEnvironmentSet)
	{
		m_pEnvironmentSet->RemoveFromParent();
		SAFE_DELETE(m_pEnvironmentSet);
	}

	SAFE_DELETE(m_pCollisionTree);

	g_pActor->RemoveFromParent();
	delete g_pActor;

	phx->Destroy();
	delete phx;
}

bool RagDollApp::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_CHAR:
		if(wParam==VK_ESCAPE)
			PostMessage(hWnd,WM_CLOSE,0,0);
		break;

	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case 'B'	:
				{
					g_bShowBone = !g_bShowBone;
				}
				break;

			case 'T'	:
				{
					if (g_pPhysXActorController == NULL)
					{
						g_pPhysXActorController = new RPhysXActorController(phx->GetNxSDK()->getScene(0));
						g_pActor->AddController( g_pPhysXActorController );

						g_pPhysXActorController->BuildNxObject(&g_ragdollBuilder);
						g_pPhysXActorController->SetEnableKinematicMode( RSkeleton::GetBipName(RBIPID_SPINE1));
						g_pPhysXActorController->SetResourceNxActorDensity(0.f);
						g_pPhysXActorController->SetEnableMoveFuncInKinematic(false);

						RMatrix mat = g_pActor->GetLocalTransform();
						g_pPhysXActorController->SetKinematicModeTransform(mat, true);
					}
					else
					{
						g_pActor->RemoveController(g_pPhysXActorController);
						delete g_pPhysXActorController;
						g_pPhysXActorController = NULL;
						g_pActor->Stop();
					}
				}
				break;

			case 'C'	:
				{
					if (IsCapturing())
					{
						StopCapture();
					}
					else
					{
						StartCapture();
					}
				}
				break;

			case 'P'	:
				{
					if (g_pPhysXActorController)
					{
						g_pActor->UpdateTransform();
						RMatrix mat = g_pActor->GetLocalTransform();
						mat._41 += 10;
						mat._42 += 10;
						mat._43 += 10;
						g_pPhysXActorController->SetKinematicModeTransform(mat, true );
					}

				}
				break;
			}
		}
		break;

	case WM_KEYUP:
		{
		}
		break;

	case WM_MOUSEWHEEL:
		{
			// 값의 + 가 휠이 위로, -가 휠이 아래로 내려가는걸 나타냅니다.
			if((INT)wParam < 0) 
			{
				m_fDistToCamera += 50;
			}	
			else 
			{
				m_fDistToCamera -= 50;
			}	
		}
		break;
	};

	return false;
}

void RagDollApp::CheckMovieCapture()
{
	if (IsCapturing())
	{
		//뭔가 레코딩 한다는 글자를 화면에 띄울수 있으면 좋겠으나 귀찮음. -_-

		// Capture Running
		RunCapture();
	}

}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	MInitLog();
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	mlog("Application started\n");

	// init / create
	g_App.OnInit();
	if(!g_App.Create()) {
		mlog("Application failed to create\n");
		g_App.Destroy();
		return 0;
	}

	// main loop
	BOOL bGotMsg;
	MSG  msg;
	do
	{
		bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		if( bGotMsg ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else 
		{
			// AutoUpdate Actor 업데이터
			class RAutoUpdateActorUpdator : public RActorController
			{
			public:
				RAutoUpdateActorUpdator(void) : RActorController(RACT_POSTUPDATE){}
				virtual ~RAutoUpdateActorUpdator(void){}
				void UpdateActor(RActor* pActor)
				{
					// actor update for render
					m_pActor = pActor;
					UpdateActorForRender();
				}
			} actorUpdate;
			actorUpdate.UpdateActor(g_pActor);

			// Sample Application Update
			// update timer / BackgroundWorker / SceneManager
			g_App.Update();


			// 게임 프리렌더링
			// 여기서 피직스 업데이트
			phx->Update(REngine::GetTimer().GetElapsedTime() * 0.001f);

			// 렌더링
			RDevice* pDevice = g_App.GetDevice();
			pDevice->Clear(true,true,false,D3DCOLOR_ARGB(255, 0, 0, 255));
			pDevice->BeginScene();

			// 디버깅 렌더링
			REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->SetCurrentRenderingTextFont(g_App.GetFont());
			if( true == g_bShowBone && g_pActor != NULL )
			{
				vector<RActorNode*>& rActorNodes = g_pActor->GetActorNodes();
				size_t nMax = rActorNodes.size();
				for (size_t i = 0; i < nMax; ++i)
				{
					if (rActorNodes[i]->QueryAttribute(RSNA_NO_DEBUG_RENDERING) == false)
					{
						REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugAxis(rActorNodes[i]->GetWorldTransform(), 4.f, 0.01f);
						REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugText(rActorNodes[i]->GetWorldPosition(), rActorNodes[i]->GetNodeName().c_str(), 0.01f);
					}
				}
			}

			// SceneManager Render
			REngine::GetSceneManager().RenderPrimaryCamera();

			// 캡쳐
			g_App.CheckMovieCapture();

			pDevice->EndScene();
			pDevice->Flip();
		}

	}while( WM_QUIT != msg.message  );

	// destroy
	g_App.Destroy();
	return (INT)msg.wParam;
}
