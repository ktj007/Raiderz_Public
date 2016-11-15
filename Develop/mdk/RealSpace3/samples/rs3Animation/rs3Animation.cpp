#include "stdafx.h"

#include "MDebug.h"
#pragma warning( disable : 4996 )

#include "mmsystem.h"
#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "MXml.h"
#include "RFont.h"
//#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTextSceneNode.h"
//#include "RAABBTreeSceneNode.h"
//#include "MCollision.h"
#include "RMeshMgr.h"
#include "RActor.h"
#include "../sampleApp.h"
#include "RSkeleton.h"
#include "RRenderHelper.h"
#include "ROverlayAnimationController.h"
#include "RBlendAnimationController.h"
#include "RDirectionalLightSceneNode.h"
#include "RActorPassRenderer.h"
#include "RUtil.h"
#include "RAnimationController_New.h"

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.
#include <afxpropertygridctrl.h>


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원


#include "MFCProperty/ReflectiveUI.h"
#include "ControlPanelDialog.h"

using namespace rs3;

#include "d3dx9.h"

const char* const g_szAnimationNames[] = {
	"1hs_idle",
	"1hs_run_f",
	"1hs_run_l",
	"1hs_run_r",
	"1hs_guard_idle",
};

const char* const BEATENANIMATION = "1hs_MF_BT2_1";
const char* const SPELLFIREANIMATION = "1hs_spell_buff_fire";

// data/model 뒤에 경로
const char* const ACTOR_MESH = "Mesh/hf/hf.elu";
const char* const WEAPON_MESH = "Mesh/weapons/1hd_03.elu";
const char* const WEAPON_MESH_NODE = "1hd_03";
const char* const WEAPON_ACTOR_NODE = "dummy_1hs_r";
const int g_nAnimation = _countof(g_szAnimationNames);


#include "reflection\BaseObject.h"

#define DECLARE_PROPERTY(x,y)	x y;\
	x		get##y(void)	{ return y;	}	\
	void	set##y(x _t)	{ 	y = _t;	}

class Properties : public CRTTIClass<Properties, CBaseObject,0x01>
{	
public:
// 	DECLARE_PROPERTY( int, Animation1 );
//	DECLARE_PROPERTY( float, Weight1 );
// 	DECLARE_PROPERTY( int, Animation2 );
	DECLARE_PROPERTY( float, Weight2 );
// 	DECLARE_PROPERTY( int, Animation3 );
	DECLARE_PROPERTY( float, Weight3 );

	float		getMinWeight(void)			{ return 0.f;		};
	float		getMaxWeight(void)			{ return 1.f;		};

	int		getMinAnimation(void)			{ return 0;		};
	int		getMaxAnimation(void)			{ return g_nAnimation-1;		};

	static	void RegisterReflection();

	Properties()
		: Weight3(1.f)
	{

	}
};

void Properties::RegisterReflection()
{
// 	RegisterProperty<int>("animation1",&getAnimation1,&setAnimation1,&getMinAnimation,&getMaxAnimation);
//	RegisterProperty<float>("weight1",&getWeight1,&setWeight1,&getMinWeight,&getMaxWeight);

// 	RegisterProperty<int>("animation2",&getAnimation2,&setAnimation2,&getMinAnimation,&getMaxAnimation);
	RegisterProperty<float>("weight2",&getWeight2,&setWeight2,&getMinWeight,&getMaxWeight);

// 	RegisterProperty<int>("animation3",&getAnimation3,&setAnimation3,&getMinAnimation,&getMaxAnimation);
	RegisterProperty<float>("weight3",&getWeight3,&setWeight3,&getMinWeight,&getMaxWeight);
}

const int NUM_CONTROLLERS = 3;

class myApplication : public sampleApp
{
	RActor			*m_pActor;
	RMesh			*m_pWeaponMesh;
	bool			m_bShowBones;

	RAnimationController_New*		m_pAnimationControllers[NUM_CONTROLLERS];

	int				m_nBaseAnimation;

	RDirectionalLightSceneNode *m_pDirLight;

	CReflectiveUI			m_UI;
	Properties				m_properties;

public:
	myApplication() 
		: sampleApp() 
		,m_bShowBones(false)
		,m_pWeaponMesh(NULL) { }

	char *GetName() { return "rs3 animation sample application"; }

	bool OnCreate();
	void OnDestroy();
	void OnUpdate();
	void OnRender();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);

	void OnAnimationBeaten();
	void OnAnimationFire();

	void OnBaseAnimation(int nAnimaion);

	virtual void OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent) override;

} g_App;

void myApplication::OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
{
	// 애니메이션 이벤트 받는곳

}

bool myApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
    switch (message)
    {
	case WM_KEYDOWN:
		if('0'<=wParam &&  wParam<='9')
			OnBaseAnimation((int)wParam-'0');
		else
		switch(wParam)
		{
		case 'B'	:	m_bShowBones=!m_bShowBones;break;
		case 'M'	:	OnAnimationBeaten();break;
		case 'F'	:	OnAnimationFire();break;
		}
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

CWinAppEx theApp;	// 이게 있어야 에러가 안나던데 mfc 왜 이럼 -_-;


bool myApplication::OnCreate()
{
	// 액터생성
	RSceneManager* pSceneManager = GetSceneManager();
	_ASSERT( pSceneManager );

	m_pActor = new RActor;

	// 테스트용 모델
	if(!m_pActor->Create(ACTOR_MESH))
	{
		SAFE_DELETE(m_pActor);
		return false;
	}
	pSceneManager->AddSceneNode(m_pActor);

	m_pWeaponMesh = pSceneManager->CreateResource<RMesh>(WEAPON_MESH);
	bool bRet = m_pWeaponMesh->Create();
	_ASSERT(bRet);
	RMeshNode* pWeaponMeshNode = m_pWeaponMesh->GetNode(WEAPON_MESH_NODE);
	_ASSERT(pWeaponMeshNode != NULL );
	RActorNode* pWeaponActorNode = m_pActor->GetActorNode(WEAPON_ACTOR_NODE);
	_ASSERT(pWeaponActorNode != NULL );

	m_pActor->AddMeshNode(pWeaponMeshNode, pWeaponActorNode);

	m_pActor->SetListener( this );

	////////////// controllers
	for(int i=0;i<NUM_CONTROLLERS; i++)
	{
		m_pAnimationControllers[i] = new RAnimationController_New;
		m_pActor->AddController(m_pAnimationControllers[i]);
	}

	m_fDistToCamera = 0;
	m_fCameraZ = MMath::HALFPI;
	m_fCameraX = 1.6f;
	m_cameraTargetPos = RVector(0,-200,80);
	m_bFreeCameraMove = false;

	// light
	m_pDirLight = new RDirectionalLightSceneNode;
	GetSceneManager()->AddSceneNode(m_pDirLight);

	// 기본 애니메이션 실행
	OnBaseAnimation(1);

	m_pAnimationControllers[1]->SetBlendMode( RBLENDMODE_BLEND );
	m_pAnimationControllers[1]->SetAnimation( g_szAnimationNames[2] );
	m_pAnimationControllers[1]->Play();

	// mfc 초기화
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << "MFC failed to initialize!" << endl;
		return 1;
	}

	ControlPanelDialog *m_pTestDlg = new ControlPanelDialog;
	m_pTestDlg->Create(ControlPanelDialog::IDD);

	RECT rt;
	GetWindowRect( m_hWnd, &rt );

	RECT rtDlg;
	GetWindowRect( m_pTestDlg->GetSafeHwnd(), &rtDlg );

	m_pTestDlg->MoveWindow(rt.right, rt.top, rtDlg.right-rtDlg.left, rtDlg.bottom-rtDlg.top);
	m_pTestDlg->ShowWindow(SW_NORMAL);

	// ui
	CReflectivePropertyGridCtrl* pPropGtrl =	m_UI.createPropGridCtrl( m_pTestDlg );			//이 다이알로그를 부모로 가지는 프로퍼티그리드콘트롤 생성
	CMFCPropertyGridProperty* pGroup =			m_UI.createGridGroup(pPropGtrl,"Animation Weights");			//프로퍼티그리드 그룹생성
	m_UI.BuildUIForObject(pGroup,&m_properties);												//해당그룹내에 프로퍼티그리드아이템추가
	pPropGtrl->ExpandAll();

	return true;
}

void myApplication::OnDestroy()
{
	if(m_pActor)
	{
		m_pActor->RemoveFromParent();
		SAFE_DELETE(m_pActor);
	}
	GetSceneManager()->ReleaseResource( m_pWeaponMesh );

	if(m_pDirLight)
		m_pDirLight->RemoveFromParent();

	for(int i=0; i<NUM_CONTROLLERS; i++)
	{
		SAFE_DELETE(m_pAnimationControllers[i]);
	}

	SAFE_DELETE(m_pDirLight);
}

void myApplication::OnUpdate()
{
	sampleApp::OnUpdate();

	char szBuffer[1024] = "";
	sprintf(szBuffer,"FPS %3.1f Active(%s)\nB : ShowBones\nM : mix test (%s)\nF : mix test (%s)",
		GetDevice()->GetFrameRate(),
		IsActive() ? "o" : "x",
		BEATENANIMATION,
		SPELLFIREANIMATION
		);

	char szBuffer2[256];
	for(int i=0;i<g_nAnimation; i++)
	{
		sprintf( szBuffer2, "\n%d : %s", i, g_szAnimationNames[i]);
		strcat(szBuffer, szBuffer2);
	}

	for(int i=0;i<NUM_CONTROLLERS; i++)
	{
		RAnimationController_New* pController = m_pAnimationControllers[i];
		const char* szAnimName = pController->GetAnimation() ? pController->GetAnimation()->GetAliasName().c_str() : "none";
		sprintf( szBuffer2, "\ncontroller%d : %s , %d", i, szAnimName, pController->GetFrame());
		strcat(szBuffer, szBuffer2);
	}

	SetText(szBuffer);

//	m_pAnimationControllers[0]->SetWeight( m_properties.getWeight1() );
	m_pAnimationControllers[1]->SetWeight( m_properties.getWeight2() );
	m_pAnimationControllers[2]->SetWeight( m_properties.getWeight3() );
}

void myApplication::OnRender()
{
	if(m_bShowBones)
	{
		m_pDevice->SetDepthEnable(false,false);
		m_pActor->RenderSkeleton();
		m_pDevice->SetDepthEnable(true);
	}
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	return g_App.Run();
}

void myApplication::OnBaseAnimation(int nAnimaion)
{
	if(g_nAnimation <= nAnimaion)
		return;

	m_nBaseAnimation = nAnimaion;
	
	m_pAnimationControllers[0]->SetAnimation( g_szAnimationNames[nAnimaion] );
	m_pAnimationControllers[0]->Play();
}

void myApplication::OnAnimationBeaten()
{
	RPLAYINFO playInfo;
	playInfo.blendMode = RBLENDMODE_BLEND;
	playInfo.fPlaySpeed = 2.0f;

	m_pAnimationControllers[2]->SetAnimation( BEATENANIMATION, &playInfo );
	m_pAnimationControllers[2]->Play();
}

void myApplication::OnAnimationFire()
{
	RActorNodeSelection nodeSelection;
	nodeSelection.SelectNodeAndChildren(m_pActor , "Bip01 L Clavicle");

	RPLAYINFO playInfo;
	playInfo.blendMode = RBLENDMODE_BLEND;
	playInfo.pNodeSet = &nodeSelection;
	playInfo.fBlendIn = 0.3f;
	playInfo.fBlendOut = 0.3f;

	m_pAnimationControllers[2]->SetAnimation( SPELLFIREANIMATION, &playInfo );
	m_pAnimationControllers[2]->Play();
}
