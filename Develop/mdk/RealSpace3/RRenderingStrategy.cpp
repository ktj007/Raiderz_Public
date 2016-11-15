#include "StdAfx.h"
#include "RRenderingStrategy.h"
#include "RPostEffectManager.h"
#include "RSpotLightSceneNode.h"
#include "RLightingShader.h"
#include "RShaderCompositeManager.h"
#include "RHBAOProcessor.h"
#include "RProjectionShadowStrategy.h"
#include "RDirectionalLightPassRenderer.h"
#include "RPointLightPassRenderer.h"
#include "RSpotLightPassRenderer.h"
#include "ROccluderSceneNode.h"
#include "ROcclusionCullingMgr.h"

namespace rs3 {

class RenderingStrategyLostListener : public RListener
{
	RRenderingStrategy* m_pRenderingStrategy;
public:
	RenderingStrategyLostListener( RRenderingStrategy* pRenderingStrategy) : m_pRenderingStrategy( pRenderingStrategy) {}

	virtual void Update() override
	{
		m_pRenderingStrategy->Invalidate();
	}
};

class RenderingStrategyRestoreListener : public RListener
{
	RRenderingStrategy* m_pRenderingStrategy;
public:
	RenderingStrategyRestoreListener( RRenderingStrategy* pRenderingStrategy) : m_pRenderingStrategy( pRenderingStrategy) {}

	virtual void Update() override
	{
		m_pRenderingStrategy->Restore( REngine::GetDevice().GetScreenWidth(), REngine::GetDevice().GetScreenHeight() );
	}
};

RRenderingStrategy::RRenderingStrategy()
:m_eMethod(RENDERING_END)
,m_pPostEffectManager(NULL)
,m_pLightingShader(NULL)
,m_bUseLighting(true)
,m_bSoftParticle(false)
,m_pGIProcessor(NULL)
,m_RenderingMode(RRM_FULL_LIGHT)
,m_bEarlyZCull(false)
,m_bUseGammaCorrect( false )
,m_iFXAA_Lv( 0 )
{
	// event 구독
	m_pLostEventListener = new RenderingStrategyLostListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_LOST)->AddListener( m_pLostEventListener );
	m_pRestoreEventListener = new RenderingStrategyRestoreListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pRestoreEventListener );

	m_pPostEffectManager = new RPostEffectManager();
	m_pLightingShader = new RLightingShader();

}

RRenderingStrategy::~RRenderingStrategy(void)
{
	// event 해제
	REngine::GetEventBroadcaster(RE_DEVICE_LOST)->RemoveListener( m_pLostEventListener );
	SAFE_DELETE(m_pLostEventListener);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pRestoreEventListener );
	SAFE_DELETE(m_pRestoreEventListener);
	SAFE_DELETE(m_pPostEffectManager);
	SAFE_DELETE(m_pLightingShader);
}

void RRenderingStrategy::SetGammaCorrect( bool _bGammaCorrect )
{ 
	m_bUseGammaCorrect = _bGammaCorrect;
}

void RRenderingStrategy::SetFXAA_Level( int _iFxaa_Lv )
{ 
	m_iFXAA_Lv = _iFxaa_Lv;
}


void RRenderingStrategy::Init(  int nW, int nH) 
{ 
	m_dwWidth = nW; m_dwHeight = nH; 
	if( m_pPostEffectManager)
		m_pPostEffectManager->Init( nW, nH);
	if( m_pLightingShader)
		m_pLightingShader->Init();
}

void RRenderingStrategy::Destroy() 
{
	if( m_pPostEffectManager)
		m_pPostEffectManager->Destroy();
	if( m_pLightingShader)
		m_pLightingShader->Destroy();
}

void RRenderingStrategy::Resize( int nW, int nH )
{
	m_dwWidth = nW;
	m_dwHeight = nH;

	if( m_pPostEffectManager)
		m_pPostEffectManager->Resize( nW, nH );
	if( m_pLightingShader)
		m_pLightingShader->Resize( nW, nH );
}

void RRenderingStrategy::Invalidate()
{
	if( m_pPostEffectManager)
		m_pPostEffectManager->Invalidate();
	if( m_pLightingShader)
		m_pLightingShader->Invalidate();
}

void RRenderingStrategy::Restore( DWORD dwWidth, DWORD dwHeight )
{
	// 버퍼 크기가 달라졌다면 resize 를 호출
	if( m_dwWidth != dwWidth || m_dwHeight != dwHeight )
	{
//		mlog( "Resize To %d %d\n", dwWidth, dwHeight );
		Resize( dwWidth, dwHeight );
	}

	if( m_pPostEffectManager)
		m_pPostEffectManager->Restore();
	if( m_pLightingShader)
		m_pLightingShader->Restore();
}


void RRenderingStrategy::DisablePEEdge()
{
	//m_pPostEffectManager->SetEnable(PE_EDGE, false);
}


void RRenderingStrategy::CalculateLOD(RCameraSceneNode* pViewCamera, RCameraSceneNode* pCamera)
{
	RPFC_THISFUNC;

	for(int pass = RRP_CULL_BEGIN; pass <= RRP_CULL_END ; ++pass)
	{
		if( pCamera->GetPassIgnore( (RRENDERPASS)pass ) == false )
		{
			SCENE_SET* pSet = pCamera->GetViewFrustumCullingMgr()->GetCulledList((RRENDERPASS)pass);
			for(SCENE_SET::iterator iter = pSet->begin(); iter != pSet->end() ; ++iter)
			{
				RSceneNode *pNode = (*iter);
				pNode->CalculateLOD( pViewCamera, &pNode->GetSphere());
			}
		}
	}

	RPFC_GPU_CALL;
}

void RRenderingStrategy::DoPreRender( RCameraSceneNode* pCamera )
{
	RPFC_THISFUNC;

	RShaderCompositeManager::SetGlobalRenderState(pCamera);

	//OnPreRender
	for(int pass = RRP_CULL_BEGIN; pass <= RRP_CULL_END ; ++pass)
	{
		if( pCamera->GetPassIgnore( (RRENDERPASS)pass ) == false )
		{
			SCENE_SET* pSet = pCamera->GetViewFrustumCullingMgr()->GetCulledList((RRENDERPASS)pass);
			for(SCENE_SET::iterator iter = pSet->begin(); iter != pSet->end() ; ++iter)
			{
				RSceneNode *pNode = (*iter);
				pNode->PreRender(pCamera);
			}
		}
	}

	RPFC_GPU_CALL;
}

void RRenderingStrategy::DoPostRender( RCameraSceneNode* pCamera )
{
	RPFC_THISFUNC;

	//OnPostRender
	for(int pass = RRP_CULL_BEGIN; pass <= RRP_CULL_END ; ++pass)
	{
		if( pCamera->GetPassIgnore( (RRENDERPASS)pass ) == false )
		{
			SCENE_SET* pSet = pCamera->GetViewFrustumCullingMgr()->GetCulledList((RRENDERPASS)pass);
			for(SCENE_SET::iterator iter = pSet->begin(); iter != pSet->end() ; ++iter)
			{
				RSceneNode *pNode = (*iter);
				pNode->PostRender(pCamera);
			}
		}
	}
}

void RRenderingStrategy::RenderSpotLightShadowMap(RCameraSceneNode* pViewCamera, SCENE_SET* pSpotLightSet)
{
	RPFC_THISFUNC;

	SCENE_SET::iterator iter;
	for( iter = pSpotLightSet->begin(); iter != pSpotLightSet->end() ; ++iter )
	{
		RSpotLightSceneNode* pSpotLight = (RSpotLightSceneNode*)(*iter);

		if( !pSpotLight->GetUsable() || !pSpotLight->GetVisible() || pSpotLight->GetOcclued() )
			continue;

		pSpotLight->RenderShadowMap(pViewCamera);
	}

	RPFC_GPU_CALL;
}

void RRenderingStrategy::RenderGStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass)
{
	SetStateGStagePass();

	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if (!pCurrentCamera->GetPassIgnore( (RRENDERPASS)pass) )	// 무시 지정된 패스는 건너뜀
		{
			RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
			_ASSERT(pPassRenderer); // default pass renderer 가 있으므로 항상 있어야 한다

			//Timer Reset & Start
			REngine::GetResourceProfiler().StartMeshProfiler();

			// render pass using strategy
			pPassRenderer->RenderGStageOnPass( pCurrentCamera, pCurrentCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), getMethod());
		}
	}

	ResetStateGStagePass();
}

void RRenderingStrategy::RenderGStagePassForProjectionShadow( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass)
{
	RPFC_THISFUNC;

	REngine::GetDevice().SetStencilEnable(true);

	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if (!pCurrentCamera->GetPassIgnore( (RRENDERPASS)pass) )
		{
			//Terrain은 Shadow-Receiver 이므로 Stencil-buffer에 0이 아닌 값으로 마스킹한다.
			if(pass==RRP_TERRAIN)
				REngine::GetDevice().SetStencilOp(RSTENCILOP_INCR,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
			//Receiver가 아닌 SceneNode들은 buffer-clear값(0)으로 마스킹한다.
			else
				REngine::GetDevice().SetStencilOp(RSTENCILOP_ZERO,RSTENCILOP_KEEP,RSTENCILOP_KEEP);

			RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
			_ASSERT(pPassRenderer); // default pass renderer 가 있으므로 항상 있어야 한다

			//Timer Reset & Start
			REngine::GetResourceProfiler().StartMeshProfiler();

			pPassRenderer->RenderGStageOnPass( pCurrentCamera, pCurrentCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), getMethod());
		}
	}

	REngine::GetDevice().SetStencilEnable(false);
	REngine::GetDevice().SetStencilOp(RSTENCILOP_KEEP,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
}

void RRenderingStrategy::SetStateMStagePass( RCameraSceneNode* pCurrentCamera, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDSFMap)
{
	RPFC_THISFUNC;

	RMatrix matToScreen;
	matToScreen.SetScreenSpaceMatrix(m_dwWidth, m_dwHeight);
	RMatrix matView = pCurrentCamera->GetViewMatrix();
	RMatrix matProjection = pCurrentCamera->GetProjectionMatrix();
	RMatrix matScreenSpaceTransform = matView * matProjection * matToScreen;
	RShaderCompositeManager::m_cScreenSpaceTransform.SetMatrix(matScreenSpaceTransform);

	/// Screen Size
	RShaderCompositeManager::m_cScreenSizeInv.SetVector2( RVector2( 1/(float)m_dwWidth, 1/(float)m_dwHeight));



	// 뷰 스페이스 노말
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_NORMALMAP, pViewNormalMap);
	REngine::GetDevice().SetTextureAddressClamp(AS_VIEWSPACE_NORMALMAP);
	REngine::GetDevice().SetTextureFilter(AS_VIEWSPACE_NORMALMAP, RTF_POINT);
	// 뷰 스페이스 뎁스
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_DEPTHMAP, pViewDepthMap);
	REngine::GetDevice().SetTextureAddressClamp(AS_VIEWSPACE_DEPTHMAP);
	REngine::GetDevice().SetTextureFilter(AS_VIEWSPACE_DEPTHMAP, RTF_POINT);
	// 뷰 스페이스 라이트 버퍼
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_LIGHTMAP, pViewLightMap);
	REngine::GetDevice().SetTextureAddressClamp(AS_VIEWSPACE_LIGHTMAP);
	REngine::GetDevice().SetTextureFilter(AS_VIEWSPACE_LIGHTMAP, RTF_POINT);

	if(pCurrentCamera->GetFogEnable())
		RShaderCompositeManager::FogOn(pCurrentCamera->GetFogNear(), pCurrentCamera->GetFogFar(), 0, pCurrentCamera->GetFogColor());
}

void RRenderingStrategy::RenderMStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDiffuseMap)
{
	RPFC_THISFUNC;
	SetStateMStagePass(pCurrentCamera,pViewNormalMap,pViewDepthMap,pViewLightMap,pViewDiffuseMap);

	// iterate render pass
	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if (!pCurrentCamera->GetPassIgnore( (RRENDERPASS)pass) )	// 무시 지정된 패스는 건너뜀
		{
			RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
			_ASSERT(pPassRenderer); // default pass renderer 가 있으므로 항상 있어야 한다

			//Timer Reset & Start
			REngine::GetResourceProfiler().StartMeshProfiler();

			pPassRenderer->RenderMStageOnPass( pCurrentCamera, pCurrentCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), getMethod());
		}
	}
	ResetStateMStagePass();

	//메쉬 프로파일 관련 - 메쉬를 공유하는 인스턴스의 수를 계산한다.
	REngine::GetResourceProfiler().CountMeshInstance(pCurrentCamera->GetViewFrustumCullingMgr()->GetCulledList(RRP_ACTOR));
}

void RRenderingStrategy::ResetStateMStagePass(void)
{
	RPFC_THISFUNC;

	REngine::GetDevice().SetTexture( AS_VIEWSPACE_NORMALMAP, NULL);
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_DEPTHMAP, NULL);
	REngine::GetDevice().SetTexture( AS_VIEWSPACE_LIGHTMAP, NULL);	

	RShaderCompositeManager::FogOff();
}

void RRenderingStrategy::RenderKStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS pass, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDSFMap)
{
	RPFC_THISFUNC;

	SetStateMStagePass(pCurrentCamera,pViewNormalMap,pViewDepthMap,pViewLightMap,pViewDSFMap);

	RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
	_ASSERT(pPassRenderer); // default pass renderer 가 있으므로 항상 있어야 한다

	pPassRenderer->RenderMStageOnPass( pCurrentCamera, pCurrentCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), m_eMethod);
	ResetStateMStagePass();

}




void RRenderingStrategy::SetStateZStagePass( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;

	REngine::GetDevice().SetColorWriteEnable( false);
	REngine::GetDevice().SetAlphaTestEnable( false);
	REngine::GetDevice().SetBlending( RBLEND_NONE, RBLEND_NONE);
	
}

void RRenderingStrategy::RenderZStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass)
{
	RPFC_THISFUNC;
	SetStateZStagePass(pCurrentCamera);

	// iterate render pass
	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if (!pCurrentCamera->GetPassIgnore( (RRENDERPASS)pass) )	// 무시 지정된 패스는 건너뜀
		{
			RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
			_ASSERT(pPassRenderer); // default pass renderer 가 있으므로 항상 있어야 한다

			//Timer Reset & Start
			REngine::GetResourceProfiler().StartMeshProfiler();

			pPassRenderer->RenderZStageOnPass( pCurrentCamera, pCurrentCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), getMethod());
		}
	}
	ResetStateZStagePass();

	//메쉬 프로파일 관련 - 메쉬를 공유하는 인스턴스의 수를 계산한다.
	REngine::GetResourceProfiler().CountMeshInstance(pCurrentCamera->GetViewFrustumCullingMgr()->GetCulledList(RRP_ACTOR));
}

void RRenderingStrategy::ResetStateZStagePass(void)
{
	RPFC_THISFUNC;

	REngine::GetDevice().SetColorWriteEnable( true);
}















void RRenderingStrategy::SetPEQuality(int _iBlurCount, int _iScreenSize)
{
	m_pPostEffectManager->PEEdgeQuality(_iBlurCount, _iScreenSize );
}


void RRenderingStrategy::SetPEFactorTickNess(const float _fTickness, const float _fBlutFactor, const float _fColorPower)
{
	m_pPostEffectManager->PEFactorTickNess( _fTickness, _fBlutFactor, _fColorPower );
}

void RRenderingStrategy::UseSSAO( int nQual)
{
	if( 0 < nQual)
	{
		if(!m_pGIProcessor
			|| m_pGIProcessor->GetQuality() != nQual)
		{
			SAFE_DELETE( m_pGIProcessor);
			m_pGIProcessor = new RHBAOProcessor( nQual);
			m_pGIProcessor->Init( m_dwWidth, m_dwHeight);
		}
	}
	else
	{
		SAFE_DELETE( m_pGIProcessor);
	}


}

int RRenderingStrategy::IsUsingSSAO()
{
	if( m_pGIProcessor)
		return m_pGIProcessor->GetQuality();
	else
		return false;
}

void RRenderingStrategy::SetLStageShaderConstant(RCameraSceneNode* pCamera, DWORD dwBufferWidth, DWORD dwBufferHeight)
{
	RPFC_THISFUNC;
	// 정점을 스크린 공간 좌표로 변환해주는 행렬.
	RMatrix matToScreen;
	matToScreen.SetScreenSpaceMatrix(dwBufferWidth, dwBufferHeight);
	RMatrix matScreenSpaceTransform = pCamera->GetViewProjectionMatrix() * matToScreen;


	_ASSERT( m_pLightingShader );
	m_pLightingShader->m_scWorldView.SetMatrix(pCamera->GetViewMatrix());
	m_pLightingShader->m_scWorldViewProj.SetMatrix( pCamera->GetViewProjectionMatrix());
	m_pLightingShader->m_scFarClipCornerInView.SetVector3(pCamera->GetFarClipCornerInView());
	if( dwBufferWidth > 0 && dwBufferHeight > 0)
		m_pLightingShader->m_scResolution.SetVector4( RVector4( (float)dwBufferWidth, (float)dwBufferHeight, 0.5f/(float)dwBufferWidth, 0.5f/(float)dwBufferHeight));
}

void RRenderingStrategy::RenderProjectionShadowBuffer(RCameraSceneNode* pCamera)
{
	RPFC_THISFUNC;
	if(REngine::GetSceneManager().GetRealtimeShadowMethod()!=SHADOW_PROJECTION)
		return;


	RProjectionShadowStrategy* pProjection = (RProjectionShadowStrategy*)REngine::GetSceneManager().GetShadowStrategy();
	_ASSERT(pProjection&& pProjection->GetShadowDesc().eMethod==SHADOW_PROJECTION);

	pProjection->RenderShadowBuffer(pCamera, m_pLightingShader);

	//이후에 스텐실 버퍼를 사용하는 경우를 위해 clear 해준다.
	REngine::GetDevice().Clear(false, false, true);
}

void RRenderingStrategy::RenderDirectionalLightingToLBuffer( RCameraSceneNode* pCamera, SCENE_SET* pLightSet, const RTextureInfo& LBufferInfo)
{
	RPFC_THISFUNC;
	REngine::GetSceneManager().GetPassRenderer<RDirectionalLightPassRenderer>()->RenderLightingToLBuffer(pCamera, pLightSet, m_pLightingShader, LBufferInfo.nTextureWidth, LBufferInfo.nTextureHeight, m_pGIProcessor, GetRenderingMode());
	RPFC_GPU_CALL;
}

void RRenderingStrategy::RenderPointLightingToLBuffer( RCameraSceneNode* pCurrentCamera,SCENE_SET*	pLightSet )
{
	RPFC_THISFUNC;
	REngine::GetSceneManager().GetPassRenderer<RPointLightPassRenderer>()->RenderPointLightingToLBuffer(pCurrentCamera, pLightSet, m_pLightingShader, GetRenderingMode());
	RPFC_GPU_CALL;
}

void RRenderingStrategy::RenderSpotLightingToLBuffer( RCameraSceneNode* pCurrentCamera, SCENE_SET* pLightSet)
{
	RPFC_THISFUNC;
	REngine::GetSceneManager().GetPassRenderer<RSpotLightPassRenderer>()->RenderSpotLightingToLBuffer(pCurrentCamera, pLightSet, m_pLightingShader, GetRenderingMode());
	RPFC_GPU_CALL;
}

void RRenderingStrategy::RenderLight(RCameraSceneNode* pCamera, RCullingMgr* pCullingMgr, const RTextureInfo& LBufferInfo)
{
	RenderDirectionalLightingToLBuffer( pCamera, pCullingMgr->GetCulledList(RRP_LIGHT), LBufferInfo );
	RenderPointLightingToLBuffer( pCamera,pCullingMgr->GetCulledList(RRP_LIGHT));		
	RenderSpotLightingToLBuffer( pCamera,pCullingMgr->GetCulledList(RRP_SPOTLIGHT));

}

void RRenderingStrategy::RenderGIToLBuffer( RCameraSceneNode* pCamera, const RTextureInfo& LBufferInfo)
{
	RPFC_THISFUNC;

	if( m_pGIProcessor && m_pGIProcessor->IsAllLight())
		m_pGIProcessor->Render( pCamera, LBufferInfo.nTextureWidth, LBufferInfo.nTextureHeight, true);

	RPFC_GPU_CALL;
}

void RRenderingStrategy::RenderLStagePass( RCameraSceneNode* pCurrentCamera, RGIProcessor* pGIProcessor, RCullingMgr* pCullingMgr, const RTextureInfo& LBufferInfo)
{
	RPFC_THISFUNC;

	SetLStageShaderConstant(pCurrentCamera, LBufferInfo.nTextureWidth, LBufferInfo.nTextureHeight);
	SetLStageTexture();

	RenderProjectionShadowBuffer(pCurrentCamera);

	RenderLight(pCurrentCamera, pCullingMgr, LBufferInfo);
	RenderGIToLBuffer( pCurrentCamera, LBufferInfo);

	RestoreLStageState();
}

void RRenderingStrategy::OcclusionCullAndSetOccludedSceneNodes(RWorldSceneNode* pWorldScene,ROcclusionCullingMgrSet* pOcclusionCullingMgrSet,RCameraSceneNode *pCamera,SCENE_SET* pOccluderSceneList)
{
	RPFC_THISFUNC;

	_ASSERT(NULL != pOcclusionCullingMgrSet);

	ROccluderList	OccluderList;
	for(SCENE_SET::iterator itor = pOccluderSceneList->begin();itor != pOccluderSceneList->end();itor++)
	{
		ROccluderSceneNode* pOccluderSceneNode = static_cast<ROccluderSceneNode*>(*itor);
		if( pOccluderSceneNode->GetWorldTransformedOccluderPointsSet() )
		{
			OccluderList.AddOccluderPointsSet( pOccluderSceneNode->GetWorldTransformedOccluderPointsSet() );
		}
	}
	pOcclusionCullingMgrSet->OcclusionCull(pWorldScene,pCamera->GetPosition(),OccluderList);
	pOcclusionCullingMgrSet->SetOccludedSceneNodes();
}

void RRenderingStrategy::ResetOcludedSceneNodes( ROcclusionCullingMgrSet* pOcclusionCullingMgrSet )
{
	pOcclusionCullingMgrSet->ResetOccludedSceneNodes();
}

}