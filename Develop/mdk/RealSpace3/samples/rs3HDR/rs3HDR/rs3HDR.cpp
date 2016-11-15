#include "rs3HDR.h"

#include "RRenderHelper.h"
#include "RDebugPassRenderer.h"
#include "REnvironmentSet.h"
#include "RHDRCameraSceneNode.h"

using namespace rs3;

//////////////////////////////////////////////////////////////////////////
// Global
//////////////////////////////////////////////////////////////////////////
float g_fMoveSpeed	= 20;

bool MyApplication::Create()
{
	REngine::GetConfig().m_nWidth	= 1218;
	REngine::GetConfig().m_nHeight	= 768;

	return RD3DApplication::Create(); 
}

bool MyApplication::OnCreate()
{
	m_pFont		= new RFont;
	m_pTextNode = new RTextSceneNode;
	m_pCamera	= new RHDRCameraSceneNode;
	
	ResetCursor();
	// font
	m_pFont->Create(m_pDevice,_T("굴림"),12);
	// text 테스트 노드
	m_pTextNode->SetFont(m_pFont);
	m_pTextNode->SetText("test");
	m_pTextNode->SetPosition(0,0);
	GetSceneManager()->AddSceneNode(m_pTextNode);

	// 카메라
	GetSceneManager()->AddSceneNode(m_pCamera);
	GetSceneManager()->SetPrimaryCamera(m_pCamera);

	//m_pDevice->SetLighting(false);
	//m_pDevice->SetDepthBias(0);

	DragAcceptFiles(GetWindowHandle(), TRUE);
	bool bBackgroundLoad = false;
	if (bBackgroundLoad)
		REngine::GetBackgroundWorker();

	CreateScene();

	return true;
}

void MyApplication::CreateScene()
{
	const char *ZONE_FILE_NAME = "../Data/Zone/Login/Login.zone.xml";
	const char *ZONE_ENV_FILE_NAME = "../Data/Zone/Login/Login.zone.env.xml";

	// 존 로딩
	bool bRet = REngine::GetSceneManager().LoadRootSceneFromXML(ZONE_FILE_NAME);
	_ASSERT(bRet);

	// 환경 씬노드 생성
	m_pEnvironmentSet = new rs3::REnvironmentSet;
	m_pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);
	REngine::GetSceneManager().AddSceneNode(m_pEnvironmentSet);
	m_pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY);

	// 카메라 셋팅
	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	_ASSERT(m_pCamera);
	m_pCamera->SetNearFarZ(10, 50000);

	m_pCamera->SetPosition(RVector3(15211.0f, 7969, 2027.0f));
	m_pCamera->SetDirection(RVector3(-0.5f, -0.8f, 0.17f));

	m_cameraPos = m_pCamera->GetPosition();
	m_cameraDir = m_pCamera->GetDirection();
}

void MyApplication::OnDestroy()
{
	// 환경 씬노드 소멸
	if(m_pEnvironmentSet)
	{
		m_pEnvironmentSet->RemoveFromParent();
		delete m_pEnvironmentSet;
	}
}


POINT g_Pt;
static bool g_bRbuttonDown = false;
static bool g_bLbuttonDown = false;
static bool g_bMbuttonDown = false;
bool MyApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_CHAR:
		if(wParam==VK_ESCAPE)
			PostMessage(hWnd,WM_CLOSE,0,0);
		break;

	case WM_MOUSEWHEEL:
		{
			// 값의 + 가 휠이 위로, -가 휠이 아래로 내려가는걸 나타냅니다.
			if((INT)wParam < 0) 
				m_fDistToCamera += g_fMoveSpeed;
			else 
				m_fDistToCamera -= g_fMoveSpeed;
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

void MyApplication::OnUpdate()
{
#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	DWORD elapsed = GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 10 : 1);

	float fRotateStep = elapsed*0.001f;
	float fMoveStep = (float)elapsed;

	if(IsActive() && g_bLbuttonDown)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(GetWindowHandle(),&pt);
		// mouse control
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x-GetDevice()->GetScreenWidth()/2;
		iDeltaY = pt.y-GetDevice()->GetScreenHeight()/2;

		float RotateAxisZ = -iDeltaX*0.01f;
		float RotateAxisRight = -iDeltaY*0.01f;

		// update direction
		RMatrix mat;
		mat.MakeIdentity();
		mat.SetRotationZ(RotateAxisZ);

		m_cameraDir =  m_cameraDir * mat;

		RVector vRight = m_pCamera->GetRight();

		mat.MakeIdentity();
		mat.SetRotationMatrixAxis(vRight, RotateAxisRight);

		m_cameraDir = m_cameraDir * mat;

		ResetCursor();
	}

	RVector posDiff(0,0,0);
	if(IsActive()){
		RVector dir = m_cameraDir;
		RVector right = RVector(0,0,1).CrossProduct(dir);
		right.Normalize();

		if(IsKeyDown('W')) posDiff+=fMoveStep*dir;
		if(IsKeyDown('S')) posDiff-=fMoveStep*dir;
		if(IsKeyDown('A')) posDiff+=fMoveStep*right;
		if(IsKeyDown('D')) posDiff-=fMoveStep*right;
		if(IsKeyDown(VK_SPACE)) posDiff+=fMoveStep*RVector(0,0,1);
	} // if(IsActive())

	if(posDiff.Length()>0) 
	{
		m_cameraPos += posDiff;
	}

	m_pCamera->Set(m_cameraPos, m_cameraDir);

	RHDRCameraSceneNode* pHDRCamera = (RHDRCameraSceneNode*)m_pCamera;
	//////////////////////////////////////////////////////////////////////////
	if(IsActive())
	{
		if( IsKeyDown(VK_F1) )
		{
			pHDRCamera->m_pHDRSceneNode->m_bDebugRT = !pHDRCamera->m_pHDRSceneNode->m_bDebugRT;
			Sleep(100);
		}

		if(IsKeyDown(VK_OEM_COMMA)) 
		{
		//	m_pCamera->RenderScreenShot("11.bmp");
		//	pHDRCamera->m_pHDRSceneNode->m_pTexSceneScaled->SaveToFile("Scaled.dds");
		}

		if(IsKeyDown(VK_OEM_PERIOD)) 
		{
	
		}

		if(IsKeyDown(VK_OEM_4))	//  '[{' for US
		{
			pHDRCamera->m_pHDRSceneNode->m_fKeyValue -= 0.1f;
		}

		if(IsKeyDown(VK_OEM_6))	//  ']}' for US
		{	

			pHDRCamera->m_pHDRSceneNode->m_fKeyValue += 0.1f;
		}

		if(IsKeyDown(VK_OEM_2))	// '/'키
		{

		}

		if(IsKeyDown(VK_OEM_PLUS))	// '+'키
		{

		}

		if(IsKeyDown(VK_OEM_MINUS))	// '-'키
		{

		}


	} // if(IsActive())

	char szBuffer[256];
	sprintf_s(szBuffer, "FPS : %3.1f\n CameraPos : %f %f %f\n CameraDir : %f %f %f\n KeyValue : %f\n",
		GetDevice()->GetFrameRate(), 
		m_cameraPos.x, m_cameraPos.y, m_cameraPos.z, 
		m_cameraDir.x, m_cameraDir.y, m_cameraDir.z,
		pHDRCamera->m_pHDRSceneNode->m_fKeyValue);

	SetText(szBuffer);
}

void MyApplication::OnRender()
{

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

	return g_App.Run();
}
