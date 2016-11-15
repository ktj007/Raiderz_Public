#include "StdAfx.h"
#include "RInferredRenderingStrategy.h"
#include "REngine.h"
#include "RShadowStrategy.h"
#include "RRenderHelper.h"
#include "RSSAOProcessor.h"
#include "RRenderingCallbackInterface.h"
#include "RSpotLightSceneNode.h"
#include "RShaderComposite.h"
#include "RShaderCompositeManager.h"
#include "RDirectionalLightSceneNode.h"
#include "RShaderFX.h"
#include "RPostEffectDistortion.h"
#include "RLightingShader.h"

namespace rs3 {


RInferredRenderingStrategy::RInferredRenderingStrategy() : RRenderingStrategy(),
m_pDepthBufferGPass( NULL),
m_pDepthBufferMPass( NULL),
m_pGBufferDepth( NULL),
m_pGBufferNormal( NULL),
m_pGBufferSpecRimAO( NULL),
m_pGBufferDSF( NULL),
m_pLBuffer(NULL),
m_bUseDSF(false),
m_fQualityFactor(1)
{
	m_bSoftParticle = true;

	// m_PassIgnored[STAGE_COUNT][RRP_MAX]
	// 패스 무시 속성을 카메라의 것을 그대로 사용 하고 패스 랜더러에서 무시 하던가 하는 쪽으로,, 
	m_eMethod = RENDERING_INFERRED;


	REngine::GetSceneManager().GetShaderCompositeManager()->SetDSF( m_bUseDSF);
}

RInferredRenderingStrategy::~RInferredRenderingStrategy(void)
{
	Destroy();
	SAFE_DELETE( m_pGIProcessor);
}

void RInferredRenderingStrategy::RenderScene( RCameraSceneNode *pCurrentCamera, RWorldSceneNode *pWorldScene, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface)
{
	RPFC_THISFUNC;
	
	REngine::GetDevice().SetTextureMipmapLodBias(0, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);
	REngine::GetDevice().SetTextureMipmapLodBias(1, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);

	RDevice* pDevice = REngine::GetDevicePtr();

	//Directional Light ShadowMap 생성
	if( !pCurrentCamera->GetPassIgnore( RRP_SHADOWMAP ) )
		pShadowStrategy->Render( pCurrentCamera, pWorldScene );


	//SpotLight ShadowMap 생성
	RCullingMgr* pCullingMgr = pCurrentCamera->GetViewFrustumCullingMgr();
	pCullingMgr->Cull(pWorldScene,pCurrentCamera->GetViewFrustum().GetPlanes(),6);

	RenderSpotLightShadowMap(pCurrentCamera, pCullingMgr->GetCulledList(RRP_SPOTLIGHT));

	//SpotLight가 Light camera에 따라 pCullingMgr->CulledListUpdate()를 호출해주므로
	//SpotLight의 ShadowMap을 그린 다음에 View-camera에 대해 호출해준다.
	pCullingMgr->CulledListUpdate(pCurrentCamera);


	//장면 렌더링
	//pShadowStrategy->RenderScene( pCurrentCamera );
	// 포워드 랜더링은 그림자 전략에서 주를 이루어 관리 하지만
	// 인퍼드 랜더링에서는 그림자 전략은 데이터를 받아오는 용도정도이며 랜더링 전략에서 주를 이룬다.

	pCullingMgr->CulledListUpdate(pCurrentCamera);

	// LOD 계산
	CalculateLOD( pCurrentCamera, pCurrentCamera);

	//OnPreRender
	DoPreRender(pCurrentCamera);

	// 일반적인 준비 패스
	pCurrentCamera->BeginRender( 0, false, this);
	pCurrentCamera->OnClearBuffer();

	RTexture* hBeforeTarget0 = pDevice->GetRenderTarget(0);
	RTexture* hBeforeTarget1 = pDevice->GetRenderTarget(1);
	RTexture* hBeforeTarget2 = pDevice->GetRenderTarget(2);
	RTexture* hBeforeTarget3 = pDevice->GetRenderTarget(3);
	//RTexture* hBeforeDepth = pDevice->GetDepthStencilBuffer();

	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시, 전 패스에 영향을 미치는 디바이스 셋팅(FillMode 등)
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->DeviceSetting();


	// G - stage
	RPFC_B("GeometryPass");
	
	pDevice->SetRenderTarget(0, m_pGBufferNormal);
	pDevice->SetRenderTarget(1, m_pGBufferDepth);
	pDevice->SetRenderTarget(2, m_pGBufferSpecRimAO);
	pDevice->SetRenderTarget(3, m_bUseDSF ? m_pGBufferDSF : NULL);
	//pDevice->SetDepthStencilBuffer(m_pDepthBufferGPass);
	pDevice->Clear( true, true, false, 0xffffffff);


	if(pShadowStrategy->GetShadowDesc().eMethod==SHADOW_PROJECTION)
		RenderGStagePassForProjectionShadow( pCurrentCamera, RRP_START_PASS_OF_GSTAGE, RRP_END_PASS_OF_GSTAGE);
	else
		RenderGStagePass( pCurrentCamera, RRP_START_PASS_OF_GSTAGE, RRP_END_PASS_OF_GSTAGE);


	RPFC_E;

	// L - stage
	RPFC_B("LightPass");

	pDevice->SetRenderTarget(0, m_pLBuffer);
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetRenderTarget(2, NULL);
	pDevice->SetRenderTarget(3, NULL);
	pDevice->Clear( true, false);

	if( IsUsetLighting())
	{
		RenderLStagePass(pCurrentCamera,m_pGIProcessor,pCullingMgr, GetLightBuffer()->GetInfo());
	}
	else
	{
		REngine::GetDevice().ColorFill( m_pLBuffer, 0x00ffffff);
	}
	RPFC_E;

	// M - stage
	RPFC_B("MatrialPass");
	
	pDevice->SetRenderTarget(0, hBeforeTarget0);
	pDevice->SetRenderTarget(1, hBeforeTarget1);
	pDevice->SetRenderTarget(2, hBeforeTarget2);
	pDevice->SetRenderTarget(3, hBeforeTarget3);

	// 후처리를 위핸 준비
	bool bActivePostEffect = m_pPostEffectManager && m_pPostEffectManager->IsBeingEnableInstance();
	if( bActivePostEffect )
	{
		m_pPostEffectManager->PreRender(pCurrentCamera, m_dwWidth != m_dwWidthSmall ); // G버퍼와 M버퍼의 크기가 다를 경우 뎁스 클리어
	}

	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시
	if( pRenderingCallbackInterface)
	{
		pRenderingCallbackInterface->PreOrderRender();
	}


	// 뎁스 버퍼를 같은 것으로 계속 사용???
	//pDevice->SetDepthStencilBuffer(m_pDepthBufferMPass);
	//pDevice->Clear( true, true, false, pCurrentCamera->GetClearColor());
	//pDevice->Clear( true, false, pCurrentCamera->isHaveStencil(), pCurrentCamera->GetClearColor(), pCurrentCamera->GetClearDepth(), pCurrentCamera->GetClearStencil() );

	RenderMStagePass( pCurrentCamera, RRP_START_PASS_OF_MSTAGE, RRP_END_PASS_OF_MSTAGE, m_pGBufferNormal, m_pGBufferDepth, m_pLBuffer, m_pGBufferDSF);


	//OnPostRender
	DoPostRender(pCurrentCamera);


	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->PostOrderRender();


  	//float fSize = 180;
  	//float fGap = fSize + 10;
  	//RRenderHelper::RenderTexture( 0, fGap*0,	fSize, fSize, m_pGBufferNormal);
  	//RRenderHelper::RenderTexture( 0, fGap*1,	fSize, fSize, m_pGBufferDepth);
  	//RRenderHelper::RenderTexture( 0, fGap*2,	fSize, fSize, m_pGBufferSpecRimAO);
  	//RRenderHelper::RenderTexture( 0, 0,	m_dwWidth/2, m_dwHeight, m_pGBufferNormal, 1.0f/2.0f, 1);
// 	RRenderHelper::RenderTexture( 0, 0,	m_dwWidth/2, m_dwHeight, m_pLBuffer, 1.0f/2.0f, 1, RBLEND_NONE);

	if( bActivePostEffect )
	{
		// TODO: 이걸 RPostEffectManager에서 하고 싶다.
		// 후처리중 마스킹이 필요한 것 들을 위한.
		// RenderPost에서 타겟을 복원 하므로 굳이 마스크 처리 전에 타겟 백업을 해 두지 않아도 된다.
		RPostEffectEnumerator enumerator = m_pPostEffectManager->GetPostEffectEnumerator();
		while( enumerator.HasElement() )
		{
			RPostEffectInterface* pPostEffect = enumerator.Get();
			_ASSERT( pPostEffect );
			if( pPostEffect->IsActive() && pPostEffect->IsUsingMaskBuffer() )
			{
				// 마스킹 정보는 프레임 단위로 갱신 하는 것이 기본 전제이다. 마스크 버퍼를 비워준다.
				pPostEffect->ClearMaskBuffer();

				REngine::GetDevice().SetRenderTarget( 0, pPostEffect->GetMaskBuffer() );

				// 후처리 효과 종류에 따른 랜더 패스 얻기
				RRENDERPASS ePass = pPostEffect->GetMaskingRenderPass();
				if( ePass != RRP_NONE )
				{
					// 마스크 렌더 스테이지 따로 필요
					RenderKStagePass( pCurrentCamera, ePass, m_pGBufferNormal, m_pGBufferDepth, m_pLBuffer, m_pGBufferDSF);
				}
			}

			enumerator.Next();
		}

		// 후처리 효과 최종 처리
		m_pPostEffectManager->RenderPost( pCurrentCamera, m_pGBufferNormal, m_pGBufferDepth);
	}

	// ResetRender State
	//pDevice->SetDepthStencilBuffer( hBeforeDepth);
	REngine::GetDevice().SetTextureMipmapLodBias( 0, 0);
	REngine::GetDevice().SetTextureMipmapLodBias( 1, 0);

	pCurrentCamera->EndRender( false);
	pCurrentCamera->OnEndRender();

	RPFC_E;
}

void RInferredRenderingStrategy::Init( int nW, int nH)
{
	RRenderingStrategy::Init( nW, nH);

	m_dwWidthSmall = (DWORD)((float)m_dwWidth * m_fQualityFactor);
	m_dwHeightSmall = (DWORD)((float)m_dwHeight * m_fQualityFactor);

	if( m_pGIProcessor)
		m_pGIProcessor->Init( m_dwWidthSmall, m_dwHeightSmall);

	// CreateRenderTarget...
	if(!m_pDepthBufferGPass)
		m_pDepthBufferGPass		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthSmall, m_dwHeightSmall, RFMT_D16);
	if(!m_pDepthBufferMPass)
		m_pDepthBufferMPass		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_D16);

	if(!m_pGBufferDepth)
		m_pGBufferDepth			= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthSmall, m_dwHeightSmall, RFMT_R32F);
	if(!m_pGBufferNormal)
		m_pGBufferNormal		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthSmall, m_dwHeightSmall, RFMT_G16R16F);
	if(!m_pGBufferSpecRimAO)
		m_pGBufferSpecRimAO		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthSmall, m_dwHeightSmall, RFMT_A8R8G8B8);
	if(!m_pGBufferDSF)
		m_pGBufferDSF			= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthSmall, m_dwHeightSmall, RFMT_G16R16F);

	if(!m_pLBuffer)
		m_pLBuffer				= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthSmall, m_dwHeightSmall, RFMT_A16B16G16R16F);
	// 포워드와 제대로 된 속도 비교를 위해 64bit이 아닌 32bit짜리로..
	//m_pLBuffer			= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_A8R8G8B8);

	if( !m_pDepthBufferGPass || !m_pDepthBufferMPass || !m_pGBufferDepth || !m_pGBufferNormal || !m_pGBufferSpecRimAO || !m_pLBuffer || !m_pGBufferDSF)
	{
		mlog( "RInferredRenderingStrategy::Init fail\n");
		_ASSERT( "RInferredRenderingStrategy::Init fail");
	}
	else
	{
		REngine::GetDevice().ColorFill( m_pGBufferDepth);
		REngine::GetDevice().ColorFill( m_pGBufferNormal);
		REngine::GetDevice().ColorFill( m_pGBufferSpecRimAO);
		REngine::GetDevice().ColorFill( m_pLBuffer, 0x00ffff00);
	}

}


void RInferredRenderingStrategy::Destroy()
{
	RRenderingStrategy::Destroy();

	if( m_pGIProcessor)
		m_pGIProcessor->Destroy();

	REngine::GetDevicePtr()->DeleteTexture( m_pDepthBufferGPass);
	m_pDepthBufferGPass = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pDepthBufferMPass);
	m_pDepthBufferMPass = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pGBufferDepth);
	m_pGBufferDepth = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pGBufferNormal);
	m_pGBufferNormal = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pGBufferSpecRimAO);
	m_pGBufferSpecRimAO = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pGBufferDSF);
	m_pGBufferDSF = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pLBuffer);
	m_pLBuffer = NULL;
}

void RInferredRenderingStrategy::SetStateGStagePass()
{
	/// DSF ID는 우선 0으로 채움
	RShaderCompositeManager::m_cDSFID.SetFloat( 0);
}

void RInferredRenderingStrategy::SetStateMStagePass( RCameraSceneNode* pCurrentCamera,RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDSFMap)
{
	RPFC_THISFUNC;

	RRenderingStrategy::SetStateMStagePass(pCurrentCamera, pViewNormalMap, pViewDepthMap, pViewLightMap, pViewDSFMap);


	/// DSF ID는 우선 0으로 채움
	RShaderCompositeManager::m_cDSFID.SetFloat( 0);

	// 뷰 스페이스 DSF 버퍼
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_DSFID, pViewDSFMap);
	REngine::GetDevice().SetTextureAddressClamp(AS_VIEWSPACE_DSFID);
	REngine::GetDevice().SetTextureFilter(AS_VIEWSPACE_DSFID, RTF_POINT);
}

void RInferredRenderingStrategy::ResetStateMStagePass(void)
{
	RRenderingStrategy::ResetStateMStagePass();

	REngine::GetDevice().SetTexture( AS_VIEWSPACE_DSFID, NULL);
}

void RInferredRenderingStrategy::SetLStageTexture()
{
	REngine::GetDevice().SetTexture(AS_GNORMAL, GetNormalBuffer());
	REngine::GetDevice().SetTexture(AS_GDEPTH, GetDepthBuffer());
	REngine::GetDevice().SetTextureFilter(AS_GNORMAL, RTF_POINT);
	REngine::GetDevice().SetTextureFilter(AS_GDEPTH, RTF_POINT);
}

void RInferredRenderingStrategy::RestoreLStageState()
{
	REngine::GetDevice().SetTexture(AS_GNORMAL, NULL);
	REngine::GetDevice().SetTexture(AS_GDEPTH, NULL);
}

int RInferredRenderingStrategy::GetMRTCount()
{
	if(GetRenderingMode()==RRM_DIFFUSE_ONLY)
		return 1;
	else
		return 2;
}

}