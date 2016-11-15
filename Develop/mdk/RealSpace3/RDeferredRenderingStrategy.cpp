#include "StdAfx.h"
#include "RDeferredRenderingStrategy.h"
#include "REngine.h"
#include "RRenderHelper.h"
#include "RSSAOProcessor.h"
#include "RRenderingCallbackInterface.h"
#include "RSpotLightSceneNode.h"
#include "RShaderComposite.h"
#include "RShaderCompositeManager.h"
#include "RDirectionalLightSceneNode.h"
#include "RShaderFX.h"
#include "RPostEffectDistortion.h"
#include "RUtil.h"
#include "RLightingShader.h"
#include "ROccluderSceneNode.h"
#include "RWaterPassRenderer.h"
#include "RPostEffectFinalScene.h"


namespace rs3 {

const RDeferredRenderingStrategy::FLAG_TABLE RDeferredRenderingStrategy::flagTable[] =
{
	{ DRF_GAMMA_CORRECT, "USE_GAMMA_CORRECT" },
};

RDeferredRenderingStrategy::RDeferredRenderingStrategy() : RRenderingStrategy(),
	m_pGBufferDepthSpecRim( NULL),
	m_pGBufferNormal( NULL),
	m_pGBufferDiffuseAO( NULL),
	m_pLBuffer(NULL)
{
	m_bSoftParticle = true;

	// m_PassIgnored[STAGE_COUNT][RRP_MAX]
	// 패스 무시 속성을 카메라의 것을 그대로 사용 하고 패스 랜더러에서 무시 하던가 하는 쪽으로,, 
	m_eMethod = RENDERING_DEFERRED;


	REngine::GetSceneManager().GetShaderCompositeManager()->SetDSF( false);


#ifdef STREOPIC_TEST
	// 3D 테스트
	//////////////////////////////////////////////
	m_pBufferLeft = m_pBufferRight =NULL;
	//////////////////////////////////////////////
#endif
}

RDeferredRenderingStrategy::~RDeferredRenderingStrategy(void)
{
	Destroy();
	SAFE_DELETE( m_pGIProcessor);
}


#ifdef STREOPIC_TEST
// 라이트 버퍼와 디퓨즈 버퍼를 합성 
void RDeferredRenderingStrategy::RenderBlend()
{
	RPFC_THISFUNC;

	//----------------------------------------------------------------------------------------------------
	//Set Render State
	REngine::GetDevice().SetFogEnable(false);
	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetFillMode(RFILL_SOLID);
	REngine::GetDevice().SetTexture( 1, m_pBufferLeft);
	REngine::GetDevice().SetTexture( 2, m_pBufferRight);
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( 2, RTF_POINT);
	//----------------------------------------------------------------------------------------------------	


	UINT uiPass, uiPassCount;
	RTechnique* pTech = &m_TechniqueBlend;
	pTech->Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for( uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		pTech->BeginPass(uiPass);
		RRenderHelper::RenderQuad(0, 0, (float)m_dwWidth, (float)m_dwHeight);
		pTech->EndPass();
	}
	pTech->End();
}
#endif


void RDeferredRenderingStrategy::RenderScene( RCameraSceneNode *pCurrentCamera, RWorldSceneNode *pWorldScene, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface)
{

#ifdef STREOPIC_TEST

	/// 3D test
	//////////////////////////////////////////////////////////////////////////////////////////
	RDevice* pDevice = REngine::GetDevicePtr();
	RTexture* hBeforeTarget0 = pDevice->GetRenderTarget(0);

	RVector vRight = pCurrentCamera->GetRight();
	RVector vCameraPosOrg = pCurrentCamera->GetPosition();
	RVector vCameraDirOrg = pCurrentCamera->GetDirection();
	RVector vCameraPos;
	RVector vCameraDir;
	RMatrix mat;

	float fMoveFactor = 7.0f;
	float fRotFactor = 0.002f;

	// 좌안 ----------------------------------------------------------------------
	// Move
	vCameraPos = vCameraPosOrg + -vRight * fMoveFactor;
	// Rotation
	mat.MakeIdentity();
	mat.SetRotationZ( fRotFactor);
	vCameraDir =  vCameraDirOrg * mat;
	pCurrentCamera->Set( vCameraPos, vCameraDir);
	pCurrentCamera->UpdateTransform();
	// 랜더
	pDevice->SetRenderTarget(0, m_pBufferLeft);
	RenderCamera( pCurrentCamera, pWorldScene, pShadowStrategy, pRenderingCallbackInterface);

	// 우안 ----------------------------------------------------------------------
	// Move
	vCameraPos = vCameraPosOrg + vRight * fMoveFactor;
	// Rotation
	mat.MakeIdentity();
	mat.SetRotationZ( -fRotFactor);
	vCameraDir =  vCameraDirOrg * mat;
	pCurrentCamera->Set( vCameraPos, vCameraDir);
	pCurrentCamera->UpdateTransform();
	// 랜더
	pDevice->SetRenderTarget(0, m_pBufferRight);
	RenderCamera( pCurrentCamera, pWorldScene, pShadowStrategy, pRenderingCallbackInterface);

	// 원래 상태 복구 -------------------------------------------------------------------
	pCurrentCamera->Set( vCameraPosOrg, vCameraDirOrg);
	pCurrentCamera->UpdateTransform();
	pDevice->SetRenderTarget(0, hBeforeTarget0);


	RenderBlend();



	//////////////////////////////////////////////////////////////////////////////////////////
#else

	if(REngine::GetSceneManager().GetRenderingStrategy()->GetRenderingMode()==RRM_DIFFUSE_ONLY)
		RenderDiffuseOnly(pCurrentCamera, pWorldScene, pRenderingCallbackInterface);
	else
		RenderCamera( pCurrentCamera, pWorldScene, pShadowStrategy, pRenderingCallbackInterface);
#endif
}

void RDeferredRenderingStrategy::SetFXAA_Level( int _iFxaa_Lv )
{
	if( _iFxaa_Lv == m_iFXAA_Lv || 5 < _iFxaa_Lv || 0 > _iFxaa_Lv )
		return;

	m_iFXAA_Lv = _iFxaa_Lv;

	((RPostEffectFinalScene*)m_pPostEffectManager->Get(PE_FINAL_SCENE))->CreateShader();
}

void RDeferredRenderingStrategy::SetGammaCorrect( bool _bGammaCorrect )
{
	if( _bGammaCorrect == m_bUseGammaCorrect )
		return;

	m_bUseGammaCorrect = _bGammaCorrect;

	DestroyConstant();

	DWORD dwFlag( 0 );
	if( _bGammaCorrect )
		dwFlag |= DRF_GAMMA_CORRECT;
	CreateShader( dwFlag );

	((RPostEffectFinalScene*)m_pPostEffectManager->Get(PE_FINAL_SCENE))->CreateShader();
}


void RDeferredRenderingStrategy::RenderCamera( RCameraSceneNode *pCurrentCamera, RWorldSceneNode *pWorldScene, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface)
{
	RPFC_THISFUNC;

	RPFC_B("rs3::RDeferredRenderingStrategy::PreRenderCamera");
	RPFC_GPU_CALL;
	RPFC_E;

	REngine::GetDevice().SetTextureMipmapLodBias(0, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);
	REngine::GetDevice().SetTextureMipmapLodBias(1, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);

	RDevice* pDevice = REngine::GetDevicePtr();

	//Directional Light ShadowMap 생성
	if( !pCurrentCamera->GetPassIgnore( RRP_SHADOWMAP ) )
		pShadowStrategy->Render( pCurrentCamera, pWorldScene );


	
	RCullingMgr* pCullingMgr = pCurrentCamera->GetViewFrustumCullingMgr();
	pCullingMgr->Cull(pWorldScene,pCurrentCamera->GetViewFrustum().GetPlanes(),6);
	
	RenderSpotLightShadowMap(pCurrentCamera, pCullingMgr->GetCulledList(RRP_SPOTLIGHT));

	//SpotLight가 Light camera에 따라 pCullingMgr->CulledListUpdate()를 호출해주므로
	//SpotLight의 ShadowMap을 그린 다음에 View-camera에 대해 호출해준다.
	pCullingMgr->CulledListUpdate(pCurrentCamera);

	if( pCurrentCamera->GetEnableOcclusionCulling() )
	{
		OcclusionCullAndSetOccludedSceneNodes(pWorldScene,pCurrentCamera->GetOcclusionCullingMgrSet(),pCurrentCamera,pCurrentCamera->GetViewFrustumCullingMgr()->GetCulledList(RRP_OCCLUDER));
	}

	// LOD 계산
	CalculateLOD( pCurrentCamera, pCurrentCamera);

	// 일반적인 준비 패스
	pCurrentCamera->BeginRender( 0, false, this);
	pCurrentCamera->OnClearBuffer();

	RTexture* hBeforeTarget0 = pDevice->GetRenderTarget(0);
	RTexture* hBeforeTarget1 = pDevice->GetRenderTarget(1);
	RTexture* hBeforeTarget2 = pDevice->GetRenderTarget(2);
	RTexture* hBeforeTarget3 = pDevice->GetRenderTarget(3);

	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시, 전 패스에 영향을 미치는 디바이스 셋팅(FillMode 등)
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->DeviceSetting();

	//OnPreRender
	DoPreRender(pCurrentCamera);

	// G - stage
	RPFC_B("GeometryPass");

	pDevice->ColorFill( m_pGBufferDepthSpecRim, D3DCOLOR_ARGB(255,255,255,255));
	pDevice->ColorFill( m_pGBufferDiffuseAO, pCurrentCamera->GetClearColor());

	REngine::GetDevice().SetRenderTarget(0, m_pGBufferDiffuseAO);
	REngine::GetDevice().SetRenderTarget(1, NULL);
	REngine::GetDevice().SetRenderTarget(2, NULL);
	REngine::GetDevice().SetRenderTarget(3, NULL);

	pDevice->Clear( true, true, true, pCurrentCamera->GetClearColor());

	if( m_bEarlyZCull)
		RenderZStagePass( pCurrentCamera, RRP_START_PASS_OF_ZSTAGE, RRP_END_PASS_OF_ZSTAGE);

	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->PreOrderRender();

	// 하늘 등 백그라운드
	RenderBackgroundPass( pCurrentCamera, RRP_START_PASS_OF_BACKGROUND, RRP_END_PASS_OF_BACKGROUND);


	REngine::GetDevice().SetRenderTarget(0, m_pGBufferNormal);
	REngine::GetDevice().SetRenderTarget(1, m_pGBufferDepthSpecRim);
	REngine::GetDevice().SetRenderTarget(2, m_pGBufferDiffuseAO);

	if(pShadowStrategy->GetShadowDesc().eMethod==SHADOW_PROJECTION)
		RenderGStagePassForProjectionShadow(pCurrentCamera, RRP_START_PASS_OF_GSTAGE, RRP_END_PASS_OF_GSTAGE);
	else
		RenderGStagePass( pCurrentCamera, RRP_START_PASS_OF_GSTAGE, RRP_END_PASS_OF_GSTAGE);

	RPFC_E;



	// L - stage
	RPFC_B("LightPass");

	if( IsUsetLighting())
	{
		pDevice->SetRenderTarget(0, m_pLBuffer);
		pDevice->SetRenderTarget(1, NULL);
		pDevice->SetRenderTarget(2, NULL);
		pDevice->SetRenderTarget(3, NULL);
		pDevice->Clear( true, false);
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
	// RT가 1개 초과일 시만 후처리를 작동 시킨다.
	bool bActivePostEffect = m_pPostEffectManager && m_pPostEffectManager->IsBeingEnableInstance();
	if( bActivePostEffect )
	{
		m_pPostEffectManager->PreRender(pCurrentCamera, false);
	}

	RenderMergeDiffuseLight( pCurrentCamera, m_pGBufferNormal, m_pGBufferDepthSpecRim, m_pLBuffer, m_pGBufferDiffuseAO);
	RenderMStagePass( pCurrentCamera, RRP_START_PASS_OF_MSTAGE, RRP_END_PASS_OF_MSTAGE, m_pGBufferNormal, m_pGBufferDepthSpecRim, m_pLBuffer, m_pGBufferDiffuseAO);


	//OnPostRender
	DoPostRender(pCurrentCamera);


	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->PostOrderRender();


	//float fSize = 180;
	//float fGap = fSize + 10;
//	RRenderHelper::RenderTexture( 0, 0,	m_dwWidth, m_dwHeight, m_pGBufferNormal);
//	RRenderHelper::RenderTexture( 0, 0,	m_dwWidth/2, m_dwHeight, m_pGBufferDiffuseAO);
// 	RRenderHelper::RenderTexture( 0, 0,	m_dwWidth/2, m_dwHeight, m_pLBuffer, 1.0f/2.0f, 1, RBLEND_NONE);
// 	REngine::GetDevice().SetAlphaTestEnable(false);

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
					RenderKStagePass( pCurrentCamera, ePass, m_pGBufferNormal, m_pGBufferDepthSpecRim, m_pLBuffer, m_pGBufferDiffuseAO);
				}
			}

			enumerator.Next();
		}

		// 후처리 효과 최종 처리
		m_pPostEffectManager->RenderPost( pCurrentCamera, m_pGBufferNormal, m_pGBufferDepthSpecRim);
	}

	// ResetRender State
	//pDevice->SetDepthStencilBuffer( hBeforeDepth);
	REngine::GetDevice().SetTextureMipmapLodBias( 0, 0);
	REngine::GetDevice().SetTextureMipmapLodBias( 1, 0);

	pCurrentCamera->EndRender( false);
	pCurrentCamera->OnEndRender();

	RPFC_E;

	if( pCurrentCamera->GetEnableOcclusionCulling() )
	{
		pCurrentCamera->GetOcclusionCullingMgrSet()->ResetOccludedSceneNodes();
	}
}



void RDeferredRenderingStrategy::RenderDiffuseOnly( RCameraSceneNode *pCurrentCamera, RWorldSceneNode *pWorldScene, RRenderingCallbackInterface* pRenderingCallbackInterface)
{
	RPFC_THISFUNC;

	RPFC_B("rs3::RDeferredRenderingStrategy::RenderDiffuseOnly");
	RPFC_GPU_CALL;
	RPFC_E;


	REngine::GetDevice().SetTextureMipmapLodBias(0, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);
	REngine::GetDevice().SetTextureMipmapLodBias(1, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);


	RDevice* pDevice = REngine::GetDevicePtr();


	RCullingMgr* pCullingMgr = pCurrentCamera->GetViewFrustumCullingMgr();
	pCullingMgr->Cull(pWorldScene,pCurrentCamera->GetViewFrustum().GetPlanes(),6);
	pCullingMgr->CulledListUpdate(pCurrentCamera);

	if( pCurrentCamera->GetEnableOcclusionCulling() )
	{
		OcclusionCullAndSetOccludedSceneNodes(pWorldScene,pCurrentCamera->GetOcclusionCullingMgrSet(),pCurrentCamera,pCurrentCamera->GetViewFrustumCullingMgr()->GetCulledList(RRP_OCCLUDER));
	}

	// LOD 계산
	CalculateLOD( pCurrentCamera, pCurrentCamera);


	// 일반적인 준비 패스
	pCurrentCamera->BeginRender( 0, false, this);
	pCurrentCamera->OnClearBuffer();


	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시, 전 패스에 영향을 미치는 디바이스 셋팅(FillMode 등)
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->DeviceSetting();

	//OnPreRender
	DoPreRender(pCurrentCamera);

	// G - stage
	RPFC_B("GeometryPass");
	{
		pDevice->ColorFill( m_pGBufferDepthSpecRim, D3DCOLOR_ARGB(255,255,255,255));
		pDevice->ColorFill( m_pGBufferDiffuseAO, pCurrentCamera->GetClearColor());


		// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시
		if( pRenderingCallbackInterface)
			pRenderingCallbackInterface->PreOrderRender();
		// 하늘 등 백그라운드
		RenderBackgroundPass( pCurrentCamera, RRP_START_PASS_OF_BACKGROUND, RRP_END_PASS_OF_BACKGROUND);


		pDevice->Clear( false, true, true, pCurrentCamera->GetClearColor());
		RenderGStagePass( pCurrentCamera, RRP_START_PASS_OF_GSTAGE, RRP_END_PASS_OF_GSTAGE);
	}
	RPFC_E;



	// M - stage
	RPFC_B("MatrialPass");
	{
		pDevice->ColorFill( m_pLBuffer, D3DCOLOR_ARGB(0,255,255,255));

		//알파 오브젝트의 경우 Light-Buffer의 값을 쓰기 때문에 clear 해준다.
		REngine::GetDevice().ColorFill( m_pLBuffer, 0xffffffff);

		RenderMStagePass( pCurrentCamera, RRP_START_PASS_OF_MSTAGE, RRP_END_PASS_OF_MSTAGE, m_pGBufferNormal, m_pGBufferDepthSpecRim, m_pLBuffer, m_pGBufferDiffuseAO);
	}
	RPFC_E;


	//OnPostRender
	DoPostRender(pCurrentCamera);

	// 엔진 외부(주로 툴 App)서 따로 그려주는 것이 필요 시
	if( pRenderingCallbackInterface)
		pRenderingCallbackInterface->PostOrderRender();



	// ResetRender State
	REngine::GetDevice().SetTextureMipmapLodBias( 0, 0);
	REngine::GetDevice().SetTextureMipmapLodBias( 1, 0);		


	pCurrentCamera->EndRender( false);
	pCurrentCamera->OnEndRender();

	if( pCurrentCamera->GetEnableOcclusionCulling() )
	{
		pCurrentCamera->GetOcclusionCullingMgrSet()->ResetOccludedSceneNodes();
	}
}


void RDeferredRenderingStrategy::DestroyConstant()
{
	m_cFogFactor.Destroy();
	m_cFogColor.Destroy();
	m_scFarClipCornerInView.Destroy();
}

void RDeferredRenderingStrategy::SetDefaultTechnique( RShaderFX*  _pMergeEffect )
{
	m_TechniqueMergeFog = _pMergeEffect->GetTechnique( "TmergeFog" );
	m_TechniqueMergeNoFog = _pMergeEffect->GetTechnique( "TmergeNoFog" );

	m_cFogFactor = _pMergeEffect->GetConstant( "g_vFogFactor");
	m_cFogColor = _pMergeEffect->GetConstant( "g_vFogColor");
	m_scFarClipCornerInView = _pMergeEffect->GetConstant("g_vFarClipCornerInView");
#ifdef STREOPIC_TEST
	// 3D test -------------------------------------------------------------------
	m_TechniqueBlend = _pMergeEffect->GetTechnique( "TBlend" );
#endif
}

void RDeferredRenderingStrategy::CreateShader( DWORD dwFlag )
{
	SHADERLIST::const_iterator it = m_ShaderList.find( dwFlag );
	if( it != m_ShaderList.end() )
	{
		RShaderFX*  pMergeEffect = it->second;
		SetDefaultTechnique( pMergeEffect );
		return;
	}

	vector<string> strDefines;
		for( int i = 0; i < _countof(flagTable); ++i )
	{
		// 디파인을 무조건 밀어 넣고 1,0 값을 택일 하여 준다. 셰이더에서는 ifdef를 사용 하면 안되고 if만 사용 하여야 한다.
		strDefines.push_back( string(flagTable[i].strDefine) );
		if( ( flagTable[i].dwFlag & dwFlag ) != 0 )
			strDefines.push_back( "1" );
		else
			strDefines.push_back( "0" );
	}

	RShaderFX*  pMergeEffect( NULL );
	if( pMergeEffect = REngine::GetDevice().CreateShaderFX("DeferredMerge.fx", strDefines ))
		SetDefaultTechnique( pMergeEffect );

	m_ShaderList.insert( SHADERLIST::value_type( dwFlag, pMergeEffect ) );
}


void RDeferredRenderingStrategy::Init( int nW, int nH)
{
	RRenderingStrategy::Init( nW, nH);

	if( m_pGIProcessor)
		m_pGIProcessor->Init( m_dwWidth, m_dwHeight);

	Restore( m_dwWidth, m_dwHeight );

	DWORD dwFlag( 0 );
	if( m_bUseGammaCorrect )
		dwFlag = DRF_GAMMA_CORRECT;
	CreateShader( dwFlag );
}

void RDeferredRenderingStrategy::Restore( DWORD dwWidth, DWORD dwHeight )
{
	RRenderingStrategy::Restore( dwWidth, dwHeight );

	if( m_pGIProcessor)
		m_pGIProcessor->Restore();

	// CreateRenderTarget...
	if(!m_pGBufferDepthSpecRim)
		m_pGBufferDepthSpecRim	= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_G16R16F);
	if(!m_pGBufferNormal)
		m_pGBufferNormal		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_G16R16F);
	if(!m_pGBufferDiffuseAO)
		m_pGBufferDiffuseAO		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_A8R8G8B8);

	if(!m_pLBuffer)
		m_pLBuffer				= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_A16B16G16R16F);

#ifdef STREOPIC_TEST
	if(!m_pBufferLeft)
		m_pBufferLeft		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_A8R8G8B8);
	if(!m_pBufferRight)
		m_pBufferRight		= REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, RFMT_A8R8G8B8);
#endif

	if( !m_pGBufferDepthSpecRim || !m_pGBufferNormal || !m_pLBuffer || !m_pGBufferDiffuseAO)
	{
		mlog( "RDeferredRenderingStrategy::Init fail\n");
		_ASSERT( "RDeferredRenderingStrategy::Init fail");
	}

	REngine::GetDevice().ColorFill( m_pGBufferDepthSpecRim);
	REngine::GetDevice().ColorFill( m_pGBufferNormal);
	REngine::GetDevice().ColorFill( m_pGBufferDiffuseAO);
	REngine::GetDevice().ColorFill( m_pLBuffer, 0x00ffff00);	
}


void RDeferredRenderingStrategy::Invalidate()
{
	RRenderingStrategy::Invalidate();

	if( m_pGIProcessor)
		m_pGIProcessor->Invalidate();

	REngine::GetDevicePtr()->SafeDeleteTexture( m_pGBufferDepthSpecRim );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pGBufferNormal );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pGBufferDiffuseAO );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pLBuffer );
}


void RDeferredRenderingStrategy::Destroy()
{
	RRenderingStrategy::Destroy();

	if( m_pGIProcessor)
		m_pGIProcessor->Destroy();

	Invalidate();


	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end();  )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
		it = m_ShaderList.erase( it );
	}
	m_ShaderList.clear();

	DestroyConstant();


#ifdef STREOPIC_TEST
	REngine::GetDevicePtr()->DeleteTexture( m_pBufferLeft);
	m_pBufferLeft = NULL;
	REngine::GetDevicePtr()->DeleteTexture( m_pBufferRight);
	m_pBufferRight = NULL;
#endif

}

void RDeferredRenderingStrategy::Resize( int nW, int nH )
{
	RRenderingStrategy::Resize( nW, nH );

	if( m_pGIProcessor)
		m_pGIProcessor->Resize( nW, nH );
}


int RDeferredRenderingStrategy::GetMRTCount()
{
	if(GetRenderingMode()==RRM_DIFFUSE_ONLY)
		return 1;
	else
		return 3;
}



void RDeferredRenderingStrategy::RenderBackgroundPass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass)
{
	RPFC_THISFUNC;

	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if (!pCurrentCamera->GetPassIgnore( (RRENDERPASS)pass) )	// 무시 지정된 패스는 건너뜀
		{
			RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
			_ASSERT(pPassRenderer); // default pass renderer 가 있으므로 항상 있어야 한다
			pPassRenderer->RenderGStageOnPass( pCurrentCamera, pCurrentCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), RENDERING_DEFERRED);
		}
	}
}

void RDeferredRenderingStrategy::SetLStageTexture()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetTexture(AS_GNORMAL, GetNormalBuffer());
	REngine::GetDevice().SetTexture(AS_GDEPTH, GetDepthBuffer());
	REngine::GetDevice().SetTexture(AS_GDIFFUSE, GetDiffuseBuffer());
	REngine::GetDevice().SetTextureFilter(AS_GNORMAL, RTF_POINT);
	REngine::GetDevice().SetTextureFilter(AS_GDEPTH, RTF_POINT);
	REngine::GetDevice().SetTextureFilter(AS_GDIFFUSE, RTF_POINT);
}

void RDeferredRenderingStrategy::RestoreLStageState()
{
	REngine::GetDevice().SetTexture(AS_GNORMAL, NULL);
	REngine::GetDevice().SetTexture(AS_GDEPTH, NULL);
	REngine::GetDevice().SetTexture(AS_GDIFFUSE, NULL);
}


// 라이트 버퍼와 디퓨즈 버퍼를 합성 
void RDeferredRenderingStrategy::RenderMergeDiffuseLight( RCameraSceneNode* pCurrentCamera, RTexture* pGBufferNormal, RTexture* pGBufferDepth, RTexture* pLBuffer, RTexture* pGBufferDiffuse)
{
	RPFC_THISFUNC;

	//----------------------------------------------------------------------------------------------------
	//Set Render State
	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetCullMode(RCULL_NONE);

	RFILLMODE eFillMode = REngine::GetDevice().GetFillMode();

	REngine::GetDevice().SetFillMode(RFILL_SOLID);
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_LIGHTMAP, pLBuffer);
	REngine::GetDevice().SetTexture( AS_DIFFUSE, pGBufferDiffuse);
 	REngine::GetDevice().SetTexture( AS_GDEPTH, pGBufferDepth);
 	REngine::GetDevice().SetTexture( AS_GNORMAL, pGBufferNormal);
	REngine::GetDevice().SetTextureFilter( AS_VIEWSPACE_LIGHTMAP, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( AS_DIFFUSE, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( AS_GDEPTH, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( AS_GNORMAL, RTF_POINT);
	//----------------------------------------------------------------------------------------------------	

	// 비용을 줄이기 위해서 포그가 활성화 시만 포그 관련 셰이더 값을 설정.
	if( pCurrentCamera->GetFogEnable())
	{
		// 포그 거리 관련 상수
		float fNear = pCurrentCamera->GetFogNear();
		float fFar = pCurrentCamera->GetFogFar();
		float fDist = fFar - fNear;
		fDist = ( fDist == 0.0f ? 0 : 1.0f/fDist );
		m_cFogFactor.SetVector4(fNear, fFar, fDist, 0.0f);
		// 포그 컬러
		DWORD dwFogColor = pCurrentCamera->GetFogColor();
		RVector vecFogColor;
		DWColorToFloat3( &vecFogColor, dwFogColor );
		m_cFogColor.SetVector3( vecFogColor );

		// 깊이 추출 관련
		m_scFarClipCornerInView.SetVector3(pCurrentCamera->GetFarClipCornerInView());
	}

	UINT uiPass, uiPassCount;
	RTechnique* pTech = pCurrentCamera->GetFogEnable() ? &m_TechniqueMergeFog : &m_TechniqueMergeNoFog;
	pTech->Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for( uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		pTech->BeginPass(uiPass);
		RRenderHelper::RenderQuad(0, 0, (float)m_dwWidth, (float)m_dwHeight);
		pTech->EndPass();
	}
	pTech->End();

	// restore render state
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetFillMode( eFillMode );
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_LIGHTMAP, NULL);
	REngine::GetDevice().SetTexture( AS_DIFFUSE, NULL);
	REngine::GetDevice().SetTexture( AS_GDEPTH, NULL);
	REngine::GetDevice().SetTexture( AS_GNORMAL, NULL);
}

}