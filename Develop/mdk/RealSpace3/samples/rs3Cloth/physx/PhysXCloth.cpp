//----------------------------------------------------------------------------------------------------
// 파일명 : PhysXSimple2
// 작성자 : MAIET Entertainment
// 날  짜 : 2008. 06. 24 ~
// 설  명 : 응용 프로그램에 대한 진입점을 정의합니다.
//
// Copyright (c) 2002 All rights reserved by MAIET Entertainment
//----------------------------------------------------------------------------------------------------

#include "PhysXCloth.h"
#include "RRenderHelper.h"
#include "RDebugPassRenderer.h"
#include "REnvironmentSet.h"

#include "RPhysXActorNode.h"
#include "NxCapsuleController.h"
#include "NxCooking.h"

using namespace rs3;
using namespace physx;

class CPhysXCharacter
{
public:
	void Init(){}
	void Halt(){}

	void Update(){}
private:
};

DWORD g_dwBoxID;

//////////////////////////////////////////////////////////////////////////
// Global
//////////////////////////////////////////////////////////////////////////
char	g_szFilename[MAX_PATH] = {0, };
POINT	g_Pt;


NxClothDesc*			pClothDesc = NULL;
NxActor*				g_pBoxActor = NULL;

std::vector<DWORD>		g_vecCollisionShapeID; // SHAPE ID

RAnimation*				g_pAnimation;				///< Animation

float g_fGravity		= 0.0f;
float g_fWind			= 0.0f;
float fSecPerFrame		= 1.0f / 60.0f; 
float g_fMoveX			= 100.0f;

bool g_bRenderCollision = false;


//////////////////////////////////////////////////////////////////////////
bool myApplication::Create()
{
	// 리소스 패쓰 설정. 주의 : 장치 생성 이전에 해준다.
	REngine::GetConfig().SetPath(RP_MESH, "../Data/Model/MapObject;../Data/Model/Sky;../Data/Model/Player/hf;../Data/Model/weapons;../Data/SFX;../Data/Model/Monster/goblin_mage");
	REngine::GetConfig().SetPath(RP_TEXTURE, "../Data/Texture/Character;../Data/Texture/Map;../Data/Texture/SFX;../Data/Texture/SpeedTree;../Data/Texture/Tile;../Data/Texture/Weapon;../Data/Texture/Sky;../Data/Texture;");
	REngine::GetConfig().SetPath(RP_EFFECT, "../Data/SFX");
	REngine::GetConfig().SetDataPath("../Data/");
	REngine::GetConfig().m_nWidth = 800;
	REngine::GetConfig().m_nHeight = 600;

	// 장치, 씬매니저
	sampleApp::Create();
	DragAcceptFiles(GetWindowHandle(), TRUE);

	//////////////////////////////////////////////////////////////////////////
	//
	// init physx
	//
	//////////////////////////////////////////////////////////////////////////

	// SDK Init
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	g_pSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);		//버젼 등록
	if(!g_pSDK && errorCode != 0) 
	{
		_ASSERT(0);
		return false;
	}

	// Cooking Init
	g_pCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	if (g_pCooking == NULL)
	{
		mlog("Failed to Init PhysX Cooking\n");
		return false;
	}

	g_pCooking->NxInitCooking();
	g_pRemoteDebugger = g_pSDK->getFoundationSDK().getRemoteDebugger();
	g_pRemoteDebugger->connect("localhost");

	// Set the physics parameters
	// 기본적으로 object들이 서로 관통할 수 있도록 허용하는“grace-depth”이다. 
	g_pSDK->setParameter(NX_SKIN_WIDTH, 0.005f);

	// Set the debug visualization parameters
	g_pSDK->setParameter(NX_VISUALIZATION_SCALE, 1);			// 미터 단위의 크기 비율
	g_pSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);		// 충돌 모양선의 크기
	g_pSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);			// 좌표축선의 크기
	g_pSDK->setParameter(NX_VISUALIZE_CLOTH_MESH, 1);

	// Scene Description
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(0, 0, -980.f);	// rs에 맞춘 기본값

	// Description으로 부터 씬 생성
	g_pScene = g_pSDK->createScene(sceneDesc);
	if(!g_pScene)
	{ 
		//하드웨어가 지원못하면 소프트웨어 모드로 생성한다
		sceneDesc.simType = NX_SIMULATION_SW; 
		g_pScene = g_pSDK->createScene(sceneDesc);  

		if(!g_pScene)
		{
			_ASSERT(0); //씬 생성 실패
			return false;
		}

		// 기본 Timing 설정
		g_pScene->setTiming();
	}

	NxReal myTimestep = 1.0f/60.0f;     
	g_pScene->setTiming(myTimestep, 8, NX_TIMESTEP_FIXED);

	// Scene Default Material Init
	NxMaterial* defaultMaterial = g_pScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5f);		// 탄성 회복도
	defaultMaterial->setStaticFriction(0.5f);	// 정지마찰력 : 물체가 처음 움직일때 필요한 마찰력
	defaultMaterial->setDynamicFriction(0.5f);	// 운동마찰력 : 물체가 움직일때 적용되는 마찰력

	return true;
}

void myApplication::CreateActor(char* pFilename)
{
	if( strlen(pFilename) <= 0 )
		return;

	m_pActor = new RActor();

	char szFilename[MAX_PATH] = {0, };
	strcpy_s(szFilename, MAX_PATH, pFilename);

	m_pActor->RemoveFromParent();
	m_pActor->Destroy();
	//////////////////////////////////////////////////////////////////////////
	if( g_App.GetSceneManager()->LoadSceneNode( m_pActor, szFilename ) )
	{
		g_App.GetSceneManager()->AddSceneNode(m_pActor);

		strcat_s(szFilename, ".ani");
		if( g_pAnimation = m_pActor->m_pMesh->m_AniMgr.Add("TestAni", szFilename))// m_pActor->m_pMesh->m_AniMgr.m_nIDCount) )
		{
			g_pAnimation->SetAnimationLoopType(RAniLoopType_Loop);
			m_pActor->SetAnimation(g_pAnimation);
		}
	}
	m_pActor->Pause();

	// Add Collision Mesh
	//CreateScene();
	for( int i = 0; i < m_pActor->GetActorNodeCount(); ++i )
	{
		RActorNode* pActorNode = m_pActor->GetActorNode(i);

		if(pActorNode->GetActorNodeType() == ACTOR_NODE_CLOTH)
		{
			RPhysXActorNode* pPhysXActorNode = (RPhysXActorNode*)pActorNode;
			pPhysXActorNode->m_fWind = g_fWind;
		}
	}
}

void myApplication::CreateScene()
{
	std::string strName;
	MMatrix matTrans(MMatrix::IDENTITY);
	std::vector< DWORD > strEnableList;
	NxD6JointDesc d6Desc;


	//////////////////////////////////////////////////////////////////////////
	// Box
	//////////////////////////////////////////////////////////////////////////
	if(!g_pBoxActor)
	{
		//pPhysxActor = MPhysX::GetInstance()->CreateObject(MPhysXObject::PO_ACTIVE);
		//MBox targetBox(RVector3(-100, -100, -10), RVector3(100, 100, 10));
		//matTrans = MMatrix::IDENTITY;
		//matTrans.SetTranslation(RVector3(0, 0, 100));

		//pPhysxActor->AddShapeBox(g_dwBoxID, targetBox, 0.5f);
		//pPhysxActor->SetWorldTransform(matTrans);

		//strEnableList.push_back(g_dwBoxID);
		//pPhysxActor->ActivateShapes(strEnableList);
		//g_pBoxActor = pPhysxActor->GetShape(g_dwBoxID);
		//g_pBoxActor->setLinearDamping(5);
		//g_pBoxActor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);

		// Box Joint

		//// joint로 두 actor들을 연결한다.
		//d6Desc.actor[0] = g_pBoxActor;
		//d6Desc.actor[1] = 0;

		////NxVec3 globalAnchor(0,7,0);					// joint에 두 actor들이 회전하기 위한 global space에서 정의되어진 point 로global anchor를 설정한다.
		////d6Desc.localAnchor[0] = globalAnchor;			// joint에 두 actor들이 회전하기위한 또한 global space에 정의되어진 축 global axis를 설정한다. 
		////d6Desc.jointFlags |= NX_JF_COLLISION_ENABLED;	// default로 두 actor들 사이에서의 충돌은 억제되어있다.
		////box2->getGlobalPose().multiplyByInverseRT(globalAnchor, d6Desc.localAnchor[1]);

		////d6Desc.localAxis[0]   = NxVec3(1,0,0);
		////d6Desc.localNormal[0] = NxVec3(0,1,0);
		////d6Desc.localAxis[1]   = NxVec3(1,0,0);
		////d6Desc.localNormal[1] = NxVec3(0,1,0);

		//// If locked, do not allow rotation.
		//d6Desc.twistMotion  = NX_D6JOINT_MOTION_LOCKED;	// x-axis
		//d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;	// y-axis
		//d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;	// z-axis
		//// If locked, do not allow translation motion.
		//d6Desc.xMotion		= NX_D6JOINT_MOTION_FREE;	// x-axis	
		//d6Desc.yMotion		= NX_D6JOINT_MOTION_FREE;	// y-axis
		//d6Desc.zMotion		= NX_D6JOINT_MOTION_FREE;	// z-axis

		//pPhysxActor->GetScene()->createJoint(d6Desc);
	}
}

void myApplication::OnDestroy()
{
	//m_pPx->RemovePxObject(pPhysxActor); // DEBUG 용 박스
	//m_pPx->RemovePxObject(pPhysxActiveActor);

	if(m_pActor)
	{
		m_pActor->RemoveFromParent();
		m_pActor->Destroy();
		SAFE_DELETE(m_pActor);
	}

	// 씬 지워주기
	g_pSDK->releaseScene(*g_pScene);

	//Cooking 해제
	if (g_pCooking)
	{
		g_pCooking->NxCloseCooking();
		g_pCooking = NULL;
	}

	// SDK 해제
	if (g_pSDK) 
	{
		g_pSDK->release();
		g_pSDK = NULL;
	}
}

static bool g_bRbuttonDown = false;
static bool g_bLbuttonDown = false;
static bool g_bMbuttonDown = false;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_CHAR:
		if(wParam==VK_ESCAPE)
			PostMessage(hWnd,WM_CLOSE,0,0);
		break;

	case WM_DROPFILES :
		{
			char szFilename[256];
	
			UINT i = 0;
			// 2. 모두 몇 개의 파일이 드롭되었는지 알아낸다.
			UINT uCount = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL ,0);

			// 3. 첫번째 파일만 읽어 들인다.
			for(i = 0;i < 1/*uCount*/;i++)
			{
				// 4. 드롭된 파일의 이름을 알아온다.
				DragQueryFile((HDROP)wParam, i, szFilename ,255);
				// 5. 드롭된 파일을 읽어 들인다..
				// 
			/*
				MPhysX::GetInstance()->RemoveObject(pPhysxActor);

				if(g_vecCollisionObject.size())
				{
					for(int i = 0; i < (int)m_pActor->m_pCollision->m_vCollisions.size(); ++i )
						MPhysX::GetInstance()->RemoveObject(g_vecCollisionObject[i]);
				}

				g_vecCollisionObject.clear();
			*/
				CreateActor(szFilename);
			}
		
			// drag and drop 작업을 끝낸다.
			DragFinish((HDROP)wParam);
		}
				break;

	case WM_MOUSEWHEEL:
		{
			// 값의 + 가 휠이 위로, -가 휠이 아래로 내려가는걸 나타냅니다.
			if((INT)wParam < 0) 
			{
				m_fDistToCamera += 10;
			}	
			else 
			{
				m_fDistToCamera -= 10;
			}	
		}
		break;

	case WM_MBUTTONDOWN:
		{
			g_bMbuttonDown = true;

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetWindowHandle(),&pt);
			g_Pt = pt;
		}
		break;

	case WM_MBUTTONUP:
		g_bMbuttonDown = false;
		break;

	case WM_RBUTTONDOWN:
		g_bRbuttonDown = true;
		break;

	case WM_RBUTTONUP:
		g_bRbuttonDown = false;
		break;

	case WM_LBUTTONDOWN:
		{
			g_bLbuttonDown = true;

			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetWindowHandle(),&pt);
			g_Pt = pt;
		}
		break;

	case WM_LBUTTONUP:
		g_bLbuttonDown = false;
		break;
	};

	return false;
}

void myApplication::OnUpdate()
{
	RApplication::OnUpdate();

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	DWORD elapsed = GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 15 : 1);

	float fRotateStep=elapsed*0.001f;
	float fMoveStep=elapsed*0.5f;

	if(IsActive() && g_bLbuttonDown)
	{
		if(IsKeyDown(VK_LEFT)) m_fCameraZ+=fRotateStep;
		if(IsKeyDown(VK_RIGHT)) m_fCameraZ-=fRotateStep;
		if(IsKeyDown(VK_UP)) m_fCameraX-=fRotateStep;
		if(IsKeyDown(VK_DOWN)) m_fCameraX+=fRotateStep;

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindowHandle(),&pt);
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x - g_Pt.x;//GetDevice()->GetScreenWidth()/2;
		iDeltaY = pt.y - g_Pt.y;//GetDevice()->GetScreenHeight()/2;

		m_fCameraZ	+= -iDeltaX*0.01f;
		m_fCameraX	+= iDeltaY*0.01f;
		m_fCameraX	= max(0.01f,min(MMath::PI-0.01f,m_fCameraX));

		g_Pt = pt;
		//	ResetCursor();
	}

	UpdateCameraDir();

	if(g_bMbuttonDown)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindowHandle(),&pt);
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x - g_Pt.x;//GetDevice()->GetScreenWidth()/2;
		iDeltaY = pt.y - g_Pt.y;//GetDevice()->GetScreenHeight()/2;

		RCameraSceneNode* pCamera = RCameraSceneNode::GetCurrentCamera();
		if(pCamera)
		{
			RVector3 up  = RCameraSceneNode::GetCurrentCamera()->GetUp();
			RVector dir  = m_cameraDir;
			RVector right = RVector(0,0,1).CrossProduct(dir);
			right.Normalize();

			m_cameraTargetPos += up * (float)iDeltaY + right * (float)iDeltaX;
			OnCameraTargetMove(m_cameraTargetPos);
		}

		g_Pt = pt;
	}

	RVector posDiff(0,0,0);
	if(IsActive())
	{
		RVector dir = m_cameraDir;
		RVector right = RVector(0,0,1).CrossProduct(dir);
		right.Normalize();

		if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
		if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
		if(IsKeyDown('A')) posDiff+=fMoveStep*right;
		if(IsKeyDown('D')) posDiff-=fMoveStep*right;

	} // if(IsActive())

	if(posDiff.Length()>0) 
		OnCameraTargetMove(m_cameraTargetPos + posDiff);

	RVector cameraPos = m_cameraTargetPos - m_cameraDir*m_fDistToCamera;
	m_pCamera->Set(cameraPos, m_cameraDir);

	static bool bCCDEnabled = true;

	static float fAniSpeed = 1.0f;
	if(IsActive())
	{
		if(IsKeyDown('1')) 
		{
			fSecPerFrame += 0.001f;
		}
		if(IsKeyDown('2')) 
		{
			fSecPerFrame -= 0.001f;
		}

		if(IsKeyDown('C')) //Right
		{
			bCCDEnabled = !bCCDEnabled;

		//	MPhysX::GetInstance()->GetNxSDK()->setParameter(NX_CONTINUOUS_CD, bCCDEnabled);

			Sleep(100);
		}

		if(IsKeyDown('R'))
		{
			g_bRenderCollision = !g_bRenderCollision;
		}

		//////////////////////////////////////////////////////////////////////////
		// Properites
		if(IsKeyDown('U'))//VK_NUMPAD7))	// Gravity Down
		{
			g_fGravity -= 10.f;
			if(g_fGravity < 0)
				g_fGravity = 0;

			NxVec3 gra;
			gra.set(0, 0, -g_fGravity);
			g_pScene->setGravity(gra);
		}
		if(IsKeyDown('I'))//VK_NUMPAD8))	// Gravity Up
		{
			g_fGravity += 10.f;

			NxVec3 gra;
			gra.set(0, 0, -g_fGravity);
			g_pScene->setGravity(gra);
		}

		// Wind
		if(IsKeyDown('J'))//VK_NUMPAD4))
		{
			g_fWind-= 10.f;

			for( int i = 0; i < m_pActor->GetActorNodeCount(); ++i )
			{
				RActorNode* pActorNode = m_pActor->GetActorNode(i);

				if(pActorNode->GetActorNodeType() == ACTOR_NODE_CLOTH)
				{
					RPhysXActorNode* pPhysXActorNode = (RPhysXActorNode*)pActorNode;
					pPhysXActorNode->m_fWind = g_fWind;
				}
			}

		}
		if(IsKeyDown('K'))//VK_NUMPAD5))
		{
			g_fWind += 10.f;

			for( int i = 0; i < m_pActor->GetActorNodeCount(); ++i )
			{
				RActorNode* pActorNode = m_pActor->GetActorNode(i);

				if(pActorNode->GetActorNodeType() == ACTOR_NODE_CLOTH)
				{
					RPhysXActorNode* pPhysXActorNode = (RPhysXActorNode*)pActorNode;
					pPhysXActorNode->m_fWind = g_fWind;
				}
			}
		}

		if( IsKeyDown(VK_NUMPAD4) )
		{
			g_fMoveX -= 0.1f;
		}
		if( IsKeyDown(VK_NUMPAD5) )
		{
			g_fMoveX += 0.1f;
		}

		//////////////////////////////////////////////////////////////////////////
		// Animation
		//////////////////////////////////////////////////////////////////////////
		static int g_nCurrentFrame = 0;

		if(IsKeyDown(VK_OEM_COMMA)) 
		{
			g_nCurrentFrame -= 5;
			if( g_nCurrentFrame < 0 )
				g_nCurrentFrame = 0;

			m_pActor->SetFrame(g_nCurrentFrame);
		}

		if(IsKeyDown(VK_OEM_PERIOD)) 
		{
			g_nCurrentFrame += 5;
			int nMaxFrame = g_pAnimation->GetMaxFrame();
			if( g_nCurrentFrame > nMaxFrame )
				g_nCurrentFrame = nMaxFrame;

			m_pActor->SetFrame(g_nCurrentFrame);
		}


		if(IsKeyDown(VK_OEM_4))	//  '[{' for US
		{
			fAniSpeed -= 0.5f;
			if(fAniSpeed < 0.1f)
				fAniSpeed = 0.1f;

			m_pActor->SetSpeed(fAniSpeed);

			Sleep(100);
		}

		if(IsKeyDown(VK_OEM_6))	//  ']}' for US
		{	
			fAniSpeed += 0.5f;
			m_pActor->SetSpeed(fAniSpeed);

			Sleep(100);
		}

		static int bPlayAni = 0;
		if(IsKeyDown(VK_OEM_2))	// '/'키
		{
			bPlayAni = !bPlayAni;
			if(bPlayAni)
				m_pActor->Pause();
			else
				m_pActor->Play();

			m_pActor->SetSpeed(fAniSpeed);

			Sleep(100);
		}

	} // if(IsActive())

	char szBuffer[256];
	sprintf_s(szBuffer, "가 FPS : %3.1f\n AniSpeed : %f\n Filename : 가나다라마바 %s\n Gravity : %f\n Wind : %f\n ",//Wire(W) : %d\n Normal(N) : %d\n AniSpeed : %f\n FrameSkip : %d\n ",
		GetDevice()->GetFrameRate(), fAniSpeed, g_szFilename, g_fGravity, g_fWind);

	SetText(szBuffer);
}


void myApplication::GameUpdate(float _fElapsed)
{
	OnUpdate();

	if (g_pScene)
	{
		g_pScene->simulate(_fElapsed);
		g_pScene->flushStream();
		g_pScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	}
}

void myApplication::GameRender()
{
	// 바닥 그리기
	g_App.GetDevice()->SetFillMode(RFILL_WIRE);

	RMatrix matWorld;
	matWorld.MakeIdentity();
	RRenderHelper::RenderGrid(MMatrix::IDENTITY, 100.0f, 10, 10);

	g_App.GetDevice()->SetFillMode(RFILL_SOLID);

	if(g_bRenderCollision)
		m_pActor->RenderMeshCollision();
}

static float fTimer = 0;
static float fTotalElapsed = 0;
static int	 nLoop = 0;
static int	 nSkip = 0;
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

	//char szFilename[256] = "C:\\Documents and Settings\\3dpg\\My Documents\\3dsmax\\export\\Rengot_sculpture_k01.elu";
	//char szFilename[256] = "C:\\Documents and Settings\\3dpg\\My Documents\\3dsmax\\export\\hf_renas_knight_heavy_armor_test01_rig01.elu";
	char szFilename[256] = "C:\\Documents and Settings\\3dpg\\My Documents\\3dsmax\\export\\SampleCloth02.elu";
	strcpy_s(g_szFilename, MAX_PATH, lpCmdLine);
#ifdef _DEBUG
	//g_App.CreateActor(szFilename);
#else
	g_App.CreateActor(lpCmdLine);
#endif

	// main loop
	BOOL bGotMsg;
	MSG  msg;
	RTimer tmp_timer; 
	tmp_timer.Reset();
	do
	{
		bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		if( bGotMsg ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else 
		{
			char buf[MAX_PATH] = {0, };
			//////////////////////////////////////////////////////////////////////////
			// 프레임 제한 60 FPS
			tmp_timer.Update();
			float fElapsed = tmp_timer.GetElapsedTime() * 0.001f;
		
			fTotalElapsed	+= fElapsed;
			fTimer			+= fElapsed;

			if( fTimer < 0 ) 
			{
				continue;
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// 메인루프
			//
			// RD3DApplication의 경우
			// (업데이트) Game의 OnUpdate -> SceneManager의 UpdateAndCull ->
			// (렌더링) SceneManager의 RenderPrimarayCamera호출 -> Game의 OnPreRender/OnRender
			RSceneManager* pSceneManager = g_App.GetSceneManager();
			RDevice* pDevice = g_App.GetDevice();

			// Game Update
			pSceneManager->UpdateTimer();
			// SceneManager Update
			pSceneManager->UpdateAndCull();

			g_App.GameUpdate(fTotalElapsed);

			//sprintf_s(buf, MAX_PATH, "(fTimer)%f ", fTimer);
			//mlog(buf);
			//sprintf_s(buf, MAX_PATH, "(TotElapsed)%f ", fTotalElapsed);
			//mlog(buf);
			//sprintf_s(buf, MAX_PATH, "(SecPerFrame)%f ", fSecPerFrame);
			//mlog(buf);
			//sprintf_s(buf, MAX_PATH, "(Loop)\n");
			//mlog(buf);

			nSkip = 0;

			// 한프레임에 해당하는 시간을 뺀다. 
			fTimer			= -fSecPerFrame; 
			fTotalElapsed	= 0;
			

			pDevice->Clear(true,true,false,D3DCOLOR_ARGB(255, 0, 0, 255));
			pDevice->BeginScene();

			// SceneManager Render
			pSceneManager->RenderPrimaryCamera();

			// Game Render
			g_App.GameRender();

			pDevice->EndScene();
			pDevice->Flip();
		}

	}while( WM_QUIT != msg.message  );

	//PFC_FINALANALYSIS("profile_game_c.txt");
	// destroy
	g_App.Destroy();
	return (INT)msg.wParam;
}
