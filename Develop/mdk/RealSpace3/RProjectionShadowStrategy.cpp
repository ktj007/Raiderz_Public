#include "StdAfx.h"
#include "RProjectionShadowStrategy.h"
#include "RDirectionalLightSceneNode.h"

#include "RShaderCompositeManager.h"
#include "RShaderComposite.h"
#include "RShaderFX.h"
#include "RTerrain.h"

#include "RLightingShader.h"
#include "RRenderHelper.h"


namespace rs3 {

class ProjectionShadowListener : public RListener
{
public:
	ProjectionShadowListener( RProjectionShadowStrategy* pProjectionShadowStrategy) : m_pProjectionShadowStrategy( pProjectionShadowStrategy) {}

	virtual void Update() override
	{
		m_pProjectionShadowStrategy->DestroyShadowMap();
		m_pProjectionShadowStrategy->CreateShadowMap();
	}
private:
	RProjectionShadowStrategy* m_pProjectionShadowStrategy;
};

RProjectionShadowStrategy::RProjectionShadowStrategy(const RShadowDesc& desc) : m_pShadowMap(NULL)
{
	_ASSERT( desc.eMethod == SHADOW_PROJECTION );
	
	m_shadowDesc = desc;
	m_nTextureSize = desc.nTextureSize;
	m_fBorderSize =1;
	m_bNeedHWFiltering = true;

	MakeTextureScaleTransform();

	// event 구독
	m_pEventListener = new ProjectionShadowListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );


	//Camera Setting
	{
		m_pLightCamera.GetViewFrustumCullingMgr()->SetRenderingList<RShadowRenderingList>();
		m_pLightCamera.SetPassIgnore((RRENDERPASS)0);
		m_pLightCamera.SetPassIgnore(RRP_SPEEDTREE, false);
		m_pLightCamera.SetPassIgnore(RRP_ACTOR, false);
		m_pLightCamera.SetPassIgnore(RRP_TERRAIN, false);
		m_pLightCamera.SetActive(true);
	}
}

RProjectionShadowStrategy::~RProjectionShadowStrategy(void)
{
	Destroy();

	// event 해제
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );
	SAFE_DELETE(m_pEventListener);
}

void RProjectionShadowStrategy::Init()
{
	//Create Buffer
	{
		m_pRenderTarget = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, RFMT_RGB16);
		m_pCommonShadowRT = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, RFMT_RGB16);
		m_pDoubleShadowRT = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, RFMT_RGB16);
		m_pDepthBuffer = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, RFMT_D16);
		_ASSERT(m_pRenderTarget && m_pCommonShadowRT && m_pDoubleShadowRT && m_pDepthBuffer);
		
		CreateShadowMap();	
	}	
}

void RProjectionShadowStrategy::Destroy()
{
	REngine::GetDevice().DeleteTexture(m_pRenderTarget);
	REngine::GetDevice().DeleteTexture(m_pCommonShadowRT);
	REngine::GetDevice().DeleteTexture(m_pDoubleShadowRT);
	REngine::GetDevice().DeleteTexture(m_pDepthBuffer);
	m_pRenderTarget = m_pCommonShadowRT = m_pDoubleShadowRT = m_pDepthBuffer = NULL;

	DestroyShadowMap();	
}

void RProjectionShadowStrategy::CreateShadowMap()
{
	int width = REngine::GetDevice().GetScreenWidth();
	int height = REngine::GetDevice().GetScreenHeight();
	m_pShadowMap = REngine::GetDevice().CreateRenderTargetTexture( width, height, RFMT_RGB16);	
	_ASSERT(m_pShadowMap);
}

void RProjectionShadowStrategy::DestroyShadowMap()
{
	REngine::GetDevice().DeleteTexture(m_pShadowMap);
	m_pShadowMap =NULL;
}

void RProjectionShadowStrategy::Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld)
{
	if ( !pViewCamera->GetActive())
		return;

	RDirectionalLightSceneNode *pDirectionalLight = pWorld->GetDirectionalLight();
	if (NULL == pDirectionalLight)
		return;

	RVector vLightDirection = pDirectionalLight->GetDirection();
	float fFarZ = pViewCamera->GetNearZ() + m_shadowDesc.fSplitDistance[0];

	RStableShadowMap shadowMapProjection(m_nTextureSize);

	RenderShadowMap(pViewCamera, pWorld, m_pLightCamera, &shadowMapProjection, pViewCamera->GetNearZ(), fFarZ, vLightDirection);

	if(m_shadowDesc.bDoubleShadow)
		CombineDoubleShadowMap();
}

void RProjectionShadowStrategy::RenderShadowBuffer(RCameraSceneNode* pCamera, RLightingShader* pLightingShader)
{
	RPFC_THISFUNC;
	//Set Render State
	{
		REngine::GetDevice().SetDepthEnable(false, false);	

		REngine::GetDevice().SetStencilEnable(true);
		REngine::GetDevice().SetStencilOp(RSTENCILOP_KEEP,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
		REngine::GetDevice().SetStencilFunc(RCMP_NOTEQUAL);
// 		REngine::GetDevice().SetStencilRef(0);
// 		REngine::GetDevice().SetStencilMask(0xFFFFFFFF);
	}
	

	//Set Texture
	{
		REngine::GetDevice().SetTexture( AS_DEPTHMAP, m_pRenderTarget);
		REngine::GetDevice().SetTextureAddressClamp(AS_DEPTHMAP);
		REngine::GetDevice().SetTextureFilter(AS_DEPTHMAP, RTF_BILINEAR);
	}


	//Set Shader constant
	{
		RMatrix matViewInv;
		pCamera->GetViewMatrix().GetInverse(&matViewInv);
		pLightingShader->m_scLightTransform.SetMatrix(matViewInv*GetCurrentLightTransform());
	}


	//Render screen-quad to stencil-masked area
	{
		RTechnique tech = pLightingShader->CreateShader(RLightingShader::DLF_PROJECTION_SHADOW)->GetTechnique("ShadowReceiverTechnique");

		REngine::GetDevice().ColorFill(m_pShadowMap, 0xFFFFFFFF);
		RTexture* pRT0 = REngine::GetDevice().SetRenderTarget(0, m_pShadowMap);
		{
			tech.Begin(0, TECH_PRESERVE_NOT);
			tech.BeginPass();
			RRenderHelper::RenderQuad(0,0,m_pShadowMap->GetInfo().nTextureWidth,m_pShadowMap->GetInfo().nTextureHeight);
			tech.EndPass();
			tech.End();		
		}
		REngine::GetDevice().SetRenderTarget(0, pRT0);		
	}

	//Restore Render State
	REngine::GetDevice().SetStencilEnable(false);
	REngine::GetDevice().SetStencilFunc(RCMP_ALWAYS);
	REngine::GetDevice().SetDepthEnable(true, true);
}


const RMatrix RProjectionShadowStrategy::GetCurrentLightTransform()
{
	return ( m_pLightCamera.GetViewMatrix() * m_pLightCamera.GetProjectionMatrix() * m_matTextureScale );
}

const RMatrix RProjectionShadowStrategy::GetCurrentLightTransform(RCameraSceneNode& LightCamera)
{
	return LightCamera.GetViewMatrix() * LightCamera.GetProjectionMatrix() * m_matTextureScale;
}

RSCID RProjectionShadowStrategy::GetDepthWriteComponent()
{
	return RSC_TRANSFORM_CONSTANT | RSC_SHADOW_WRITE;
}

void RProjectionShadowStrategy::SetandClearRenderTarget(RCameraSceneNode* pCamera, RTexture* pRenderTarget)
{
	REngine::GetDevice().SetRenderTarget(0, pRenderTarget);
	REngine::GetDevice().Clear(true, true, false, 0xFFFFFFFF);
	REngine::GetDevice().SetViewport( m_fBorderSize, m_fBorderSize, m_nTextureSize-2.f*m_fBorderSize, m_nTextureSize-2.f*m_fBorderSize);
}

}