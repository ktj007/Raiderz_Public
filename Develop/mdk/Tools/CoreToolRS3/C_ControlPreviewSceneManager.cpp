#include "stdafx.h"
#include "C_ControlPreviewSceneManager.h"

#include "RVType.h"
#include "RCameraSceneNode.h"

#include "RTreePassRenderer.h"
#include "RWaterPassRenderer.h"

#include "C_XMLXConst.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperRender.h"

#include "M_ToolSceneNodeInstance.h"

using namespace rs3;

static const UINT TEXTURE_SIZE = 512;

static RVt_pos_tex_rhw vRenderTargetVertex[4] =
{
	MVector4(0, 0, 0, 1)	, 0.0f, 0.0f, 
	MVector4(TEXTURE_SIZE, 0, 0, 1)	, 1.0f, 0.0f, 
	MVector4(0, TEXTURE_SIZE, 0, 1)	, 0.0f, 1.0f, 
	MVector4(TEXTURE_SIZE, TEXTURE_SIZE, 0, 1), 1.0f, 1.0f, 
};

CControlPreviewSceneManager *CControlPreviewSceneManager::m_pPtr = NULL;

CControlPreviewSceneManager::CControlPreviewSceneManager()
: m_hRenderTarget(R_NONE)
, m_pPreviewSceneManager(NULL)
, m_hDepthBuffer(R_NONE)
, m_pPreviewCamera(NULL)
, m_pDevice(NULL)
, m_pControlMediator(NULL)
{
}

CControlPreviewSceneManager::~CControlPreviewSceneManager()
{
}

void CControlPreviewSceneManager::Release()
{
	//카메라 지우기
	if(m_pPreviewCamera != NULL)
	{
		m_pPreviewCamera->DestroyBuffers();
		delete m_pPreviewCamera;
		m_pPreviewCamera = NULL;
	}
	SAFE_DELETE(m_pPtr);
}

bool CControlPreviewSceneManager::Init(CControlMediator* pControlMediator, RDeviceD3D *pDevice, RToolSceneManager *pSceneManager)
{
	ASSERT(pDevice!=NULL);
	ASSERT(pSceneManager!=NULL);

	m_pDevice = pDevice;
	m_pPreviewSceneManager = pSceneManager;

	m_pPreviewCamera = new RCameraSceneNode();
	m_pPreviewCamera->SetPosition(0, 0, 0);
	m_pPreviewCamera->SetDirection(RVector(0, 1, 0));
	m_pPreviewCamera->UpdateTransform();
	m_pPreviewCamera->SetFov(DegreeToRadian((float)XCONST::CAMERA_FOV), 1.0f);
	m_pPreviewCamera->SetNearFarZ( 5.0f, 100000.0f);
	m_pPreviewCamera->UpdateViewFrustrum();
	m_pPreviewCamera->SetActive(true);
	m_pPreviewCamera->SetClearColor( D3DCOLOR_XRGB(255, 255, 255));

	m_pPreviewCamera->CreateRenderTarget(TEXTURE_SIZE, TEXTURE_SIZE, 0);
	m_pPreviewCamera->CreateDepthBuffer(TEXTURE_SIZE, TEXTURE_SIZE);

	m_hRenderTarget = m_pPreviewCamera->GetRenderTarget(0);
	m_hDepthBuffer = m_pPreviewCamera->GetDepthBuffer();

	m_pControlMediator = pControlMediator;

	return true;
}

void CControlPreviewSceneManager::UpdateRender(RSceneNode *pNode)
{
	// 물체를 고정해두고, 카메라 위치를 변경
	pNode->SetPosition(RVector(0, 0, 0));
	RSceneNodeUpdateVisitor visitor;
	pNode->Traverse(&visitor);

	RVector vMax = pNode->GetAABB().vmax;
	RVector vMin = pNode->GetAABB().vmin;
	RVector vCenter = ( vMax + vMin ) * 0.5f;
	RVector vHalf = vMax - vCenter;

	float fLength = vHalf.Length() + 30.f;
	RVector vPos = RVector(1.f,-1.f,1.f) * fLength;
	RVector vAt = vCenter;
	RVector vDir = vAt - vPos;
	vDir.Normalize();
	m_pPreviewCamera->SetTransform(vPos, vDir, RVector(0, 0, 1));

	// SetRender State
	m_pDevice->SetDepthEnable( true, true);
	m_pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);

	// begin scene
	m_pDevice->BeginScene();

	//m_pPreviewCamera->ClearCull(RRP_CAMERA,(RRP_MAX-1));
	//pNode->Cull(m_pPreviewCamera);

	m_pPreviewCamera->BeginRender();
	m_pPreviewCamera->OnClearBuffer();
	//m_pPreviewCamera->RenderPass(RRP_RENDER_BEGIN, RRP_RENDER_END_ALL);
	m_pPreviewCamera->EndRender();
	m_pPreviewCamera->OnEndRender();

	// end scene
	m_pDevice->EndScene();

	// point light list
	m_pControlMediator->GetHelperRender()->ClearPointLightRenderList();
}

rs3::RSceneNode*  CControlPreviewSceneManager::LoadSceneNode(const char *szFilename, const char *szNodeName)
{
	RSceneNode* pSceneNode = m_pPreviewSceneManager->CreateSceneNode(szNodeName);

	bool bRet = m_pPreviewSceneManager->LoadSceneNodeFromFile( pSceneNode, szFilename );
	if ( !bRet )
	{
		delete pSceneNode;
		return NULL;
	}

	if (_stricmp(pSceneNode->GetNodeIDString(), RSID_TREE) == 0 )
		pSceneNode->SetScale(33.0f);

	pSceneNode->UpdateTransform();
	return pSceneNode;
}