#include <windows.h>
#include "crtdbg.h"
#include "mmsystem.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RStaticMeshSceneNode.h"
#include "RSimpleRotationSceneNode.h"
#include "RMultiPassLightSceneNode.h"
#include "RSimpleSphereSceneNode.h"
#include "RCollision.h"
#include "RGlobalGlowSceneNode.h"
#include "../sampleApp.h"

using namespace rs3;

#include "d3dx9.h"

class myApplication : public sampleApp {
	RStaticMeshSceneNode *m_pTree;
	RSimpleRotationSceneNode *m_pRotation;
	RMultiPassLightSceneNode *m_pLight;
	RSimpleSphereSceneNode *m_pLightSphere;
	RCollisionSphere *m_pCollision;

	RGlobalGlowSceneNode	*m_pGlow;

public:
	myApplication() : sampleApp(), m_pCollision(NULL) {}

	char *GetName() { return "rs3 glow sample application"; }
	bool OnCreate();
	void OnDestroy();
	void OnCameraTargetMove(const RVector &to);;
} g_App;

bool myApplication::OnCreate()
{
	// by pok, TODO : 포비든 템플이 없어졌네요.
	if(!GetSceneManager()->LoadRootSceneFromXML("Gunz2Test/Gunz2Test.zone.xml")) return false;

	// Glow
	m_pGlow = new RGlobalGlowSceneNode;
	GetSceneManager()->AddSceneNode(m_pGlow);

	// TODO: 공유자원 처리
	RGlobalGlowPreSceneNode::Create();

	// collision 테스트
//	m_pCollision = new RCollisionSphere(50.f);
//	GetSceneManager()->AddSceneNode(m_pCollision);
//	m_pCollision->SetPosition(RVector(0,0,0));

	// 자동 회전
	m_pRotation = new RSimpleRotationSceneNode;
//	m_pRotation->SetHierachy(true);
	m_pRotation->m_fRotationSpeed = 0.5f;
	m_pRotation->SetPosition(RVector(0,0,200));
	m_pCamera->AddChild(m_pRotation);

	// 라이트
	m_pLight = new RMultiPassLightSceneNode;
//	m_pLight->SetHierachy(true);
	m_pLight->SetPosition(RVector(400,0,0));
	m_pLight->m_fRadius = 1000.f;
	m_pRotation->AddChild(m_pLight);

	m_pLightSphere = new RSimpleSphereSceneNode;
	m_pLight->AddChild(m_pLightSphere);
//	m_pLightSphere->SetHierachy(true);
	m_pLightSphere->m_fRadius = 10.f;

	return true;
}

void myApplication::OnCameraTargetMove(const RVector &to)
{
	RVector outMove;
//	m_pCollision->MoveTestSliding( m_cameraTargetPos , to, &outMove);
//	m_pCollision->SetPosition(outMove);
//	m_cameraTargetPos = outMove;
	m_cameraTargetPos = to;
}

void myApplication::OnDestroy()
{
	if(m_pLightSphere)
	{
		m_pLightSphere->RemoveFromParent();
		SAFE_DELETE(m_pLightSphere);
	}
	if(m_pLight)
	{
		m_pLight->RemoveFromParent();
		SAFE_DELETE(m_pLight);
	}
	if(m_pRotation)
	{
		m_pRotation->RemoveFromParent();
		SAFE_DELETE(m_pRotation);
	}
	SAFE_DELETE(m_pCollision);

	if(m_pTree)
	{
		m_pTree->RemoveFromParent();
		SAFE_DELETE(m_pTree);
	}
	if(m_pGlow)
	{
		// TODO: 공유자원 처리
		RGlobalGlowPreSceneNode::Destroy();
		m_pGlow->RemoveFromParent();
		SAFE_DELETE(m_pGlow);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	return g_App.Run();
}