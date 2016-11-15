#include "../sampleApp.h"
#include "mmsystem.h"
#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "MXml.h"
#include "RFont.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
#include "RRenderHelper.h"
//#include "RPortalSceneManager.h"
#include "RActor.h"

using namespace rs3;

#include "d3dx9.h"

class myApplication : public sampleApp {
//	RActor				m_actor;
//	RPortalSceneManager	*portalManager;	// 테스트용
	bool				m_bWireframe;
public:
	myApplication() : sampleApp(), m_bWireframe(false) { }

	char *GetName() { return "rs3 indoor sample application"; }

	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	void OnPreRender();
	void OnRender();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
} g_App;

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'Q'	:	m_bWireframe = !m_bWireframe;
//		case 'T'	:	m_bBlend=!m_bBlend;break;
		}
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

bool myApplication::OnCreate()
{
	//RMeshMgr *pMeshMgr = RMeshMgr::GetInstance();
	//RMesh *pMesh = pMeshMgr->CreateMesh("player/human_male/human_male.elu");
	//if(!pMesh) return false;

//	m_actor.Create("prison/Prison_new_portal.elu");

//	m_actor.TurnOffShade();

	
//	portalManager->LoadXML("prison/Prison.xml");
//	portalManager->LoadXML("../Data/Zone/Riode/Riode.zone.xml");
	m_pSceneManager->LoadRootSceneFromXML("../Data/Zone/Riode/Riode.zone.xml");

// 
// 	portalManager->BeginBuild();
// 	portalManager->AddActor(&m_actor);
// 	portalManager->EndBuild();
// 
	m_pCamera->SetNearFarZ(10,100000);
	m_cameraTargetPos = RVector(37000,47000,900);
	return true;
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();
// 
// 	list<RCell*> cells;
// 	portalManager->GetCells(cells,m_pCamera->GetWorldPosition());
// 
// 	string szAreaName = cells.empty() ? "n/a" : "";
// 
// 	for(list<RCell*>::iterator itr = cells.begin();itr!=cells.end();itr++)
// 		szAreaName = szAreaName + (*itr)->m_szName.c_str() + " ";
// 
// 	char szBuffer[256] = "";
// 	sprintf(szBuffer,"FPS %3.1f (%s) %d nodes,  q : wireframe",
// 		GetDevice()->GetFrameRate(),szAreaName.c_str(),portalManager->GetNodeRenderedCount()
// 		);
// 	SetText(szBuffer);

}

void myApplication::OnPreRender()
{
	REngine::GetDevice().SetFillMode(m_bWireframe ? RFILL_WIRE : RFILL_SOLID);
}

void myApplication::OnRender()
{
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,25,20,20,0x80808080);
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,100,5,5,0xffffffff);
}

void myApplication::OnDestroy()
{
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	return g_App.Run();
}