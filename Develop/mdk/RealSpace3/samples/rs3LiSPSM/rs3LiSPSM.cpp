#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>

#include "crtdbg.h"
#include "mmsystem.h"
#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"

#include "RShaderFX.h"

#include "RShadowStrategy.h"
#include "RLightManager.h"
#include "RDirectionalLightSceneNode.h"
#include "REnvironmentSet.h"

#include <algorithm>

using namespace std;
using namespace rs3;

#include "d3dx9.h"

const char* ZONE_NAME = "Zone/Login/Login.zone.xml";
const char* ZONE_ENV_FILE_NAME = "Zone/Login/Login.zone.env.xml";
const char* ACTOR_MESH = "goblin_commander/goblin_commander.elu";

class myApplication : public sampleApp {
public:
	myApplication() : sampleApp() { }

	char *GetName() { return "rs3 LiSPSM sample application"; }
	bool OnCreate();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
	void OnUpdate();
	void OnRender();
	bool OnInit() {
		if(!sampleApp::OnInit())
			return false;
//		REngine::GetConfig().m_strTexturePath = "tile;treetexture;map";
//		REngine::GetConfig().SetTreePath("treemodels");
		REngine::GetConfig().depthFormat = RFMT_D24S8;
		return true;
	}

protected:
	bool			NextMethod();
	bool			NextFilter();

	bool			m_bLightRotate;

	int				m_nMethod;
	int				m_nFilterMethod;

	bool			m_bViewDepthMap;
	RVector			m_lightDir;
	float			m_zFarMax;

	REnvironmentSet* m_pEnvironmentSet;
	RActor			*m_pActor;

} g_App;

const float ZNEAR_MIN = 100.0f;
const float FOV = DegreeToRadian(60);

bool myApplication::NextMethod()
{
	m_nMethod = (m_nMethod+1) % SHADOW_END;
	m_pSceneManager->SetRealtimeShadowMethod( (RSHADOWMETHOD)m_nMethod, 1024, 3 );
	return true;
}

bool myApplication::NextFilter()
{
	m_nFilterMethod = (m_nFilterMethod+1) % SF_END;
	m_pSceneManager->SetShadowFilterType( (RSHADOWFILTERMETHOD)m_nFilterMethod);
	return true;
}

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
	case WM_KEYDOWN:
		switch(wParam)
		{
//		case 'R' : aabbmethod=1-aabbmethod; return true;
		case 'F'	: return NextFilter();
		case 'M'	: return NextMethod();
//		case 'O'	: m_pSceneManager->SetShadowMapEnable(!m_pSceneManager->GetShadowMapEnable());return true;
		case 'L'	: m_bLightRotate = !m_bLightRotate;return true;
		case 'V'	: m_bViewDepthMap = !m_bViewDepthMap;return true;
		}break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

//RAABBTreeSceneNode *pTree = NULL;


HRESULT CheckResourceFormatSupport(IDirect3DDevice9* m_pd3dDevice, D3DFORMAT fmt, D3DRESOURCETYPE resType, DWORD dwUsage)
{
	HRESULT hr = S_OK;
	IDirect3D9* tempD3D = NULL;
	m_pd3dDevice->GetDirect3D(&tempD3D);
	D3DCAPS9 devCaps;
	m_pd3dDevice->GetDeviceCaps(&devCaps);

	D3DDISPLAYMODE displayMode;
	tempD3D->GetAdapterDisplayMode(devCaps.AdapterOrdinal, &displayMode);

	hr = tempD3D->CheckDeviceFormat(devCaps.AdapterOrdinal, devCaps.DeviceType, displayMode.Format, dwUsage, resType, fmt);

	tempD3D->Release(), tempD3D = NULL;

	return hr;
}

bool myApplication::OnCreate()
{
	m_zFarMax = 30000.0f;

	m_bLightRotate = true;
	m_bViewDepthMap = false;

	GetSceneManager()->LoadRootSceneFromXML(ZONE_NAME);

	// 환경 씬노드 생성
	m_pEnvironmentSet = new REnvironmentSet;

	// 파일로부터 만들기
	bool bRet = m_pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);

	// 씬매니저에 더하기
	REngine::GetSceneManager().AddSceneNode(m_pEnvironmentSet);

	// 환경 설정
	bRet = m_pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY);

// 	GetSceneManager()->SetShadowMapEnable(true);
// 	GetSceneManager()->m_pShadowMap->SetMethod(RSHADOWMAP_LISPSM);

// 	RMeshMgr::GetInstance()->AddXml("../Data/Model/Player/human_male/human_male.xml","human_male");
// 	RMesh *pMesh = RMeshMgr::GetInstance()->Get("human_male");
// 	if(!pMesh) return false;

	m_pActor = new RActor;
	m_pActor->Create(ACTOR_MESH);
	m_pActor->SetAnimation("walk");
	m_pActor->Play();
	m_pActor->SetPosition(RVector(0,500,0));
	m_pActor->SetDynamicShadowCaster(true);
	m_pCamera->AddChild(m_pActor);

	m_nMethod = 0;
	m_cameraTargetPos = RVector(16000,7000,100);
	NextMethod();

	return true;
}

void myApplication::OnDestroy()
{
	if(m_pEnvironmentSet)
	{
		m_pEnvironmentSet->RemoveFromParent();
		SAFE_DELETE(m_pEnvironmentSet);
	}

	if(m_pActor)
	{
		m_pActor->RemoveFromParent();
		SAFE_DELETE(m_pActor);
	}
}

void myApplication::OnUpdate()
{
	m_pCamera->SetFov(FOV);
	m_pCamera->SetNearFarZ(ZNEAR_MIN,m_zFarMax);

	float fElapsed = 0.1f * GetDevice()->GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 15 : 1);

	if(m_nMethod!=0)
	{
		static float fAngle = 0.f;
		if(m_bLightRotate)
			fAngle+=m_pSceneManager->GetTimer().GetElapsedTime()*0.001f;

		fAngle = fmod(fAngle,MMath::PI);
		RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetLightManager()->GetDefaultLight();
		m_lightDir = RVector(cosf(fAngle),.5f,-sinf(fAngle));
		m_lightDir.Normalize();
		pDirLight->SetDirection(m_lightDir);
	}

	if(IsKeyDown(VK_ADD)) m_zFarMax += fElapsed;
	if(IsKeyDown(VK_SUBTRACT)) m_zFarMax -= fElapsed;

	sampleApp::OnUpdate();

	const char* szMethods[] = { "none", "LiSPSM","PSSM", "ESM" };

	const char* szFilterMethods[] = { "none", "pcf","poisson", "pcss" };

//	char szExtra[256] = "";
	//switch(m_nMethod)
	//{
	//case 1 :sprintf(szExtra,"%s %3.3f, %3.3f ", (m_bShadowTestInverted ? "inv" : "") , m_ppNear,m_ppFar );break;
	//case 2 : sprintf(szExtra,"nopt %3.3f " ,m_fLSPSM_NoptWeight);break;
	//default: szExtra[0] = 0;break;
	//}

	char szBuffer[256];
	sprintf(szBuffer,"FPS %3.1f , method %s filter %s ( zn %3.3f zf %3.3f )",GetDevice()->GetFrameRate(), 
		szMethods[m_nMethod],szFilterMethods[m_nFilterMethod], m_pCamera->GetNearZ(), m_pCamera->GetFarZ());
	SetText(szBuffer);
	
}

void myApplication::OnRender()
{
// 	if(m_bViewDepthMap && GetSceneManager()->m_pShadowMap)
// 		GetSceneManager()->m_pShadowMap->RenderShadowMap(150,150,200,200);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//	_CrtSetBreakAlloc(37082);

	return g_App.Run();
}

