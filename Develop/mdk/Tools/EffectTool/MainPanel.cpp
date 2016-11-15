// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "MainPanel.h"
#include "MainFrame.h"

#include "MFileSystem.h"

#include "REngine.h"
#include "RDeviceD3D.h"
#include "RActor.h"
#include "RActorNodeConstraintController.h"
#include "REffectSceneNode.h"
#include "RDirectionalLightSceneNode.h"
#include "RRenderHelper.h"

#include "Config.h"
#include "TSceneManager.h"
#include "TCamera.h"

namespace EffectTool
{

BEGIN_EVENT_TABLE(MainPanel, wxPanel)
	EVT_IDLE(MainPanel::OnIdle)
	EVT_SIZE(MainPanel::OnSize)
	EVT_MOUSE_EVENTS(MainPanel::OnMouseEvent)
END_EVENT_TABLE()

MainPanel::MainPanel(wxWindow* parent, wxWindowID winid) :
 wxPanel(parent, winid)
,m_pDevice(NULL)
,m_pSceneManager(NULL)
,m_pCamera(NULL)
,m_pActor(NULL)
,m_pSelectedActorNode(NULL)
,m_pEffectSceneNode(NULL)
,m_pSelectedChild(NULL)
,m_bDrawHelperGrid(true)
,m_bDrawHelperAxis(true)
,m_bLightToCameraDir(true)
{
}

MainPanel::~MainPanel()
{
	DestroyDevice();
}

void MainPanel::Init()
{
	InitDevice();

	CreateDefaultEffectSceneNode();

	bool bRet = CreateDefaultActor();
	_ASSERT(bRet);
}

void MainPanel::InitDevice()
{
	int nWidth = 100;
	int nHeight = 100;
	GetSize(&nWidth, &nHeight);
	REngine::GetConfig().m_nWidth = nWidth;
	REngine::GetConfig().m_nHeight = nHeight;

	HWND hWnd = GetHWND();
	m_pDevice = new RDeviceD3D();
	m_pDevice->Create(hWnd);
	REngine::RegisterDevice(m_pDevice);

	m_pSceneManager = new TSceneManager;
	REngine::RegisterSceneManager(m_pSceneManager);
	m_pSceneManager->Create(m_pDevice);
	REngine::InitSharedResource();

	m_pDevice->SetAmbient(D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 0.5f, 1.0f ));

	_ASSERT(NULL == m_pCamera);
	m_pCamera = new TCamera();
	m_pCamera->ResetSize();
	m_pCamera->AddAttribute(RSNA_DO_NOT_SAVE);
	m_pCamera->SetNearFarZ(10.f, 50000.f);
	m_pSceneManager->AddSceneNode(m_pCamera);
	m_pSceneManager->SetPrimaryCamera(m_pCamera);
	m_pCamera->SetClearColor( D3DCOLOR_ARGB(255, 53, 53, 53));
	m_pCamera->Reset(RVector(0.f,0.f,0.f));

	RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
	if(!pDirLight)
	{
		pDirLight = new RDirectionalLightSceneNode();
		m_pSceneManager->AddSceneNode(pDirLight);
		pDirLight->SetLightAmbient(0,0,0,1);
	}

	//m_pFont = new RFont;
	//m_pTextNode = new RTextSceneNode;

	//m_pFont->Create(GetDevice(),L"Arial", 16);
	//m_pTextNode->SetFont(m_pFont);
	//m_pTextNode->SetColor(0xffff0000);
	//m_pTextNode->SetText("");
	//m_pTextNode->SetPosition(1,1);
	//m_pTextNode->AddAttribute(RSNA_DO_NOT_SAVE);
	//m_pSceneManager->AddSceneNode(m_pTextNode);

	m_pSceneManager->SetDistortionEnable(true);
	m_pSceneManager->SetHDREnable(true);
}

void MainPanel::DestroyDevice()
{
	//m_pTextNode->RemoveFromParent();
	//SAFE_DELETE(m_pTextNode);
	//SAFE_DELETE(m_pFont);

	if(m_pCamera)
	{
		m_pCamera->RemoveFromParent();
		m_pCamera->Destroy();
	}
	SAFE_DELETE(m_pCamera);

	DeleteEffectSceneNode();
	DeleteActor();

//	SAFE_DELETE(m_pCopiedSceneNode);

	RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
	if(pDirLight)
	{
		pDirLight->RemoveFromParent();
		SAFE_DELETE(pDirLight);
	}

	if(m_pSceneManager)
	{
		m_pSceneManager->Clear();
		REngine::DestroySharedResource();
		m_pSceneManager->Destroy();
	}
	SAFE_DELETE(m_pSceneManager);
	REngine::RegisterSceneManager(NULL);

	if(m_pDevice)
	{
		m_pDevice->Destroy();
		delete m_pDevice;
		m_pDevice = NULL;
		REngine::RegisterDevice(NULL);
	}
}

void MainPanel::DeviceSetting()
{
}

void MainPanel::PreOrderRender()
{
	RRenderHelper::SetRenderState();

	if(m_bDrawHelperGrid)
	{
		RMatrix matCenter;

		matCenter.MakeIdentity();
		matCenter.SetTranslation(RVector( -500.f, -500.f, 0.f ));
		RRenderHelper::RenderGrid(matCenter, 10.f, 100, 100, 0xff898989);

		matCenter.MakeIdentity();
		matCenter.SetTranslation(RVector( -2000.f, -2000.f, 0.f ));
		RRenderHelper::RenderGrid(matCenter, 100.f, 40, 40, 0xff8f8f8f);

		RRenderHelper::RenderCenter(RMatrix::IDENTITY, 4000.f, 4000.f, 0xff88ff88);
	}

	if(m_bDrawHelperAxis)
		RRenderHelper::RenderAxis(RMatrix::IDENTITY, 20.f);

	RRenderHelper::EndRenderState();

//	m_pDevice->SetFillMode(m_fillMode);
}

void MainPanel::PostOrderRender()
{
	//DrawBoxSelectedNode();
	//DrawEmitter(m_bShowEmitter);
	//DrawEmitterAABB(m_bShowEmitter);
	//DrawActor(m_bShowEmitter);
	//DrawLightAABB(m_bShowLight);
}

bool MainPanel::CreateEffectSceneNode(const wxString& fileName)
{
	DeleteEffectSceneNode();

	wxString strFileNameWithExt = MGetPureFileNameExt(fileName.ToStdString());
	if(false == strFileNameWithExt.Contains(EFFECT_FILE_EXT))
		strFileNameWithExt += EFFECT_FILE_EXT;

	wxString strFileName = CConfig::GetInstance().m_strEffectPath + strFileNameWithExt;

	_ASSERT(m_pSceneManager);
	if(false == m_pSceneManager->LoadWorldSceneNodeFromRootXMLFile(m_pSceneManager->GetCurrentRootSceneNode(true), strFileName.c_str()))
		return false;

	const RSceneNodeList& topLevelNodes = m_pSceneManager->GetCurrentRootSceneNode(false)->GetChildren();
	for( RSceneNodeList::const_iterator itr = topLevelNodes.begin(); itr!=topLevelNodes.end(); ++itr)
	{
		RSceneNode* pSceneNode = *itr;
		if( MIsDerivedFromClass(REffectSceneNode, pSceneNode) )
		{
			// 두개 이상있으면 에러
			_ASSERT( NULL == m_pEffectSceneNode );
			m_pEffectSceneNode = MDynamicCast(REffectSceneNode, pSceneNode);
		}
	}

	if(NULL == m_pEffectSceneNode)
	{
		_ASSERT(false);
		return false;
	}

	m_effectFileName = strFileNameWithExt;

	m_pEffectSceneNode->UpdateRenderableNodesList();

	OnCreateEffectSceneNode();

	m_pEffectSceneNode->Start();
	return true;
}

void MainPanel::CreateDefaultEffectSceneNode()
{
	m_effectFileName = "";

	DeleteEffectSceneNode();
	_ASSERT(NULL == m_pEffectSceneNode);

	m_pEffectSceneNode = new REffectSceneNode;
	// REffectSceneNode가 기본적으로 usable이 false이므로 설정해주지 않으면 제대로 업데이트가 동작하지 않는다
	m_pEffectSceneNode->SetUsable(true);
	AttachEffect(m_pEffectSceneNode, m_pActor, m_pSelectedActorNode);

	m_pEffectSceneNode->SetNodeName("ef");

	OnCreateEffectSceneNode();
}

void MainPanel::DeleteEffectSceneNode()
{
	m_pSelectedChild = NULL;

	if(m_pEffectSceneNode)
	{
		m_pEffectSceneNode->Clear();
	}
	SAFE_DELETE( m_pEffectSceneNode );
}

void MainPanel::OnIdle(wxIdleEvent& event)
{
	UpdateKeys();
	Update();
	Render();

	event.RequestMore();
}

void MainPanel::OnSize(wxSizeEvent& event)
{
	if(NULL == m_pDevice)
		return;

	if(NULL == m_pCamera)
		return;

	wxSize size = event.GetSize();

	_ASSERT( size.x > 0 && size.y > 0 );

	REngine::GetConfig().m_nWidth = size.x;
	REngine::GetConfig().m_nHeight = size.y;

	m_pCamera->SetFov((float)CConfig::GetInstance().CAMERA_FOV, (float)size.x/(float)size.y);
	m_pDevice->ResetDevice();
}

void MainPanel::OnMouseEvent(wxMouseEvent& event)
{
	if(NULL == m_pCamera)
		return;

	int istat = 0;
	if( event.LeftIsDown() )
		istat = 1;
	else if( event.RightIsDown() )
		istat = 2;
	else if( event.MiddleIsDown() )
		istat = 3;

	if( istat > 0 )
		SetFocus();

	long x = 0;
	long y = 0;
	event.GetPosition( &x, &y );
	RVector2 pos( x, y );

	int delta = event.GetWheelRotation();

	m_pCamera->OnMouseMove( istat, pos, delta );
}

void MainPanel::OnCreateEffectSceneNode()
{
	MainFrame* mainFrame = wxDynamicCast(GetParent(), MainFrame);
	_ASSERT(mainFrame);

//	RefreshSceneNodeTreeView();
	if(m_pEffectSceneNode)
	{
		mainFrame->RefreshEffectSceneTreeCtrl(m_pEffectSceneNode);
	}

//	UpdateCaption();
//	ReSelectActorNode();
}

void MainPanel::UpdateKeys()
{
	#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	static DWORD thistime , lasttime = timeGetTime() , elapsed;
	thistime = timeGetTime();
	elapsed  = (thistime - lasttime) * (IsKeyDown(VK_SHIFT) ? 1:3);
	lasttime = thistime;
	float time = elapsed * 0.03f;

	if(false == HasFocus())
		return;

	if(IsKeyDown('Q'))		m_pCamera->Up(time); // 높이조정
	if(IsKeyDown('E'))		m_pCamera->Down(time);
	if(IsKeyDown('W'))		m_pCamera->m_fDist -= (time*4); // 줌인 아웃
	if(IsKeyDown('S'))		m_pCamera->m_fDist += (time*4);
	if(IsKeyDown('A'))		m_pCamera->CenterRotationLeft(time); // 그냥 회전
	if(IsKeyDown('D'))		m_pCamera->CenterRotationRight(time);

	if(m_pEffectSceneNode && IsKeyDown(VK_SPACE))
		m_pEffectSceneNode->Start();
}

void MainPanel::UpdateLightToCamera()
{
	if(m_bLightToCameraDir)
	{
		RDirectionalLightSceneNode* pDirLight = m_pSceneManager->GetDefaultLight();
		if(pDirLight)
		{
			pDirLight->SetLightPosition( m_pCamera->GetPosition() );
			pDirLight->SetLightDirection( m_pCamera->m_vTargetPos - m_pCamera->GetPosition() );
		}
	}
}

void MainPanel::Update()
{
	if(NULL == m_pSceneManager)
		return;

	m_pCamera->UpdateData();
	UpdateLightToCamera();

//	PFC_B("UpdateAndCull");

	//if( GetActor() )
	//{
	//	RSceneNodeUpdateVisitor v;
	//	GetActor()->Traverse( &v );
	//}

	m_pSceneManager->UpdateTimer();
	m_pSceneManager->Update();

//	PFC_E;

	//if( m_pSelectedChild == NULL && m_pSelectedEffectActor == NULL)
	//	GlobalObjects::g_pMainForm->childTabForm->DisableAllControls();

	//if( m_bAnimationLoop && GetActor() && GetActor()->GetAnimationController().IsPlayDone())
	//{
	//	GetActor()->GetAnimationController().SetFrame(0);
	//	GetActor()->Play();
	//}
}

void MainPanel::Render()
{
	if(NULL == m_pDevice)
		return;

	m_pDevice->Clear(true, true, false, m_pCamera->GetClearColor());
	m_pDevice->BeginScene();

	m_pSceneManager->RenderPrimaryCamera(this);

	m_pDevice->EndScene();
	m_pDevice->Flip();
}

bool MainPanel::CreateActor(const wxString& fileName)
{
	DeleteActor();
	_ASSERT(NULL == m_pActor);
	m_pActor = new RActor;
	m_pActor->AddAttribute(RSNA_DO_NOT_SAVE);

	if(!m_pActor->Create(fileName.c_str()))
	{
		SAFE_DELETE(m_pActor);
		mlog("파일 로드 실패 - %s\n", fileName);
		return false;
	}

	m_actorFileName = fileName;

	m_pActor->SetPosition(RVector(0,0,0));
	m_pSceneManager->AddSceneNode(m_pActor);

	//RefleshAniTreeView();
	//SetNodeListTreeView();

	mlog("%s 로드 완료 \n", fileName);
	return true;
}

bool MainPanel::CreateDefaultActor()
{
	wxString defaultEluFilename = wxString( CConfig::GetInstance().m_strEffectPath ) + "test dummy.elu";

	bool result = CreateActor(defaultEluFilename);
	if(result)
	{
		_ASSERT(m_pEffectSceneNode);
		m_pEffectSceneNode->RemoveFromParent();
		AttachEffect( m_pEffectSceneNode, m_pActor, m_pActor->GetActorNode("Dummy02") );
	}

	return result;
}

void MainPanel::DeleteActor()
{
	m_pSelectedActorNode = NULL;

	if(m_pActor)
	{
		m_pActor->RemoveFromParent();
		m_pActor->Destroy();
	}
	SAFE_DELETE(m_pActor);
}

void MainPanel::AttachEffect(REffectSceneNode* pEffectSceneNode, RSceneNode* pSceneNode, RActorNode* pActorNode)
{
	RSceneNodeController* pOldController = pEffectSceneNode->GetFirstControllerByName(RSNCTRL_PREUPDATE, RCID_ACTORNODECONSTRAINT);
	if(pOldController)
	{
		pEffectSceneNode->RemoveController(pOldController);
		delete pOldController;
	}

	m_pSceneManager->AttachEffect(pEffectSceneNode, pSceneNode, pActorNode);
	RSceneNodeController* pNewController = pEffectSceneNode->GetFirstControllerByName(RSNCTRL_PREUPDATE, RCID_ACTORNODECONSTRAINT);
	if(pNewController)
		pNewController->AddAttribute(RCA_STICKY);
}

}