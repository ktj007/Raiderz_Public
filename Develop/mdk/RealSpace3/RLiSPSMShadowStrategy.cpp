#include "StdAfx.h"
#include "RLiSPSMShadowStrategy.h"
#include "RRenderHelper.h"
#include "RDirectionalLightSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RRenderingCallbackInterface.h"
#include "RShaderFX.h"

namespace rs3 {

//#define	_RGB_DEBUG_DEPTHBUFFER	// 그림자 맵에 rgb 출력, 테스트용

//////////////////////////////////////////////////////////////////////////
//
RLispsmShadowStrategy::RLispsmShadowStrategy(const RShadowDesc& desc)
{
	_ASSERT( desc.eMethod == SHADOW_LISPSM || desc.eMethod == SHADOW_ESM );
	m_shadowDesc = desc;

	m_bShadowReceiveEnable = true;
	m_pShadowCamera = NULL;
	m_nTextureSize = desc.nTextureSize;
	m_fBorderSize = 1.f;

	MakeTextureScaleTransform();
}

RLispsmShadowStrategy::~RLispsmShadowStrategy()
{
	Destroy();
}

void RLispsmShadowStrategy::Init()
{
	_ASSERT(m_pShadowCamera==NULL);

	m_pShadowCamera = new RCameraSceneNode();
	m_pShadowCamera->GetViewFrustumCullingMgr()->SetRenderingList<RShadowRenderingList>();

	RFORMAT eShadowRTFormat = RFMT_R16F;
	if(!REngine::GetDevice().QueryFeature(RQF_R16F))
		eShadowRTFormat = RFMT_R32F;

	m_pRenderTarget = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, RFMT_G16R16F);
	m_pCommonShadowRT = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, eShadowRTFormat);
	m_pDoubleShadowRT = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, eShadowRTFormat);
	m_pDepthBuffer = REngine::GetDevice().CreateRenderTargetTexture(m_nTextureSize, m_nTextureSize, RFMT_D16);

	_ASSERT(m_pRenderTarget && m_pCommonShadowRT && m_pDoubleShadowRT && m_pDepthBuffer);

	//필요한 것만 활성화
	m_pShadowCamera->SetPassIgnore((RRENDERPASS)0);
	m_pShadowCamera->SetPassIgnore(RRP_SPEEDTREE, false);
	m_pShadowCamera->SetPassIgnore(RRP_ACTOR, false);

	m_pShadowCamera->SetActive(true);

	m_cameraFrustumAABB.Initialize();
	return;
}

void RLispsmShadowStrategy::Destroy()
{
	if ( m_pShadowCamera )
	{
		m_pShadowCamera->DestroyBuffers();
		m_pShadowCamera->Destroy();
		delete m_pShadowCamera;
		m_pShadowCamera = NULL;
	}

	REngine::GetDevice().DeleteTexture(m_pRenderTarget);
	REngine::GetDevice().DeleteTexture(m_pCommonShadowRT);
	REngine::GetDevice().DeleteTexture(m_pDoubleShadowRT);
	REngine::GetDevice().DeleteTexture(m_pDepthBuffer);
	m_pRenderTarget = m_pCommonShadowRT = m_pDoubleShadowRT = m_pDepthBuffer = NULL;
}

void RLispsmShadowStrategy::Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld)
{
	RPFC_THISFUNC;
	_ASSERT(pWorld);
	_ASSERT(pViewCamera);

	// 그려질 필요가 없는 경우 검사
	RDirectionalLightSceneNode *pDirectionalLight = pWorld->GetDirectionalLight();
	if (NULL == pDirectionalLight)
		return;
	if ( !pViewCamera->GetActive())
		return;

	RVector vLightDirection = pDirectionalLight->GetDirection();
	float fFarZ = pViewCamera->GetNearZ() + m_shadowDesc.fSplitDistance[0];

	RStableShadowMap shadowMapProjection(m_nTextureSize);

	// COMMENT OZ - 말 그대로 셰도우맵 랜더
	RenderShadowMap(pViewCamera, pWorld, *m_pShadowCamera, &shadowMapProjection, pViewCamera->GetNearZ(), fFarZ, vLightDirection);

	RSphere BoundingSphere = shadowMapProjection.GetBoundingSphere();
	m_vDepthBias[0] = (BoundingSphere.radius*2) / GetCurrentDepthMapSize();


	if(m_shadowDesc.bDoubleShadow)
		CombineDoubleShadowMap();
}



const RMatrix RLispsmShadowStrategy::GetCurrentLightTransform()
{
	if ( m_pShadowCamera )
	{
		return ( m_pShadowCamera->GetViewMatrix() 
			* m_pShadowCamera->GetProjectionMatrix() * m_matTextureScale );
	}

	return RMatrix::IDENTITY;
}

const RMatrix RLispsmShadowStrategy::GetCurrentLightViewTransform()
{
	if ( m_pShadowCamera )
		return ( m_pShadowCamera->GetViewMatrix() );

	return RMatrix::IDENTITY;
}

const RCameraSceneNode* RLispsmShadowStrategy::GetCurrentDepthCamera()
{
	return m_pShadowCamera;
}

RBoundingBox RLispsmShadowStrategy::GetCurrentLightFrustumAABB()
{
	return m_cameraFrustumAABB;
}

const RMatrix RLispsmShadowStrategy::GetCurrentLightTransform(RCameraSceneNode& LightCamera)
{
	return LightCamera.GetViewMatrix() * m_matToler *LightCamera.GetProjectionMatrix() * m_matTextureScale;
}

//////////////////////////////////////////////////////////////////////////
//RExponentialShadowStrategy
RExponentialShadowStrategy::RExponentialShadowStrategy(const RShadowDesc& desc)
: RLispsmShadowStrategy(desc)
{
//	m_bUniform = true;
	m_matToler.MakeIdentity();
//	m_bNeedHWFiltering = true;
//	m_fBorderSize = 0.f;
}

void RExponentialShadowStrategy::Init()
{
	RLispsmShadowStrategy::Init();
	m_pShadowCamera->SetPassIgnore( RRP_STATICMESH, false);
	m_pShadowCamera->SetPassIgnore( RRP_TERRAIN, false);
}

}