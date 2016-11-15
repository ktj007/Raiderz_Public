#include "StdAfx.h"
#include "RSpotLightSceneNode.h"
#include "RDevice.h"
#include "RRenderHelper.h"
#include "RDirectionalLightSceneNode.h"
#include "RUtil.h"
#include "RToken.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"

// TODO: 
// * 중복될필요없는것들을 static 으로 만든다.
// * z-fighting 문제가 심각하므로, user clipping 을 포기한다.

namespace rs3 {

MImplementRTTI(RSpotLightSceneNode, RLightSceneNode);

RSpotLightSceneNode::RSpotLightSceneNode(void) : RLightSceneNode(RRP_SPOTLIGHT)
{
	m_LightCamera.SetActive(true);
	m_LightCamera.SetPassIgnore((RRENDERPASS)0);
	m_LightCamera.SetPassIgnore(RRP_SPEEDTREE, false);
	m_LightCamera.SetPassIgnore(RRP_STATICMESH, false);
	m_LightCamera.SetPassIgnore( RRP_TERRAIN, false);
	m_LightCamera.SetPassIgnore( RRP_ACTOR, false);
	m_LightCamera.GetViewFrustumCullingMgr()->SetRenderingList<RShadowRenderingList>();

	m_hSpotLightTexture =NULL;
	m_fFar = 1000.0f;
	m_fFov = 90.0f;
	m_bShadowCast = false;

	RShadowDesc desc;
	desc.eMethod = SHADOW_ESM;
	desc.nTextureSize = 512;
	desc.bDoubleShadow = false;
	m_pShadowStrategy = new RExponentialShadowStrategy(desc);
}

RSpotLightSceneNode::~RSpotLightSceneNode(void)
{
	m_pShadowStrategy->Destroy();
	SAFE_DELETE( m_pShadowStrategy );
	REngine::GetDevice().DeleteTexture(m_hSpotLightTexture);
}


void RSpotLightSceneNode::ShowShadowMapToScreen(int posX, int posY, int nSize)
{
	RShaderFX* pShaderClearFloat = m_pShadowStrategy->GetClearFloatShader();

	if(pShaderClearFloat)
	{
		RRenderHelper::SetRenderState();

		const float VIEW_SIZE = 200.f;

		REngine::GetDevice().SetDepthEnable(false, false);

		REngine::GetDevice().SetTexture(0, GetDepthMap());
		RTechnique tecRenderFloat = pShaderClearFloat->GetTechnique("RenderFloatRenderTarget");
		tecRenderFloat.Begin(NULL, TECH_PRESERVE_NOT);
		tecRenderFloat.BeginPass();
		RRenderHelper::RenderQuad(posX,posY,nSize,nSize);
		tecRenderFloat.EndPass();
		tecRenderFloat.End();

		REngine::GetDevice().SetDepthEnable(true, true);
	}
}

void RSpotLightSceneNode::SetSpotLightTexture(const char* strFilePath)
{
	char strFileName[128];
	SplitPath(strFilePath, NULL, strFileName);
	if( m_strSpotLightTextureFileName == strFileName || strcmp(strFileName, DEFAULT_SPOATMAP)==0)
		return;

	REngine::GetDevice().DeleteTexture(m_hSpotLightTexture);

	m_hSpotLightTexture = REngine::GetDevice().CreateTexture( strFileName, RTF_LINEAR, 0 );
	if(m_hSpotLightTexture)
		m_strSpotLightTextureFileName =strFileName;
}

void RSpotLightSceneNode::SetShadowCast(bool bShadowCast)
{
	m_bShadowCast = bShadowCast;

	m_pShadowStrategy->Destroy();

	if(bShadowCast)
		m_pShadowStrategy->Init();
}

void RSpotLightSceneNode::RenderShadowMap(RCameraSceneNode* pViewCamera)
{
	RPFC_THISFUNC;
	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	
	if( pSceneManager->GetRealtimeShadowMethod() == SHADOW_NONE || !m_bShadowCast)	
		return;

	RWorldSceneNode* pWorldScene = pSceneManager->GetCurrentRootSceneNode(false);
	RCullingMgr* pCullingMgr = m_LightCamera.GetViewFrustumCullingMgr();
	pCullingMgr->Cull(pWorldScene,m_LightCamera.GetViewFrustum().GetPlanes(),6);
	pCullingMgr->CulledListUpdate(&m_LightCamera);

	// 뷰카메라 기준 LOD 계산
	REngine::GetSceneManagerPtr()->GetRenderingStrategy()->CalculateLOD( pViewCamera, &m_LightCamera);

	m_pShadowStrategy->RenderShadowMap(m_LightCamera);
}

RSphere RSpotLightSceneNode::GetBoundingSphere()
{
	RVector3 vPosition = GetWorldPosition();
	RVector3 vDirection = GetWorldDirection();
	float fFov = GetFov()  * (MMath::PI/180.0f);
	float fRadius = (GetFar() * (1+tanf(fFov/2.0f)*tanf(fFov/2.0f)))/2.0f;

	//Actor에 붙어 애니메이션될 경우 부모의 스케일값을 적용하기위해
	//Light의 범위를 스케일
	RVector3 vScale = GetScaleFromWorldTransform();
	float fScale = max(max(vScale.x, vScale.y), vScale.z);
	fRadius *=fScale;

	RVector3 fCenter = vPosition +vDirection.Normalize()*fRadius;

	RSphere sphere;
	sphere.center = fCenter;
	sphere.radius = fRadius;

	return sphere;
}

bool RSpotLightSceneNode::IsInViewFrustum(RCameraSceneNode* pCamera)
{
	RSphere sphere = GetBoundingSphere();

	RViewFrustum viewFrustum = pCamera->GetViewFrustum();
	bool bInViewFrustum = viewFrustum.TestSphere(sphere);

   	return bInViewFrustum;
}

bool RSpotLightSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	char szBuffer[256];
	MXmlElement *pElement = element.FirstChildElement("SPOTMAP");
	if(pElement)
	{
		if (_Contents(szBuffer,pElement))
			SetSpotLightTexture(szBuffer);
	}

	if ( pElement = element.FirstChildElement(RTOK_ATTNEND))
		_Contents(&m_fFar, pElement);

	if ( pElement = element.FirstChildElement(RTOK_ATTNSTART))
		_Contents(&m_fAttenuationStart, pElement);

	if ( pElement = element.FirstChildElement("FOV"))
		_Contents(&m_fFov, pElement);

	bool bShadowCast=false;
	if ( pElement = element.FirstChildElement("SHADOWCAST"))
		_Contents(&bShadowCast, pElement);
	if ( element.FirstChildElement(RTOK_CASTSHADOW) )
		bShadowCast =true;
	SetShadowCast(bShadowCast);
	
	if ( pElement = element.FirstChildElement(RTOK_INTENSITY))
	{
		_Contents(&m_fIntensity, pElement);
		
		// 실제 적용되는 Intensity를 초기화
		m_fApplyIntensity = m_fIntensity;
	}

	if ( pElement = element.FirstChildElement(RTOK_COLOR))
	{
		std::string strContents;
		_Contents(strContents, pElement);
		sscanf(strContents.c_str(),"%f %f %f",&m_color.x,&m_color.y,&m_color.z);
	}

	if ( pElement = element.FirstChildElement("USERENDERMINAREA"))
	{
		bool bUseRenderMinArea =false;
		_Contents(&bUseRenderMinArea, pElement);

		SetUseRenderMinArea(bUseRenderMinArea);
	}
	
	if ( pElement = element.FirstChildElement("RENDERMINAREA"))
	{
		float fRenderMinArea =0.0f;
		_Contents(&fRenderMinArea, pElement);

		SetRenderMinArea(fRenderMinArea);
	}

	//Actor가 부모가 될 경우 Actor와 같이 애니메이션되게 하기 위해
	//매프레임 업데이트해준다.
	if( GetParent() )
		SetAutoUpdate(true);

	return true;
}

void RSpotLightSceneNode::SaveToXML(MXmlElement &element)
{
	MXmlElement *pElement = NULL;

	if(!m_strSpotLightTextureFileName.empty())
	{
		pElement = new MXmlElement("SPOTMAP");
		_SetContents(pElement, m_strSpotLightTextureFileName);
		element.LinkEndChild(pElement);
	}

	pElement = new MXmlElement(RTOK_ATTNEND);
	_SetContents(pElement, GetFar());
	element.LinkEndChild(pElement);

	pElement = new MXmlElement(RTOK_ATTNSTART);
	element.LinkEndChild(pElement);
	_SetContents(pElement, m_fAttenuationStart);

	pElement = new MXmlElement("FOV");
	_SetContents(pElement, GetFov());
	element.LinkEndChild(pElement);

	if(m_bShadowCast)
	{
		pElement = new MXmlElement(RTOK_CASTSHADOW);
		element.LinkEndChild(pElement);
	}

	pElement = new MXmlElement(RTOK_INTENSITY);
	element.LinkEndChild(pElement);
	_SetContents(pElement, m_fIntensity);

	char szBuffer[256];
	pElement = new MXmlElement(RTOK_COLOR);
	sprintf(szBuffer,"%.3f %.3f %.3f",m_color.x,m_color.y,m_color.z);
	_SetContents(pElement, szBuffer);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("USERENDERMINAREA");
	element.LinkEndChild(pElement);
	_SetContents(pElement, GetUseRenderMinArea());

	pElement = new MXmlElement("RENDERMINAREA");
	element.LinkEndChild(pElement);
	_SetContents(pElement, GetRenderMinArea());
}

void RSpotLightSceneNode::OnUpdate()
{
	RPFC_THISFUNC;

	m_LightCamera.SetTransform(GetWorldTransform());

	//Actor에 붙어 애니메이션될 경우 부모의 스케일값을 적용하기위해
	//Light의 범위를 스케일
	RVector3 vScale = GetScaleFromWorldTransform();
	float fScale = max(max(vScale.x, vScale.y), vScale.z);
	float fFar = m_fFar*fScale;

	RMatrix matView, matProj;
	matView.SetLookAtMatrixLH(m_LightCamera.GetPosition(), m_LightCamera.GetPosition() + m_LightCamera.GetDirection(), m_LightCamera.GetUp());
	matProj.SetProjectionMatrixFovLH(m_fFov *  (MMath::PI/180.0f), 1, 10.0f, fFar);

	m_LightCamera.SetUserView(matView);
	m_LightCamera.SetUserProjection(matProj);

	m_LightCamera.UpdateViewFrustrum();
}

void RSpotLightSceneNode::UpdateAABB()
{
	RPFC_THISFUNC;
	RSphere sphere = GetBoundingSphere();

	float fRadius = sphere.radius;
	RVector3 fCenter = sphere.center;

	RBoundingBox aabb;
	aabb.minx = fCenter.x - fRadius;
	aabb.maxx = fCenter.x + fRadius;
	aabb.miny = fCenter.y - fRadius;
	aabb.maxy = fCenter.y + fRadius;
	aabb.minz = fCenter.z - fRadius;
	aabb.maxz = fCenter.z + fRadius;
	SetAABB( aabb );
}

void RSpotLightSceneNode::OnUpdateAABB()
{
	UpdateAABB();
}

RSceneNode* RSpotLightSceneNode::Clone()
{
	RSpotLightSceneNode* pSpotLight = new RSpotLightSceneNode;

	// common
	pSpotLight->m_strNodeName = m_strNodeName;
	pSpotLight->SetTransform(GetLocalTransform());
	pSpotLight->SetScale(GetScale());

	//Property
	pSpotLight->m_color = m_color;
	pSpotLight->m_fIntensity = m_fIntensity;
	pSpotLight->m_fApplyIntensity = m_fApplyIntensity;
	pSpotLight->m_fAttenuationStart = m_fAttenuationStart;
	pSpotLight->SetFar(m_fFar);
	pSpotLight->SetFov(m_fFov);
	pSpotLight->SetSpotLightTexture(m_strSpotLightTextureFileName.c_str());
	
	pSpotLight->SetShadowCast(m_bShadowCast);
	pSpotLight->SetUseRenderMinArea(m_pShadowStrategy->GetUseRenderMinArea());
	pSpotLight->SetRenderMinArea(m_pShadowStrategy->GetRenderMinArea());

	//Light가 Actor의 자식으로 있을 경우 Light도 Actor와 같이 애니메이션되어야 하므로
	//AutoDate 여부도 복사한다.
	pSpotLight->SetAutoUpdate(IsAutoUpdate());

	return pSpotLight;
}

}
