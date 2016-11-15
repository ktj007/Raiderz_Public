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

class myApplication : public sampleApp
{
private:
	std::vector<RActor*>	m_vActors;
	RDirectionalLightSceneNode *m_pDirLight;

	void CreateActor();
	
public:
	myApplication() 
		: sampleApp()
	{ 
	}

	char *GetName() { return "rs3 actor sample application"; }
// 	bool OnInit() {	REngine::GetConfig().SetPath(RP_MESH, "../Data/Model"); return sampleApp::OnInit(); }

	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
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
		case 'P':
			CreateActor();
		break;
		}
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

void myApplication::CreateActor()
{
	// 咀磐 积己
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT(pSceneManager);

	RActor *pActor = new RActor();
	_ASSERT(pActor);

	int nSize = static_cast<int>(m_vActors.size());
	pSceneManager->AddSceneNode(pActor);
	pActor->Create(ACTOR_MESH);
	int x = nSize % 10;
	int y = nSize / 10;

	pActor->SetPosition( x * 170.f, y * 170.f, 0.0f);
	pActor->SetUsingPassRenderer(true);

	RSceneNodeUpdateVisitor updateVisitor;
	pActor->Traverse( &updateVisitor );

	m_vActors.push_back(pActor);
}

bool myApplication::OnCreate()
{
	// 咀磐积己
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	// light
	m_pDirLight = new RDirectionalLightSceneNode;
	GetSceneManager()->AddSceneNode(m_pDirLight);
	m_pDirLight->SetDirection(RVector( 1, 0, 0));

	m_vActors.clear();
	CreateActor();

	m_fDistToCamera = 200;
	m_cameraTargetPos = RVector( 100, 0, 50 );
	m_fCameraZ = 1.6f;
	m_fCameraX = 2.0f;

	return true;
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();

	char szBuffer[128];
	
	szBuffer[0] = '\0';
	sprintf(szBuffer,"FPS %5.2f , Actor count : %d\n", m_pDevice->GetFrameRate(), m_vActors.size());
	SetText(szBuffer);
}

void myApplication::OnRender()
{
}

void myApplication::OnDestroy()
{
	if ( m_vActors.empty() == false )
		for  ( size_t si = 0; si<m_vActors.size(); ++si )
		{
			m_vActors[si]->RemoveFromParent();
			delete m_vActors[si];
		}

	m_vActors.clear();

	if ( m_pDirLight )
	{
		m_pDirLight->RemoveFromParent();
		delete m_pDirLight;
		m_pDirLight = NULL;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	g_App.Run();

	PFC_FINALANALYSIS("rs3actor.txt");
}