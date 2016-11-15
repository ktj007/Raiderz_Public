#include "stdafx.h"

#include "Main.h"
#include "RConfiguration.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RRenderHelper.h"
#include "RPointLightSceneNode.h"
#include "RDirectionalLightSceneNode.h"
#include "RActorPassRenderer.h"
#include "RShaderCompositeManager.h"

using namespace rs3;


#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

static float fSecPerFrame = 1.0f / 60.0f; 
static float fTimer = 0;
RTimer tmp_timer; 

//////////////////////////////////////////////////////////////

CMcvMain::CMcvMain() : m_pActor(NULL)
{
	m_fWheel	= 0.f;
	m_nWheel	= 0;

	m_vModelScale	= RVector(1.f,1.f,1.f);
	m_vLightPos		= RVector(0.f,0.f,0.f);
	m_vMapObjPos	= RVector(0.f,0.f,0.f);

	m_hWnd = NULL;

	m_bFullScreen = false;
	m_bActive = true;

	m_nWidth = 800;
	m_nHeight = 600;

	m_pDevice = NULL;

	m_pSelectedActorNode = NULL;
	m_pPointLights = NULL;
	m_pDirectionalLight = NULL;
	m_bDirectionalLightMatchCamera = false;

	m_bShowBones		= false;
	m_bShowCollision	= false;

	m_fRotateZ = 0;

	tmp_timer.Reset();
}

CMcvMain::~CMcvMain()
{
	Destroy();
}
RSceneManager* CMcvMain::CreateSceneManager()
{
	class CUSTOM_SCENEMANAGER : public RSceneManager
	{
	public:
		//virtual RSceneNode* CreateSceneNode(const char* pChar)
		//{
		//	RSceneNode* pSceneNode = physx::CreateRPhysXSceneNode(pChar);
		//	if ( pSceneNode == NULL )
		//	{
		//		pSceneNode = RSceneManager::CreateSceneNode(pChar);
		//	}
		//	return pSceneNode;
		//}
	};

	return new CUSTOM_SCENEMANAGER;
}

bool CMcvMain::Create(HWND hWnd)
{
	RECT rect;

	m_hWnd = hWnd;

	::GetClientRect(hWnd,&rect);

	m_nWidth = rect.right - rect.left;
	m_nHeight = rect.bottom - rect.top;
	m_bFullScreen = false;

	REngine::GetConfig().m_nWidth = m_nWidth;
	REngine::GetConfig().m_nHeight = m_nHeight;
//	,m_nWidth,m_nHeight,m_bFullScreen,RFMT_ARGB32

	m_pDevice = new RDeviceD3D;
	((RDeviceD3D*)m_pDevice)->Create(m_hWnd);
	m_pDevice->ResetDevice();
	REngine::RegisterDevice(m_pDevice);

	m_pManager = CreateSceneManager();
	REngine::RegisterSceneManager(m_pManager);
	m_pManager->Create(m_pDevice);
	bool bInited = REngine::InitSharedResource();
	if(!bInited)
	{
		REngine::DestroySharedResource();
		SAFE_DELETE(m_pDevice);

		MessageBox( NULL, "can't initialize. please check config.xml", "error", MB_OK );
		return false;
	}

	//RSetSceneManager( m_pManager );

	//m_pManager->GetPassRenderer< RActorPassRenderer >();

	m_pManager->AddSceneNode(&m_Camera);
	m_pManager->SetPrimaryCamera(&m_Camera);

	// init actor
	m_pActor = new RActor;
	m_pManager->AddSceneNode(m_pActor);

//	m_pDevice->SetClearFlag(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);

	////////////////////////////////////////////////////////

//	m_Camera.Create(m_nWidth,m_nHeight);
	m_Camera.SetNearFarZ( 10.f, 50000.f );
	m_Camera.ResetSize();
	m_Camera.SetClearColor( (DWORD)0xFFD0D0D0 );

	m_vLightPos = RVector(42.f,-28.f,150.f);//캐릭터 바운딩 박스의 중간 앞쪽..

	m_pDevice->SetAmbient(D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 0.5f, 1.0f ));
	m_pDevice->SetTextureFilter(0,RTF_BILINEAR);
	m_pDevice->SetTextureMipmapLodBias(0,-0.5f);

	RBatch::m_bisToolApp = true;

	CameraReset();

	InitLight();

	return true;
}

void CMcvMain::InitLight()
{
	m_pDirectionalLight = new RDirectionalLightSceneNode;
	m_pDirectionalLight->SetLightAmbient(RVector4(1,1,1,1));
	m_pDirectionalLight->SetLightDiffuse(RVector4(0,0,0,0));
	m_pManager->AddSceneNode(m_pDirectionalLight);

	m_pPointLights = new RPointLightSceneNode[NUM_POINTLIGHTS];
	for( unsigned int i = 0; i < NUM_POINTLIGHTS; ++i )
	{
		m_pManager->AddSceneNode(&m_pPointLights[i]);
		SetPointLightEnable( false, i );
	}
}

void CMcvMain::Restore()
{
}

void CMcvMain::Destroy()
{
//	REffectMgr::GetInstance()->Destroy();
//	RMeshMgr::GetInstance()->DelAll();
//	RMeshMgr::GetInstance()->Destroy();

	m_Camera.RemoveFromParent();
	m_Camera.Destroy();

	if(m_pActor)
	{
		m_pActor->RemoveFromParent();
		m_pActor->Destroy();
		SAFE_DELETE(m_pActor);
	}

	if(m_pPointLights)
	{
		for( int i = 0; i < NUM_POINTLIGHTS; ++i )
		{
			m_pPointLights[i].RemoveFromParent();
		}
		SAFE_DELETE_ARRAY(m_pPointLights);
	}

	if(m_pDirectionalLight)
	{
		m_pDirectionalLight->RemoveFromParent();
		SAFE_DELETE(m_pDirectionalLight);
	}

	// ㅜ.ㅜ [9/11/2006 madduck]
//	REffectMgr::GetInstance()->Destroy();

	REngine::DestroySharedResource();
	m_pManager->Destroy();
	SAFE_DELETE(m_pManager);
	REngine::RegisterSceneManager(NULL);

	SAFE_DELETE(m_pDevice);
	REngine::RegisterDevice(NULL);
}

void CMcvMain::Update()
{
	static DWORD thistime , lasttime = timeGetTime() , elapsed;

	thistime = timeGetTime();

	elapsed  = (thistime - lasttime) * (IsKeyDown(VK_SHIFT) ? 5:1);

	lasttime = thistime;

	float fsp = elapsed * 0.1f;

	UpdateKey(fsp);

	m_Camera.ResetSize();
	m_Camera.UpdateData();

	if( m_bDirectionalLightMatchCamera )
		UpdateDirLightMatchCamera();

	RBatch::m_vToolLightPos = m_vLightPos;

	// 선택된 노드가 있으면 업데이트 전에 애니메이션 강제 업데이트
	if(m_pSelectedActorNode)
	{
		m_pSelectedActorNode->ForceUpdateAnimation();
	}

	
	if( m_pActor )
	{
		// 애니메이션이 없어도 Visibility업데이트를 적용하기 위해.
		if( GetMainView()->isAlpha2PassRendering() )
			m_pActor->SetAlpha2PassRendering( true );
		else
			m_pActor->SetAlpha2PassRendering( false );

		RSceneNodeUpdateVisitor visitor;
		m_pActor->Traverse(&visitor);

		m_pManager->GetCurrentRootSceneNode(false)->Traverse(&visitor);	// 090605
	}

	m_pManager->UpdateTimer();
	m_pManager->Update();
}
#include "RAnimationResource.h"



// 에니메이션 되는 것 사각형 그린다
bool CMcvMain::RenderAllActorBoundBox( const RSceneNodeList& nodes)
{
	for( RSceneNodeList::const_iterator itr = nodes.begin(); itr!=nodes.end(); ++itr)
	{
		RSceneNode* pSceneNode = *itr;
		if( MIsDerivedFromClass( RActorNode, pSceneNode)) 
		{
			RActorNode* pActorNode = (RActorNode*)pSceneNode;
			RBoundingBox box = pActorNode->m_pMeshNode->m_boundingBox;
			if(!pActorNode->m_pMeshNode->isRenderAble())
				box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
			RMatrix matTM = pActorNode->GetWorldTransform();
			RRenderHelper::RenderBox(matTM,box,0xffffffff);

		}

		// 모든 뎁스를 돌게 재귀적 호출
		RenderAllActorBoundBox( pSceneNode->GetChildren());
	}

	return true;
}




void CMcvMain::Render()
{
	CMcvView* pView = GetMainView();

	float fTime = timeGetTime() * 0.001f;

	m_pDevice->Clear(true,true,false, m_Camera.GetClearColor() );
	m_pDevice->BeginScene();

	m_pDevice->ShaderOff();

	m_pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);

	m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	m_pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	m_pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
	m_pDevice->SetTexture( 0, R_NONE);
	m_pDevice->SetTexture( 1, R_NONE);
	m_pDevice->SetLighting( false );
	m_pDevice->SetDepthEnable(true,true);

	m_pDevice->SetTransform(RST_WORLD,RMatrix::IDENTITY);

//	const RSceneNodeList& topLevelNodes = m_pManager->GetRootSceneNode().GetChildren();
//	RenderAllActorBoundBox( topLevelNodes);	// 모든 액터 노드 바운딩 그려준다. - 090605, OZ

	m_pManager->RenderPrimaryCamera();

	if( pView->isDrawGrid() )
	{
		DrawGrid( RVector(-2000, 2000, -0.05f), 100.f, 100.f, 40, 40, 0xFFC0C0C0);
		DrawCenter( 4000, 4000, 0xFF808080);
	}

	RRenderHelper::SetRenderState();

	if(pView->isDrawBBox())
	{
		if ( pView->GetActor()->GetAnimation())
		{
			RAnimationResource* pAnimationResource = pView->GetActor()->GetAnimation()->GetResource();
			if(pAnimationResource->GetMaxAniBoundingBox())
				RRenderHelper::RenderBox(RMatrix::IDENTITY,*pAnimationResource->GetMaxAniBoundingBox());
		}
	}

	if(m_bShowBones)
	{
		m_pDevice->SetDepthEnable(false,false);
		pView->GetActor()->RenderSkeleton();
	}

	m_pDevice->ShaderOff();

	m_pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);

	m_pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0, RTSS_COLORARG1, RTA_DIFFUSE);
	m_pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	m_pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
	m_pDevice->SetTexture( 0, R_NONE);
	m_pDevice->SetTexture( 1, R_NONE);
	m_pDevice->SetLighting( false );
	m_pDevice->SetDepthEnable(true,true);

	m_pDevice->SetTransform(RST_WORLD,RMatrix::IDENTITY);

	RRenderHelper::SetRenderState();
	RRenderHelper::RenderAxis(RMatrix::IDENTITY,20);

	// draw light box
	if ( 0)
	{
		RMatrix matLight;
		matLight.SetTranslationMatrix(m_vLightPos);

		RRenderHelper::RenderSphere(matLight,10,0xffff0000);

		if(pView->isDrawBBox())
		{
			RBoundingBox box( RVector(-5.f,-5.f,-5.f), RVector(5.f,5.f,5.f) );
			RRenderHelper::RenderBox( matLight, box, 0xffffffff);
		}
//			draw_box(&matLight,RVector(5.f,5.f,5.f),RVector(-5.f,-5.f,-5.f),0xffffffff);
	}

	//////////////////////////////////////////////////////////

	m_pDevice->SetLighting(true);

	RActor* pActor = m_pActor;
	bool bTraceDummyLoc = false;

	CButton* pCheck = (CButton*)GetModelInfoSidebar()->GetDlgItem( IDC_CHECK_TRACE_LOC);
	if ( pCheck  &&  pCheck->GetCheck() == TRUE)
		bTraceDummyLoc = true;

	if ( pActor)
	{
		bool bExistDummy = false;
		RActorNode* pActorNode = pActor->GetActorNode( "dummy_loc");
		if ( pActorNode)
		{
			RAnimationControllerNode* pControllerNode = pActor->GetAnimationController().GetNodeController( "dummy_loc");
			if ( pControllerNode)
			{
				RAnimationNode* pAnimationNode = pControllerNode->GetAnimationNode();
				if ( pAnimationNode && pAnimationNode->m_positionKeyTrack.size() > 2)
					bExistDummy = true;
			}
		}


		if ( pActorNode  &&  bExistDummy  &&  bTraceDummyLoc == true)
		{
			pActorNode->ForceUpdateAnimation();
			vec3 _pos( pActorNode->GetLocalTransform()._41, pActorNode->GetLocalTransform()._42, pActorNode->GetLocalTransform()._43);
			vec3 _dir = pActorNode->GetDirection();

			RMatrix matTM;
			matTM.SetLocalMatrix( _pos, _dir, vec3::AXISZ);

			pActor->SetTransform( matTM);
		}
		else
		{
			pActor->SetTransform( vec3::ZERO, vec3::AXISY, vec3::AXISZ);
		}
	}

	if(GetMainView()->isVertexNormal())
	{
		for(int i=0;i<pActor->GetActorNodeCount();i++)
		{
			pActor->GetActorNodes()[i]->RenderWire();
			pActor->GetActorNodes()[i]->RenderNormal();
		}
	}

	CString strNodeTMInfo;
	if(m_pSelectedActorNode)
	{
		m_pSelectedActorNode->RenderWire();
		m_pDevice->SetDepthEnable(false,false);
		RBoundingBox box = m_pSelectedActorNode->m_pMeshNode->m_boundingBox;
		if(!m_pSelectedActorNode->m_pMeshNode->isRenderAble())	// dummy or bone
			box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
		RMatrix matTM;
		if ( bTraceDummyLoc == true  &&  m_pSelectedActorNode->GetNodeName() == "dummy_loc")
			matTM = m_pSelectedActorNode->GetLocalTransform();
		else
			matTM = m_pSelectedActorNode->GetWorldTransform();
//		RMatrix matTM = m_pSelectedActorNode->GetResultTransform() * m_pSelectedActorNode->m_pActor->GetWorldTransform();
		RRenderHelper::RenderBox(matTM,box,0xffffffff);
		RRenderHelper::RenderAxis(matTM,30,0.5f);

		RVector pos(matTM._41,matTM._42,matTM._43);
		char szBuffer[256];
		sprintf( szBuffer, "( %.1f,  %.1f,  %.1f )",matTM._41,matTM._42,matTM._43 );
		strNodeTMInfo = szBuffer;
		RVector dir = Normalize(m_vLightPos-pos);

//		RRenderHelper::RenderArrow(pos,dir,1,10,3,3,0xff808080);
	}
	else
	{
		strNodeTMInfo = "( 0.0,  0.0,  0.0 )";
	}

	static DWORD time = timeGetTime();
	DWORD curr = timeGetTime();
	if ( curr > (time + 300))
	{
		GetModelInfoSidebar()->m_nodeTMInfo.SetWindowText( strNodeTMInfo );
		GetModelInfoSidebar()->m_nodeTMInfo.InvalidateRect(NULL);
		time = curr;
	}

	m_pDevice->EndScene();
	m_pDevice->Flip();
}

////////////////////////////////////////////////////////////////////

void CMcvMain::UpdateCameraKey(float time)
{
	// 모드에 따라서 작동이 달라져야함
	// 카메라를 중심캐릭터를 비추는 모드
	// 카메라가 마음대로 돌아다니는 모드

	//////////////////////////////////
	// 각도 조정

	if( GetMainView()->isCharacterView() ) {

		// 카메라 이동

		if(IsKeyDown('Q'))		m_Camera.Up(time);		// 높이조정
		if(IsKeyDown('E'))		m_Camera.Down(time);
		if(IsKeyDown('W'))		m_Camera.m_fDist -= (time*4);	// 줌인 아웃
		if(IsKeyDown('S'))		m_Camera.m_fDist += (time*4);
		if(IsKeyDown('A'))		m_Camera.CenterRotationLeft(time);
		if(IsKeyDown('D'))		m_Camera.CenterRotationRight(time);	// 그냥 회전

		if(IsKeyDown(VK_LEFT))	m_Camera.CenterRotationLeft(time);	
		if(IsKeyDown(VK_RIGHT))	m_Camera.CenterRotationRight(time);

	} else {
	
		if(IsKeyDown('Q'))		m_Camera.Up(time);
		if(IsKeyDown('E'))		m_Camera.Down(time);
		if(IsKeyDown('W'))		m_Camera.Front(time*4);
		if(IsKeyDown('S'))		m_Camera.Back(time*4);
		if(IsKeyDown('A'))		m_Camera.Left(time*4);
		if(IsKeyDown('D'))		m_Camera.Right(time*4);

		if(IsKeyDown(VK_LEFT))	m_Camera.CenterRotationLeft(time);
		if(IsKeyDown(VK_RIGHT))	m_Camera.CenterRotationRight(time);

	}

	if(IsKeyDown(VK_UP))	m_Camera.CenterRotationDown(time);
	if(IsKeyDown(VK_DOWN))	m_Camera.CenterRotationUp(time);

	if(IsKeyDown(VK_OEM_4))	m_fRotateZ+= .01f*time;	// [
	if(IsKeyDown(VK_OEM_6))	m_fRotateZ-= .01f*time;	// ]
}

void CMcvMain::UpdateDirLightMatchCamera()
{
	if( m_pDirectionalLight )
	{
		m_pDirectionalLight->SetLightPosition( m_Camera.GetPosition() );
		m_pDirectionalLight->SetLightDirection( ( m_Camera.m_vTargetPos - m_Camera.GetPosition() ) );
	}
}

void CMcvMain::UpdateKey(float time)
{
	//////////////////////////////////
	// 지금윈도우가 최상위인경우

	BOOL bWindowTop = GetMainView()->IsTopParentActive();
	
	if(bWindowTop &&  GetMainView()->GetActiveWindow() != GetMainView()->GetAniBlendDlg() ) {

		UpdateCameraKey(time);
		UpdateLightKey(time);

		if(IsKeyDown(VK_SPACE)) {
//			CameraReset();
		}
	

		if(IsKeyDown('G'))
			GetMainView()->GridOnOff();

		if(m_fWheel != 0) {
			m_Camera.m_fDist = (-m_fWheel * 0.1f) + m_Camera.m_fDist;
			m_fWheel = 0;
		}
	}

}

void CMcvMain::UpdateLightKey(float time)
{
	RVector vecForward = m_Camera.GetDir();
	RVector vecRight = CrossProduct(m_Camera.GetDir(),RVector(0,0,1));
	RVector vecUp = CrossProduct(vecRight,vecForward);

	if(IsKeyDown(VK_NUMPAD4))	m_vLightPos -= time*2 * vecRight;
	if(IsKeyDown(VK_NUMPAD6))	m_vLightPos += time*2 * vecRight;

	if(IsKeyDown(VK_NUMPAD8))	m_vLightPos += time*2 * vecUp;
	if(IsKeyDown(VK_NUMPAD2))	m_vLightPos -= time*2 * vecUp;

	if(IsKeyDown(VK_NUMPAD3))	m_vLightPos -= time*2 * vecForward;
	if(IsKeyDown(VK_NUMPAD9))	m_vLightPos += time*2 * vecForward;

//	SetLight(0, m_vLightPos );
//	SetLight(1, m_Camera.GetPos() );

	m_pPointLights[0].SetPosition(m_vLightPos);
}

void CMcvMain::CameraReset()
{
	if(m_pActor->m_pMesh)
		m_Camera.Reset(m_pActor->m_pMesh->GetStaticBBox());
	else
		m_Camera.Reset();

	m_vLightPos = RVector(42.f,-28.f,150.f);//캐릭터 바운딩 박스의 중간 앞쪽..
	m_vMapObjPos = RVector(0,0,0);
}

void CMcvMain::SetSpecularEnable( bool bEnable )
{
	REngine::GetSceneManagerPtr()->GetShaderCompositeManager()->SetSpecularEnable( bEnable );
}

void CMcvMain::SetLightingOnlyEnable( bool bEnable )
{
	REngine::GetSceneManagerPtr()->GetShaderCompositeManager()->SetViewLightingOnly( bEnable );
}

void CMcvMain::SetDirLightDiffuseColor( DWORD dwR, DWORD dwG, DWORD dwB )
{
	if( m_pDirectionalLight )
		m_pDirectionalLight->SetLightDiffuse( ConvertColor( dwR, dwG, dwB ) );
}

void CMcvMain::SetDirLightAmbientColor( DWORD dwR, DWORD dwG, DWORD dwB )
{
	if( m_pDirectionalLight )
		m_pDirectionalLight->SetLightAmbient( ConvertColor( dwR, dwG, dwB ) );
}

void CMcvMain::SetPointLightEnable( bool bEnable, unsigned int index )
{
	if( m_pPointLights && index < NUM_POINTLIGHTS )
	{
		if( bEnable )
			m_pPointLights[index].m_fApplyIntensity = m_pPointLights[index].m_fIntensity;
		else
			m_pPointLights[index].m_fApplyIntensity = 0.0f;
	}
}

RVector4 CMcvMain::ConvertColor( DWORD dwR, DWORD dwG, DWORD dwB )
{
	return RVector4( dwR / 255.f, dwG / 255.f, dwB / 255.f, 1.f );
}