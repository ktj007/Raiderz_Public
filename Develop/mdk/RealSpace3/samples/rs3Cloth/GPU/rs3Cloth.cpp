//----------------------------------------------------------------------------------------------------
// 파일명 : rs3Cloth.cpp
// 작성자 : MAIET Entertainment
// 날  짜 : 2007. 09. 27 ~
// 설  명 : 응용 프로그램에 대한 진입점을 정의합니다.
//
// Copyright (c) 2002 All rights reserved by MAIET Entertainment
//----------------------------------------------------------------------------------------------------

#include "rs3Cloth.h"
#include "RRenderHelper.h"

//////////////////////////////////////////////////////////////////////////
// Global
static float g_elapsedTime = 0.0f;
POINT			g_Pt;
bool g_bNormalRender	= false;

bool g_bOnUpdate = true;
bool g_bSimulate = true;
bool g_bFrameSkip = false;
RSphere* g_pDebugSphere = 0;

//////////////////////////////////////////////////////////////////////////
// mySceneNode
/////////////////////////////////////////////////////////////////////////
mySceneNode::mySceneNode()
{
	Init();
}

mySceneNode::~mySceneNode()
{
	OnDestroy();
}

void mySceneNode::Init()
{
	m_lpDevice	= (RDeviceD3D*)g_App.GetDevice();

	m_bIsPaused					= false;
	m_bWireframe				= false;
	m_bEnvironmentHasChanged	= true;
}

bool mySceneNode::OnDestroy()
{
	DeleteTexture();
	DestroyShader();

	//////////////////////////////////////////////////////////////////////////
	// 07.10.29
	m_Actor.RemoveFromParent();
	m_Actor.Destroy();

	return true;	
}

void mySceneNode::DeleteTexture()
{
}

void mySceneNode::DestroyShader()
{
}

bool mySceneNode::LoadFile()
{
	OPENFILENAME OFN;
	char lpstrFile[MAX_PATH]="";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner	= g_App.GetWindowHandle();
	OFN.lpstrFilter	= "Elu File(*.*)\0*.*\0Elu File\0*.elu;*.elu\0";
	OFN.lpstrFile	= lpstrFile;
	OFN.nMaxFile	= 256;
	OFN.Flags		|= OFN_NOCHANGEDIR;

	if( GetOpenFileName(&OFN)!=0 ) 
	{
		// 1. 파일을 읽어 들여 모델을 구축한다.
		if( REngine::GetSceneManager().LoadSceneNode( &m_Actor, lpstrFile ) )
		{
			REngine::GetSceneManager().AddSceneNode(&m_Actor);

			//int count	 = m_Actor.m_pMesh->m_AniMgr.m_nIDCount;
			strcat_s(lpstrFile, ".ani");
			if( m_pAnimation = m_Actor.m_pMesh->m_AniMgr.Add("TestAni", lpstrFile) )
			{
				m_pAnimation->SetAnimationLoopType(RAniLoopType_Loop);
				m_Actor.SetAnimation(m_pAnimation);
			}
		}
	}

	return true;
}

bool mySceneNode::OnCreate()
{
	// 릴리즈일 경우 크래쉬 덤프 남기기
#ifndef _DEBUG
	MCrashDump::Init(NULL, NULL, false);
#endif

	// 메뉴바 추가
//	SetMenu(g_App.GetWindowHandle(), LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDR_MENU1) ) );

	// 윈도우 가운데로
	MoveWindow(g_App.GetWindowHandle(),
		(GetSystemMetrics(SM_CXFULLSCREEN) - REngine::GetConfig().m_nWidth) / 2,
		(GetSystemMetrics(SM_CYFULLSCREEN) - REngine::GetConfig().m_nHeight)/ 2,
		REngine::GetConfig().m_nWidth, REngine::GetConfig().m_nHeight, TRUE);

	// Drop & Drag 기능 활성화
	DragAcceptFiles(g_App.GetWindowHandle(), TRUE);
//	DragAcceptFiles(hWnd, FALSE); // 끌때


//	LoadFile();

	return true;
}

void mySceneNode::OnUpdate()
{
	
}

void mySceneNode::Step(int nCount)
{
	if (m_bIsPaused) 
	{
		m_nStepSimulation	= nCount;
		m_bIsPaused			= false;
	}
}

void mySceneNode::OnRender()
{

	// Clear targets and z
//	m_lpDevice->Clear(true, true, 0, 0x000000FF, 1.0f, 0L);
	//RCameraSceneNode* pCameraSceneNode = RCameraSceneNode::GetCurrentCamera();
	m_lpDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	m_lpDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// 바닥 그리기
	m_lpDevice->SetFillMode(RFILL_WIRE);
	
	RMatrix World;
	World.MakeIdentity();
	RRenderHelper::RenderGrid(World, 100.0f, 10, 10);

	m_lpDevice->SetFillMode(RFILL_SOLID);
}

//////////////////////////////////////////////////////////////////////////
// myApplication
//////////////////////////////////////////////////////////////////////////
char strCmdLine[256] = {0, };
bool myApplication::Create()
{
	sampleApp::Create();

	m_fDistToCamera = 1000.f;


	//if (RunUnitTest() == false) 
	//	return 0;

	m_pSceneNode = new mySceneNode;
	GetSceneManager()->AddSceneNode(m_pSceneNode);

	m_pSceneNode->OnCreate();

	char szFilename[256] = "../data/model/mapobject/Rengot_sculpture_k01.elu";

	m_pSceneNode->m_Actor.RemoveFromParent();
	m_pSceneNode->m_Actor.Destroy();
	if( GetSceneManager()->LoadSceneNode( &m_pSceneNode->m_Actor, szFilename ) )
	{
		GetSceneManager()->AddSceneNode(&m_pSceneNode->m_Actor);

		//int count	 = m_Actor.m_pMesh->m_AniMgr.m_nIDCount;
		strcat_s(szFilename, 256 - strlen(szFilename), ".ani");
		if( m_pSceneNode->m_pAnimation = m_pSceneNode->m_Actor.m_pMesh->m_AniMgr.Add("TestAni", szFilename) )
		{
			m_pSceneNode->m_pAnimation->SetAnimationLoopType(RAniLoopType_Loop);
			m_pSceneNode->m_Actor.SetAnimation(m_pSceneNode->m_pAnimation);
		}
	}
	m_pSceneNode->m_Actor.Pause();

	//////////////////////////////////////////////////////////////////////////
	// TEST
	//char szFilename[256] = "C:\\WORK\\SoulHunt\\Game\\Runtime\\Samples\\TestModel\\TestObject.elu";

	//m_pSceneNode->m_Actor.RemoveFromParent();
	//m_pSceneNode->m_Actor.Destroy();
	//if( GetSceneManager()->LoadSceneNode( &m_pSceneNode->m_Actor, szFilename ) )
	//{
	//	GetSceneManager()->AddSceneNode(&m_pSceneNode->m_Actor);

	//	//int count	 = m_Actor.m_pMesh->m_AniMgr.m_nIDCount;
	//	strcat_s(szFilename, 256 - strlen(szFilename), ".ani");
	//	if( m_pSceneNode->m_pAnimation = m_pSceneNode->m_Actor.m_pMesh->m_AniMgr.Add("TestAni", szFilename, m_pSceneNode->m_Actor.m_pMesh->m_AniMgr.m_nIDCount) )
	//	{
	//		m_pSceneNode->m_pAnimation->SetAnimationLoopType(RAniLoopType_Loop);
	//		m_pSceneNode->m_Actor.SetAnimation(m_pSceneNode->m_pAnimation);
	//	}
	//}
	//m_pSceneNode->m_Actor.Pause();
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 테스트용 구 포인터를 얻는다.
	//for(unsigned int i=0;i<m_pSceneNode->m_Actor.m_actorNodes.size();i++)
	//{
	//	RActorNode* pActorNode = m_pSceneNode->m_Actor.m_actorNodes[i];
	//	if(pActorNode->m_pMeshNode->GetFlag(RM_FLAG_CLOTH_MESH))
	//	{
	//		RClothNode* pClothNode = (RClothNode*)pActorNode;
	//		g_pDebugSphere = &pClothNode->m_DebugCurrentSphere;
	//		break;
	//	}
	//}

	//////////////////////////////////////////////////////////////////////////
	return true;
}

void myApplication::OnDestroy()
{
	m_pSceneNode->RemoveFromParent();
	SAFE_DELETE(m_pSceneNode);
}

static bool g_bRbuttonDown = false;
static bool g_bLbuttonDown = false;
static bool g_bMbuttonDown = false;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CHAR:
		if(wParam==VK_ESCAPE)
			PostMessage(hWnd,WM_CLOSE,0,0);
		break;

	case WM_DROPFILES :
		{
			char szFilename[256];
			POINT pt;
			// 1. 어느 위치에 드롭되었는지 그 항목을 알아낸다.
			if (DragQueryPoint((HDROP)wParam, &pt)) 
			{
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
					m_pSceneNode->m_Actor.RemoveFromParent();
					m_pSceneNode->m_Actor.Destroy();
					if( GetSceneManager()->LoadSceneNode( &m_pSceneNode->m_Actor, szFilename ) )
					{
						GetSceneManager()->AddSceneNode(&m_pSceneNode->m_Actor);

						//int count	 = m_Actor.m_pMesh->m_AniMgr.m_nIDCount;
						strcat_s(szFilename, ".ani");
						if( m_pSceneNode->m_pAnimation = m_pSceneNode->m_Actor.m_pMesh->m_AniMgr.Add("TestAni", szFilename) )
						{
							m_pSceneNode->m_pAnimation->SetAnimationLoopType(RAniLoopType_Loop);
							m_pSceneNode->m_Actor.SetAnimation(m_pSceneNode->m_pAnimation);
						}
					}
				}
			}
			// drag and drop 작업을 끝낸다.
			DragFinish((HDROP)wParam);
			break;
		}

	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 

		// 메뉴의 선택 영역을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_OPEN:
			{
				m_pSceneNode->LoadFile();
			}
			break;
		case IDM_EXIT:
			//DestroyWindow(hDlg);	// 다이얼로그 종료
			PostMessage(hWnd,WM_CLOSE,0,0);
			break;
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
	sampleApp::OnUpdate();

#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	if( IsKeyDown(VK_SHIFT) )
	{
		m_pSceneNode->m_Actor.SetPosition( RVector(300,0,100) );
	}
	else m_pSceneNode->m_Actor.SetPosition( RVector(300,0,0) );

	m_pSceneNode->m_Actor.Update();
	/*
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
		//	RCameraSceneNode::GetCurrentCamera()->SetPosition(pos);
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
		//if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);
		//if(IsKeyDown(VK_CONTROL)) posDiff-=fMoveStep*RVector(0,0,1);
	} // if(IsActive())

	if(posDiff.Length()>0) 
	{
		OnCameraTargetMove(m_cameraTargetPos + posDiff);
	}


	RVector cameraPos = m_cameraTargetPos - m_cameraDir*m_fDistToCamera;

	//		GetSceneManager()->GetPrimaryCamera()->Set(cameraPos, m_cameraDir);
	m_pCamera->Set(cameraPos, m_cameraDir);

	static bool bCheckT = false;
	static bool bCheckR = false;
	static bool bCheckV = false;
	if(IsActive())
	{
		if(IsKeyDown('W'))	// WireFrame
		{
		//	Sleep(100);
		}

		if(IsKeyDown('1')) 
		{
			Sleep(100);
		}
		if(IsKeyDown('2')) 
		{
			Sleep(100);
		}
		if(IsKeyDown('3')) 
		{
			Sleep(100);
		}
		if(IsKeyDown('4')) 
		{
			g_bOnUpdate = !g_bOnUpdate;
			Sleep(100);
		}
		if(IsKeyDown('5')) 
		{
			g_bSimulate = !g_bSimulate;
			Sleep(100);
		}

		if(IsKeyDown('B')) 
		{
			Sleep(100);
		}

		if(IsKeyDown('R') )
		{
		}

		if(IsKeyDown('N'))
		{
			g_bNormalRender = !g_bNormalRender;
			Sleep(100);
		}

		if(IsKeyDown('P'))
		{
			m_pSceneNode->m_bIsPaused = !m_pSceneNode->m_bIsPaused;
			Sleep(100);
		}
		if(IsKeyDown('T')) 
		{

			Sleep(100);
		}

		if(IsKeyDown('F')) 
		{
			g_bFrameSkip = !g_bFrameSkip;
			Sleep(100);
		}
		

		if(IsKeyDown('S')) //Up
		{

		}

		if(IsKeyDown('X')) //Down
		{

		}

		if(IsKeyDown('Z')) //left
		{
//			m_pSceneNode->m_Actor.SetSpecularEnable(false);
			Sleep(100);
		}

		if(IsKeyDown('C')) //Right
		{
//			m_pSceneNode->m_Actor.SetSpecularEnable(true);
			Sleep(100);
		}

	
		if(1)//m_pSceneNode->m_Actor)
		{
			if(IsKeyDown(VK_NUMPAD2)) 
			{
				m_pSceneNode->m_Actor.SetPosition(m_pSceneNode->m_Actor.GetWorldTransform()._m[3][0],
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][1],	
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][2]-5.5f);

				g_pDebugSphere->center.z -= m_pSceneNode->m_Actor.GetSpeed();
			}
			if(IsKeyDown(VK_NUMPAD8)) 
			{
				m_pSceneNode->m_Actor.SetPosition(m_pSceneNode->m_Actor.GetWorldTransform()._m[3][0],
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][1],
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][2]+5.5f);

				g_pDebugSphere->center.z += m_pSceneNode->m_Actor.GetSpeed();
			}

			if(IsKeyDown(VK_NUMPAD4)) 
			{
				m_pSceneNode->m_Actor.SetPosition(m_pSceneNode->m_Actor.GetWorldTransform()._m[3][0]-5.5f,
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][1],	
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][2]);

				g_pDebugSphere->center.x -= m_pSceneNode->m_Actor.GetSpeed();
			}
			if(IsKeyDown(VK_NUMPAD6)) 
			{
				m_pSceneNode->m_Actor.SetPosition(m_pSceneNode->m_Actor.GetWorldTransform()._m[3][0]+5.5f,
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][1],
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][2]);

				g_pDebugSphere->center.x += m_pSceneNode->m_Actor.GetSpeed();
			}

			if(IsKeyDown(VK_NUMPAD1)) 
			{
				m_pSceneNode->m_Actor.SetPosition(m_pSceneNode->m_Actor.GetWorldTransform()._m[3][0],
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][1]-5.5f,	
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][2]);

				g_pDebugSphere->center.y -= m_pSceneNode->m_Actor.GetSpeed();
			}
			if(IsKeyDown(VK_NUMPAD7)) 
			{
				m_pSceneNode->m_Actor.SetPosition(m_pSceneNode->m_Actor.GetWorldTransform()._m[3][0],
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][1]+5.5f,	
					m_pSceneNode->m_Actor.GetWorldTransform()._m[3][2]);

				g_pDebugSphere->center.y += m_pSceneNode->m_Actor.GetSpeed();
			}

		}

		if(IsKeyDown(VK_DOWN)) 
		{
		
		}
		if(IsKeyDown(VK_UP)) 
		{
		
		}
		if(IsKeyDown(VK_LEFT)) 
		{
		
		}
		if(IsKeyDown(VK_RIGHT)) 
		{
		
		}
		if(IsKeyDown(VK_DELETE)) 
		{
		}
		if(IsKeyDown(VK_END)) 
		{

		}

		// 새로 고침
		if(IsKeyDown(VK_F5)) 
		{
			////////////////////////////////////////////////////////////////////////////
			//// TEST
			//char szFilename[256] = "C:\\WORK\\SoulHunt\\Game\\Runtime\\Samples\\TestModel\\TestChar_16_8.elu";

			//m_pSceneNode->m_Actor.RemoveFromParent();
			//m_pSceneNode->m_Actor.Destroy();
			//if( GetSceneManager()->LoadSceneNode( &m_pSceneNode->m_Actor, szFilename ) )
			//{
			//	GetSceneManager()->AddSceneNode(&m_pSceneNode->m_Actor);

			//	//int count	 = m_Actor.m_pMesh->m_AniMgr.m_nIDCount;
			//	strcat_s(szFilename, 256 - strlen(szFilename), ".ani");
			//	if( m_pSceneNode->m_pAnimation = m_pSceneNode->m_Actor.m_pMesh->m_AniMgr.Add("TestAni", szFilename, m_pSceneNode->m_Actor.m_pMesh->m_AniMgr.m_nIDCount) )
			//	{
			//		m_pSceneNode->m_pAnimation->SetAnimationLoopType(RAniLoopType_Loop);
			//		m_pSceneNode->m_Actor.SetAnimation(m_pSceneNode->m_pAnimation);
			//	}
			//}
			//m_pSceneNode->m_Actor.Pause();
			//////////////////////////////////////////////////////////////////////////
		
			Sleep(100);
		}


		//VK_OEM_3 = ~
		if(IsKeyDown(VK_OEM_3))
		{
	
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

			m_pSceneNode->m_Actor.SetFrame(g_nCurrentFrame);
		}

		if(IsKeyDown(VK_OEM_PERIOD)) 
		{
			g_nCurrentFrame += 5;
			int nMaxFrame = m_pSceneNode->m_pAnimation->GetMaxFrame();
			if( g_nCurrentFrame > nMaxFrame )
				g_nCurrentFrame = nMaxFrame;

			m_pSceneNode->m_Actor.SetFrame(g_nCurrentFrame);
		}

		static float fAniSpeed = 0.5f;
		if(IsKeyDown(VK_OEM_4))	//  '[{' for US
		{
			fAniSpeed -= 0.1f;
			if(fAniSpeed < 0.1f)
				fAniSpeed = 0.1f;

			m_pSceneNode->m_Actor.SetSpeed(fAniSpeed);

			Sleep(100);
		}

		if(IsKeyDown(VK_OEM_6))	//  ']}' for US
		{	
			fAniSpeed += 0.1f;
			m_pSceneNode->m_Actor.SetSpeed(fAniSpeed);

			Sleep(100);
		}

		static int bPlayAni = 0;
		if(IsKeyDown(VK_OEM_2))	// '/'키
		{
			bPlayAni = !bPlayAni;
			if(bPlayAni)
				m_pSceneNode->m_Actor.Pause();
			else
				m_pSceneNode->m_Actor.Play();

			m_pSceneNode->m_Actor.SetSpeed(fAniSpeed);

			Sleep(100);
		}

	} // if(IsActive())

	char szBuffer[256];
	sprintf_s(szBuffer, " FPS : %3.1f\n Pause(P) : %d\n Wire(W) : %d\n Normal(N) : %d\n AniSpeed : %f\n FrameSkip : %d\n ",
					    GetDevice()->GetFrameRate(),
						m_pSceneNode->m_bIsPaused,
						m_pSceneNode->m_bWireframe,
						g_bNormalRender,
						m_pSceneNode->m_Actor.GetSpeed(),
						g_bFrameSkip);
	SetText(szBuffer);
	*/

	/*timer.Update();
	m_pSceneNode->m_Actor.Update();*/

	UpdateBackgroundWorker();
}

void myApplication::OnRender()
{
	//m_pDevice->HoldDevice(true);
	m_pDevice->Clear(true,true,false,m_dwClearColor);
	m_pDevice->BeginScene();

	m_pSceneManager->RenderPrimaryCamera();

	m_pDevice->EndScene();
	m_pDevice->Flip();
	//m_pDevice->UnHoldDevice(true);
}

static float fSecPerFrame = 1.0f / 10.0f; 
static float fTimer = 0;
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	MInitLog();
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(359);

//	return g_App.Run();
	mlog("Application started\n");
	g_App.OnInit();

	if(!g_App.Create()) {
		mlog("Application failed to create\n");
		g_App.Destroy();
		return 0;
	}

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
			if(g_bFrameSkip)
			{

				
				//////////////////////////////////////////////////////////////////////////
				// 프레임 제한 60 FPS
				tmp_timer.Update();
				fTimer += tmp_timer.GetElapsedTime() * 0.001f;//(float)g_App.GetDevice()->GetLastElapsedTime() * 0.001f; 
				//timer.GetElapsedTime()

				if( fTimer < 0 ) 
				{
					continue;
				}

				// 한프레임에 해당하는 시간을 뺀다. 
				fTimer -= fSecPerFrame; 
				//////////////////////////////////////////////////////////////////////////
				g_App.GetSceneManager()->UpdateTimer();
				g_App.OnUpdate();
				g_App.GetSceneManager()->UpdateAndCull();

				g_App.OnRender();
			}
			else
			{
				g_App.GetSceneManager()->UpdateTimer();
				g_App.OnUpdate();
				g_App.GetSceneManager()->UpdateAndCull();
				
				g_App.OnRender();
			}
			
		}

	}while( WM_QUIT != msg.message  );

	g_App.Destroy();

	return (INT)msg.wParam;
}