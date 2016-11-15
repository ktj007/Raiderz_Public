#include "StdAfx.h"
#include "RPSSMShadowStrategy.h"
#include "RDirectionalLightSceneNode.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"

namespace rs3 {

//#define SHADOWMAP_REUSE_TECHNIQUE

RPSSMShadowStrategy::RPSSMShadowStrategy(const RShadowDesc& desc)
: m_nCurrentIndex(0)
{
	_ASSERT( desc.eMethod == SHADOW_PSSM );
	m_shadowDesc = desc;

	m_nTextureSize = desc.nTextureSize;

	m_nTextureNum = desc.nTextureNum;
	if(m_nTextureNum > MAX_PSSM_SPLIT_COUNT)
		m_nTextureNum = MAX_PSSM_SPLIT_COUNT;

	m_fSplitDistance[0] = 0.f;
	for( int i = 0; i < MAX_PSSM_SPLIT_COUNT; ++i )
		m_fSplitDistance[i+1] = desc.fSplitDistance[i];

	m_bShadowReceiveEnable = true;
	m_fBorderSize = 0.f;

	for( int i = 0; i < MAX_PSSM_SPLIT_COUNT; ++i )
	{
		m_vSplitInfo[i].Set( 0.f, 0.f, 0.f, -1.f );
	}

	SetUseRenderMinArea(true);
}

RPSSMShadowStrategy::~RPSSMShadowStrategy()
{

}

const RCameraSceneNode* RPSSMShadowStrategy::GetCurrentDepthCamera()
{
	return m_camShadows[m_nCurrentIndex].m_camShadow;
}

RBoundingBox RPSSMShadowStrategy::GetCurrentLightFrustumAABB()
{
	return m_camShadows[m_nCurrentIndex].m_camFrustumAABB;
}

const RMatrix RPSSMShadowStrategy::GetCurrentLightTransform()
{
	return m_camShadows[m_nCurrentIndex].m_camShadow->GetViewMatrix()
		* m_camShadows[m_nCurrentIndex].m_camShadow->GetProjectionMatrix() * m_matTextureScale;
}

const RMatrix RPSSMShadowStrategy::GetCurrentLightViewTransform()
{
	return m_camShadows[m_nCurrentIndex].m_camShadow->GetViewMatrix();
}

void RPSSMShadowStrategy::SetCurrentCameraIndex(int nIndex)
{
	if ( nIndex < 0 )
		return;
	if ( nIndex >= (int)m_camShadows.size())
		return;

	m_nCurrentIndex = nIndex;
}

float RPSSMShadowStrategy::GetCurrentViewNearZ(RCameraSceneNode *pViewCamera)
{
	float fViewCameraDistance = pViewCamera->GetFarZ() - pViewCamera->GetNearZ();
	return pViewCamera->GetNearZ() + fViewCameraDistance*m_camShadows[m_nCurrentIndex].nZ;
}

float RPSSMShadowStrategy::GetCurrentViewFarZ(RCameraSceneNode *pViewCamera)
{
	float fViewCameraDistance = pViewCamera->GetFarZ() - pViewCamera->GetNearZ();
	return pViewCamera->GetNearZ() + fViewCameraDistance*m_camShadows[m_nCurrentIndex].fZ;
}

void RPSSMShadowStrategy::SetCurrentViewNearZClip(float nz, float fz)
{
	SCameraSet &camSet = m_camShadows[m_nCurrentIndex];
	camSet.nZ = nz;
	camSet.fZ = fz;
}

void RPSSMShadowStrategy::CreateBuffers(int nSize)
{
	RFORMAT eShadowRTFormat = RFMT_R16F;
	if(!REngine::GetDevice().QueryFeature(RQF_R16F))
		eShadowRTFormat = RFMT_R32F;

	m_pRenderTarget = REngine::GetDevice().CreateRenderTargetTexture(nSize, nSize*m_nTextureNum, RFMT_G16R16F);
	m_pCommonShadowRT = REngine::GetDevice().CreateRenderTargetTexture(nSize, nSize*m_nTextureNum, eShadowRTFormat);
	m_pDoubleShadowRT = REngine::GetDevice().CreateRenderTargetTexture(nSize, nSize*m_nTextureNum, eShadowRTFormat);
	
#ifdef SHADOWMAP_REUSE_TECHNIQUE
	m_pReusedRT = new RTexture*[m_nTextureNum*2];
	for(int i=0; i<m_nTextureNum*2; ++i)
		m_pReusedRT[i] = REngine::GetDevice().CreateRenderTargetTexture(nSize, nSize, RFMT_R32F);
#endif

	m_pDepthBuffer = REngine::GetDevice().CreateRenderTargetTexture(nSize, nSize*m_nTextureNum, RFMT_D16);
	
	for ( int si = 0; si<m_nTextureNum; ++si )
	{
		SCameraSet CameraSet;

		float nZ = m_fSplitDistance[si];
		float fZ = m_fSplitDistance[si+1];

		CameraSet.nZ = nZ;
		CameraSet.fZ = fZ;

		CameraSet.nPrevRT = si*2;
		CameraSet.nCurrentRT = si*2+1;

		CameraSet.m_camFrustumAABB.Initialize();

		CameraSet.m_camShadow = new RCameraSceneNode;
		RCameraSceneNode* cam = CameraSet.m_camShadow;

		cam->SetViewPort(0, nSize*si, nSize, nSize);
		cam->SetActive(true);

		cam->SetPassIgnore((RRENDERPASS)0);
		cam->SetPassIgnore(RRP_SPEEDTREE, false);
		cam->SetPassIgnore(RRP_STATICMESH, false);
		cam->SetPassIgnore( RRP_TERRAIN, false);
		cam->SetPassIgnore( RRP_ACTOR, false);

		cam->GetViewFrustumCullingMgr()->SetRenderingList<RShadowRenderingList>();

		m_camShadows.push_back(CameraSet);
	}
}

void RPSSMShadowStrategy::DestroyBuffers()
{
	for ( size_t si = 0; si<m_camShadows.size(); ++si )
	{
		m_camShadows[si].m_camShadow->Destroy();

		delete m_camShadows[si].m_camShadow;
	}

	REngine::GetDevice().DeleteTexture(m_pRenderTarget);
	REngine::GetDevice().DeleteTexture(m_pCommonShadowRT);
	REngine::GetDevice().DeleteTexture(m_pDoubleShadowRT);
	REngine::GetDevice().DeleteTexture(m_pDepthBuffer);
	m_pRenderTarget = m_pCommonShadowRT = m_pDoubleShadowRT = m_pDepthBuffer = NULL;

#ifdef SHADOWMAP_REUSE_TECHNIQUE
	for(int i=0; i<m_nTextureNum*2; ++i)
		REngine::GetDevice().DeleteTexture(m_pReusedRT[i]);
	delete[] m_pReusedRT;
#endif
}

void RPSSMShadowStrategy::Init()
{
	// 필요한 값들을 초기화
	MakeTextureScaleTransform();

	// 그림자 카메라 세팅
	CreateBuffers(m_nTextureSize);
}

void RPSSMShadowStrategy::Destroy()
{
	DestroyBuffers();
	m_camShadows.clear();
}

void RPSSMShadowStrategy::Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld)
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

	RVector3 vLightDirection = pDirectionalLight->GetDirection();
	if( m_pDirectionalLight!=pDirectionalLight || m_vLightDirection!=vLightDirection)
	{
		SetResetDepthMap(true);
		m_pDirectionalLight =pDirectionalLight;
		m_vLightDirection =vLightDirection;
	}

	static RStableShadowMap shadowMap[4] = {RStableShadowMap(m_nTextureSize),RStableShadowMap(m_nTextureSize),RStableShadowMap(m_nTextureSize),RStableShadowMap(m_nTextureSize)};

	//Radius값이 음수이면 그 Split는 사용되지 않음.
	//쉐이더에서 Radius값을 분모로 사용하기 때문에 0이면 안됨.
	m_vSplitInfo[0].a = m_vSplitInfo[1].a = m_vSplitInfo[2].a = m_vSplitInfo[3].a = -1;

	for ( size_t si = 0; si<m_camShadows.size(); ++si )
	{
		SCameraSet &cameraSet = m_camShadows[si];

		float fNearZ = pViewCamera->GetNearZ() + cameraSet.nZ;
		float fFarZ = pViewCamera->GetNearZ() + cameraSet.fZ;

		
#ifdef SHADOWMAP_REUSE_TECHNIQUE
		swap(cameraSet.nPrevRT, cameraSet.nCurrentRT);
		SetRenderTargetIndex(cameraSet.nPrevRT, cameraSet.nCurrentRT);

		if(si<REUSE_MIN_INDEX)	//기존 방식
			RenderShadowMap(pViewCamera, pWorld, *cameraSet.m_camShadow, &shadowMap[si], fNearZ, fFarZ, pDirectionalLight->GetDirection());
		else	//DepthMap 재사용 방식
			RenderReusedShadowMap(pViewCamera, pWorld, cameraSet.m_camShadow, &shadowMap[si], fNearZ, fFarZ);
#else
		RenderShadowMap(pViewCamera, pWorld, *cameraSet.m_camShadow, &shadowMap[si], fNearZ, fFarZ, pDirectionalLight->GetDirection());
#endif

		RSphere BoundingSphere = shadowMap[si].GetBoundingSphere();
		RVector3 vCameraToSphereCenter = BoundingSphere.center - pViewCamera->GetWorldPosition();

		m_vSplitInfo[si] = RVector4(0, 0, -vCameraToSphereCenter.Length(), BoundingSphere.radius);
		m_vDepthBias[si] = (BoundingSphere.radius*2) / GetCurrentDepthMapSize();
	}

	if(m_shadowDesc.bDoubleShadow)
		CombineDoubleShadowMap();

#ifdef SHADOWMAP_REUSE_TECHNIQUE
	if(m_nTextureNum>REUSE_MIN_INDEX)
		CombineDepthMap(m_nTextureNum);
#endif

	SetResetDepthMap(false);
}

}