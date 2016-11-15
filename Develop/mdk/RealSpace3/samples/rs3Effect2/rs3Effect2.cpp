// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <string>

#include "crtdbg.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "../sampleApp.h"
#include "d3dx9.h"
#include "RSceneNodeID.h"

#include "RActor.h"
#include "RDirectionalLightSceneNode.h"
#include "RTrail.h"

#include <MXml.h>

using namespace rs3;
using namespace std;

const char* ACTOR_MESH = "goblin_Mage/goblin_Mage.elu";
const char* ANIMATION_NAMES[] = { "atk_r"};
const int ANIMATION_COUNT = elementsOf(ANIMATION_NAMES);

class MyApplication : public sampleApp 
{
public:
	RActor			*m_pActors;
	RActorNode		*m_pWeaponNode;
	RTrail			*m_pTrail;

	RDirectionalLightSceneNode *m_pDirLight;

	MyApplication() 
		: sampleApp()
	{ 
		m_pActors = NULL;
		m_pDirLight = NULL;
		
		m_pWeaponNode = NULL;
		m_pTrail = NULL;
	}

	char *GetName() 
	{
		return _T("Trail Effect");
	}
	
	bool OnCreate();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
	void OnUpdate();
	void OnRender();

} g_App;


bool MyApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'Q' :
			PostQuitMessage(0);
		break;
		}
	}

	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

bool MyApplication::OnCreate()
{
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	// light
	m_pDirLight = new RDirectionalLightSceneNode;
	GetSceneManager()->AddSceneNode(m_pDirLight);
	m_pDirLight->SetDirection(RVector(1,0,0));

	m_pActors = new RActor();
	// 테스트용 모델
	pSceneManager->AddSceneNode(m_pActors);

	m_pActors->Create(ACTOR_MESH);
	m_pActors->SetPosition(RVector(0, 0, 0));
	m_pActors->Update();	// aabb 업데이트를 위해

	// 패스 랜더러 사용여부
	m_pActors->SetUsingPassRenderer(true);
	m_pActors->SetAnimation( ANIMATION_NAMES[0] );

	m_pWeaponNode = m_pActors->GetActorNode("Bip01 R Finger0Nub");
	_ASSERT(m_pWeaponNode);

	m_pTrail = new RTrail();
	m_pSceneManager->AddSceneNode(m_pTrail);
	m_pTrail->Start();

	m_fDistToCamera = 0;

	_ASSERT(m_pCamera);
	m_pCamera->SetNearFarZ(10, 50000);

	return true;
}

void MyApplication::OnDestroy()
{
	sampleApp::OnDestroy();

	m_pTrail->RemoveFromParent();
	delete m_pTrail;

	m_pActors->RemoveFromParent();
	delete m_pActors;

	m_pDirLight->RemoveFromParent();
	delete m_pDirLight;
}

void MyApplication::OnUpdate()
{
	if(m_pActors->isOncePlayDone())
		m_pActors->SetAnimation( ANIMATION_NAMES[0] );

	m_pWeaponNode->ForceUpdateAnimation();
	m_pTrail->SetPosition(m_pWeaponNode->GetWorldPosition());
	m_pTrail->UpdateTransform();
	m_pTrail->OnUpdate();
	m_pTrail->OnUpdateAABB();

	sampleApp::OnUpdate();
}

void MyApplication::OnRender()
{
	sampleApp::OnRender();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	return g_App.Run();
}

