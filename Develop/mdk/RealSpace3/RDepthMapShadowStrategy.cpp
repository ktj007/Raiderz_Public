#include "StdAfx.h"
#include "RDepthMapShadowStrategy.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"
#include "RPSSMShadowStrategy.h"
#include "RShaderFX.h"
#include "RActorPassRenderer.h"
#include "RActorNodeRenderStrategy.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
RDepthMapShadowStrategy::RDepthMapShadowStrategy()
{
	m_pShaderClearFloat = REngine::GetDevice().CreateShaderFX("Clear.fx");

	const float fToler = 5.f;	///< 그림자가 아니라고 인정하는 최소값
	m_matToler.MakeIdentity();
	m_matToler.SetTranslation( RVector(0,0,-fToler) );

	m_pRenderTarget = m_pDepthBuffer = NULL;
	m_pReusedRT =NULL;
	m_pCommonShadowRT = m_pDoubleShadowRT = NULL;

	m_bResetDepthMap =true;
	m_nPrevRT =0;
	m_nCurrentRT=1;

	m_pDirectionalLight =NULL;

	m_bUseRenderMinArea =false;
	m_fMinRenderArea =0.002f;

	m_vDepthBias.Set( 0.f, 0.f, 0.f, 0.f );
}

RDepthMapShadowStrategy::~RDepthMapShadowStrategy()
{
	if(m_pShaderClearFloat)
	{
		REngine::GetDevice().DeleteShaderFX(m_pShaderClearFloat);
		m_pShaderClearFloat = NULL;
	}
}

/// shadow casting 할 노드들을 cull 하여 등록한다
void RDepthMapShadowStrategy::CullCasters( RWorldSceneNode* pWorld, RCameraSceneNode &shadowCamera )
{
	RPFC_THISFUNC;
	// 카메라 업데이트
	shadowCamera.Traverse(&m_ShadowCameraUpdator);
	shadowCamera.UpdateViewFrustrum();

	// Cull 을 하고 ( Near은 광원 방향이므로 제외한다. )
	const RPlane *pLightViewFrustumPlanes = shadowCamera.GetViewFrustum().GetPlanes();

	RPlane	cullingPlanes[5];
	for(unsigned i=0;i<4;i++)	cullingPlanes[i] = pLightViewFrustumPlanes[i];
	cullingPlanes[4] = pLightViewFrustumPlanes[RV_FAR];

	RCullingMgr* pCullingMgr = shadowCamera.GetViewFrustumCullingMgr();
	
	pCullingMgr->Cull(pWorld,cullingPlanes,5,true);

	vector<RCULLDESC> vecCullDesc;
	RCULLDESC CullDesc;
	CullDesc.SetDesc(cullingPlanes, 4);
	vecCullDesc.push_back(CullDesc);

	pCullingMgr->CullSubNode(&shadowCamera, vecCullDesc, NULL, &pLightViewFrustumPlanes[RV_FAR]);

}


/// Caster 중에서 제일 광원에 가까운 것과 먼 것을 찾아, 그 값을 얻는다.
void RDepthMapShadowStrategy::FindMinMaxZObject(RCameraSceneNode &shadowCamera, RRENDERPASS beginPass, RRENDERPASS endPass, float& fOutmin, float& fOutmax)
{
	RPFC_THISFUNC;
	fOutmin =-1;
	fOutmax = 1;


	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if (shadowCamera.GetPassIgnore( (RRENDERPASS)pass) )
			continue;

		SCENE_SET* pSet = shadowCamera.GetViewFrustumCullingMgr()->GetCulledList((RRENDERPASS)pass);

		for(SCENE_SET::iterator iter = pSet->begin() ; iter != pSet->end() ; ++iter )
		{
			const RSceneNode* pSceneNode = (*iter);

			float fMin =0;
			float fMax =0;

			shadowCamera.FindMinMaxZ(pSceneNode, fMin, fMax);

			fOutmin = min(fOutmin, fMin);
			fOutmax = max(fOutmax, fMax);
		}
	}

}

void RDepthMapShadowStrategy::RenderShadowMap(RCameraSceneNode& shadowCamera)
{
	RPFC_THISFUNC;
	// 카메라 업데이트
	shadowCamera.Traverse(&m_ShadowCameraUpdator);
	shadowCamera.UpdateViewFrustrum();

	//Set RenderTaret
	RTexture* pRT = REngine::GetDevice().GetRenderTarget(0);
	RTexture* pDepthBuffer = REngine::GetDevice().SetDepthStencilBuffer(m_pDepthBuffer);
	RViewport oldViewport = REngine::GetDevice().GetViewport();


	REngine::GetSceneManagerPtr()->GetRenderingStrategy()->DoPreRender(&shadowCamera);
	{
		//Render DoubleShadow Actor
		if( m_shadowDesc.bDoubleShadow )
		{
			SetandClearRenderTarget(&shadowCamera, m_pDoubleShadowRT);
			RenderDoubleShadowSceneNode(&shadowCamera);

			SetandClearRenderTarget(&shadowCamera, m_pCommonShadowRT);
			RenderSceneNode(&shadowCamera, RRP_RENDER_BEGIN, RRP_RENDER_END);
		}
		else
		{
			SetandClearRenderTarget(&shadowCamera, m_pRenderTarget);
			RenderSceneNode(&shadowCamera, RRP_RENDER_BEGIN, RRP_RENDER_END);
		}		
	}
	REngine::GetSceneManagerPtr()->GetRenderingStrategy()->DoPostRender(&shadowCamera);

	REngine::GetDevice().SetRenderTarget(0, pRT);
	REngine::GetDevice().SetDepthStencilBuffer(pDepthBuffer);
	REngine::GetDevice().SetViewport( oldViewport);	
}

void RDepthMapShadowStrategy::RenderShadowMap(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld, RCameraSceneNode &shadowCamera,
											  RShadowTransform* ShadowTransform, float fNearZ, float fFarZ, const RVector3 &vLightDirection)
{
	RPFC_THISFUNC;
	ShadowTransform->SetupForCull( pViewCamera, vLightDirection, fNearZ, fFarZ, true );

	// 컬링준비 : 광원 카메라 및 투영 행렬 세팅 ( 1차 )
	shadowCamera.SetUserView( ShadowTransform->GetLightViewMatrix() );
	shadowCamera.SetUserProjection( ShadowTransform->GetLightProjectionMatrix() );

	CullCasters( pWorld, shadowCamera );
	// ViewCamera 기준 LOD 계산
	REngine::GetSceneManagerPtr()->GetRenderingStrategy()->CalculateLOD( pViewCamera, &shadowCamera);

	// 광원방향 최대/최소 지점을 찾아 바운딩을 확장한다
	float fmin = 0.0f;
	float fmax = 0.0f;

	FindMinMaxZObject(shadowCamera, RRP_SPEEDTREE, RRP_STATICMESH, fmin, fmax);

	// 검색해서 찾은 Z 방향 최대최소값으로 행렬을 조정한다 ( 2차 )
	ShadowTransform->AdjustMinMaxZ( fmin, fmax );

	ShadowTransform->SetupForRender();

	// 렌더링 준비
	shadowCamera.SetPosition( ShadowTransform->GetCameraPosition() );
	shadowCamera.SetUserView( ShadowTransform->GetLightViewMatrix() );
	shadowCamera.SetUserProjection( ShadowTransform->GetLightProjectionMatrix() );

	RenderShadowMap( shadowCamera );
}

void RDepthMapShadowStrategy::ClearDepth(int x, int y, int nWidth, int nHeight)
{
	if(!m_pShaderClearFloat) return;

	// 그림자가 아니도록 큰 값으로 클리어
	RTechnique tecClear = m_pShaderClearFloat->GetTechnique("Clear");
	tecClear.Begin(0, TECH_PRESERVE_NOT);
	tecClear.BeginPass();

	RRenderHelper::RenderQuad(x,y,nWidth,nHeight);

	tecClear.EndPass();
	tecClear.End();
}

void RDepthMapShadowStrategy::SetandClearRenderTarget(RCameraSceneNode* pCamera, RTexture* pRenderTarget)
{
	REngine::GetDevice().SetRenderTarget(0, pRenderTarget);

	if( pCamera->IsHaveUserViewPort() )
	{
		RViewport ViewPort = pCamera->GetViewPort();

		ClearDepth(ViewPort.x, ViewPort.y, ViewPort.nWidth, ViewPort.nHeight);

		REngine::GetDevice().SetViewport(ViewPort);
	}
	else
	{
		ClearDepth(0, 0, m_nTextureSize, m_nTextureSize);
		REngine::GetDevice().SetViewport( m_fBorderSize, m_fBorderSize, m_nTextureSize-2.f*m_fBorderSize, m_nTextureSize-2.f*m_fBorderSize);
	}

	REngine::GetDevice().Clear(false, true, false);
}

void RDepthMapShadowStrategy::MakeTextureScaleTransform()
{
	// 테두리 픽셀을 사용하지 않으므로 줄인다
	float fBorderTextureSize = m_fBorderSize / (float)m_nTextureSize;

	m_matTextureScale.MakeIdentity();
	m_matTextureScale._11 = 0.5f - fBorderTextureSize;
	m_matTextureScale._22 = - ( 0.5f - fBorderTextureSize);
	m_matTextureScale._41 = 0.5f;
	m_matTextureScale._42 = 0.5f;

	// 텍셀의 코너값이 텍스쳐의 중점에 기록되므로 반텍셀 만큼 이동해준다
	float fHalfTexelSize = .5f / (float)m_nTextureSize;
	m_matTextureScale._41 += fHalfTexelSize;
	m_matTextureScale._42 += fHalfTexelSize;
}

RSCID RDepthMapShadowStrategy::GetDepthWriteComponent()
{
	return RSC_TRANSFORM_CONSTANT | RSC_DEPTH_WRITE_ESM;
}

void RDepthMapShadowStrategy::RenderReusedShadowMap(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld, RCameraSceneNode* pCamera,
															   RShadowTransform* ShadowTransform, float fNearZ, float fFarZ)
{
	RVector3 vPrevPosition = pCamera->GetPosition();
	RViewFrustum PrevFrustum = pCamera->GetViewFrustum();
	RMatrix matPrevViewProj = pCamera->GetViewMatrix()*pCamera->GetProjectionMatrix();

	ShadowTransform->SetupForCull( pViewCamera, m_vLightDirection, fNearZ, fFarZ, m_bResetDepthMap );

	pCamera->SetPosition( ShadowTransform->GetCameraPosition() );
	pCamera->SetUserView( ShadowTransform->GetLightViewMatrix() );
	pCamera->SetUserProjection( ShadowTransform->GetLightProjectionMatrix() );
	pCamera->UpdateViewFrustrum();

	REngine::GetDevice().SetDepthStencilBuffer(m_pDepthBuffer);

	pCamera->GetViewFrustumCullingMgr()->Cull(pWorld, pCamera->GetViewFrustum().GetPlanes(), 4, true);
	
	RenderStaticSceneNode(pCamera, pWorld, ShadowTransform, vPrevPosition, matPrevViewProj);
	RenderDynamicSceneNode(pWorld, pCamera, ShadowTransform);
}

void RDepthMapShadowStrategy::SetStaticPass(RCameraSceneNode* pCamera)
{
	pCamera->SetPassIgnore((RRENDERPASS)0);
	pCamera->SetPassIgnore(RRP_STATICMESH, false);
	pCamera->SetPassIgnore( RRP_TERRAIN, false);
}

void RDepthMapShadowStrategy::SetDynamicPass(RCameraSceneNode* pCamera)
{
	pCamera->SetPassIgnore((RRENDERPASS)0);
	pCamera->SetPassIgnore(RRP_SPEEDTREE, false);
	pCamera->SetPassIgnore(RRP_ACTOR, false);
}

bool RDepthMapShadowStrategy::CullUpdatedRegion(RWorldSceneNode* pWorld, RCameraSceneNode* pCamera, RViewFrustum* pPrevFrustum, RVector3 vCameraDelta, 
												RRENDERPASS BeginPass, RRENDERPASS EndPass)
{
	std::vector<RCULLDESC> vecCullDesc;

	RPlane* pCurrentPlanes = const_cast<RPlane*>(pCamera->GetViewFrustum().GetPlanes());

	RPlane CullPlanes0[4];
	RPlane CullPlanes1[4];
	RCULLDESC CullDesc;

	if(pPrevFrustum)
	{
		if(vCameraDelta.x!=0)
		{
			GetCullPlanesForX(pPrevFrustum, &pCamera->GetViewFrustum(), vCameraDelta.x, CullPlanes0);

			CullDesc.SetDesc(CullPlanes0, 4);
			vecCullDesc.push_back(CullDesc);
		}

		if(vCameraDelta.y!=0)
		{
			GetCullPlanesForY(pPrevFrustum, &pCamera->GetViewFrustum(), vCameraDelta.y, CullPlanes1);

			CullDesc.SetDesc(CullPlanes1, 4);
			vecCullDesc.push_back(CullDesc);
		}
	}
	else
	{
		CullDesc.SetDesc(pCurrentPlanes, 4);
		vecCullDesc.push_back(CullDesc);
	}

	if(!vecCullDesc.empty())
	{
		pCamera->GetViewFrustumCullingMgr()->CullSubNode(pCamera, vecCullDesc, NULL, NULL);
		return true;
	}

	return false;
}

void RDepthMapShadowStrategy::RenderStaticSceneNode(RCameraSceneNode* pCamera, RWorldSceneNode* pWorld, RShadowTransform* pShadowTranform, RVector3 vPrevLightPos, RMatrix matPrevViewProj)
{
	//--------------------------------------------------------------------------------------------------
	//Calculate LightView-Space Delta
	RMatrix matView = pShadowTranform->GetLightViewMatrix();
	RVector4 vPrevViewPosition;
	RVector4 vCurrentViewPosition;
	matView.TransformVect(vPrevLightPos, vPrevViewPosition);
	matView.TransformVect(pShadowTranform->GetCameraPosition(), vCurrentViewPosition);

	RVector3 vCameraDelta = (vPrevViewPosition-vCurrentViewPosition);
	//--------------------------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------------------------
	//Culling
	SetStaticPass(pCamera);

	bool bCulled= false;
	if(m_bResetDepthMap)
		bCulled = CullUpdatedRegion(pWorld, pCamera, NULL, vCameraDelta, RRP_TERRAIN, RRP_STATICMESH);
	else
	{
		RViewFrustum PrevFrustum;
		PrevFrustum.SetFromMatrix(matPrevViewProj);

		bCulled = CullUpdatedRegion(pWorld, pCamera, &PrevFrustum, vCameraDelta, RRP_TERRAIN, RRP_STATICMESH);
	}
	//--------------------------------------------------------------------------------------------------



	//--------------------------------------------------------------------------------------------------
	//Rendering
	REngine::GetDevice().SetRenderTarget(0, m_pReusedRT[m_nCurrentRT]);

	if(bCulled)
	{
		float fmin = 0.0f;
		float fmax = 0.0f;

		FindMinMaxZObject(*pCamera, RRP_TERRAIN, RRP_STATICMESH, fmin, fmax);

		pShadowTranform->AdjustMinMaxZ( fmin, fmax );
		pShadowTranform->SetupForRender();

		pCamera->SetUserProjection( pShadowTranform->GetLightProjectionMatrix() );
		pCamera->UpdateViewFrustrum();

		ClearDepth(0, 0, m_nTextureSize, m_nTextureSize);		//Color Buffer Clear
		REngine::GetDevice().Clear(false, true);				//Depth Buffer Clear
		RenderSceneNode(pCamera, RRP_TERRAIN, RRP_STATICMESH);

		if(!m_bResetDepthMap)
			RenderPrevDepthMap(matPrevViewProj, pCamera->GetViewProjectionMatrix());
	}
	else
		RenderPrevDepthMap(matPrevViewProj, matPrevViewProj);		
	//--------------------------------------------------------------------------------------------------
}

void RDepthMapShadowStrategy::RenderPrevDepthMap(const RMatrix& matPrevViewProj, const RMatrix& matCurrentViewProj)
{
	//--------------------------------------------------------------------------------------------------
	//Calculate Screen Space Delta
	RVector3 vPrevP(matPrevViewProj._41, matPrevViewProj._42, matPrevViewProj._43);
	RVector3 vCurrentP(matCurrentViewProj._41, matCurrentViewProj._42, matCurrentViewProj._43);

	RVector3 vPixelDelta = (vPrevP-vCurrentP)/2.0f;
	//--------------------------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------------------------
	//Render Prev DepthMap
	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetTexture(0, m_pReusedRT[m_nPrevRT]);
	REngine::GetDevice().SetTextureFilter(0, RTF_POINT);
	
	m_pShaderClearFloat->GetConstant("g_TexcoordDelta").SetVector3(vPixelDelta);


	RTechnique tecRenderFloat = m_pShaderClearFloat->GetTechnique("RenderDepthMap");
	tecRenderFloat.Begin(0, TECH_PRESERVE_NOT);
	tecRenderFloat.BeginPass();

	RRenderHelper::RenderQuad(0, 0, m_nTextureSize, m_nTextureSize);

	tecRenderFloat.EndPass();
	tecRenderFloat.End();
	
	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetDepthEnable(true, true);
	//-----------------------------------------------------------------------------------------------------
}

void RDepthMapShadowStrategy::RenderDynamicSceneNode(RWorldSceneNode *pWorld, RCameraSceneNode* pCamera, RShadowTransform* pShadowTranform)
{
	SetDynamicPass(pCamera);

	float fmin = 0.0f;
	float fmax = 0.0f;

	FindMinMaxZObject(*pCamera, RRP_SPEEDTREE, RRP_ACTOR, fmin, fmax);
	pShadowTranform->AdjustMinMaxZ( fmin, fmax);
	pCamera->SetUserProjection( pShadowTranform->GetLightProjectionMatrix() );
	pCamera->UpdateViewFrustrum();

	REngine::GetDevice().SetRenderTarget(0, m_pReusedRT[m_nPrevRT]);

	ClearDepth(0, 0, m_nTextureSize, m_nTextureSize);		//Color Buffer Clear
	REngine::GetDevice().Clear(false, true);				//Depth Buffer Clear

	RenderSceneNode(pCamera, RRP_SPEEDTREE, RRP_ACTOR);
}

void RDepthMapShadowStrategy::RenderSceneNode(RCameraSceneNode* pCamera, RRENDERPASS beginPass, RRENDERPASS endPass)
{
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	pCamera->SetTreeUserLOD(true, 1.f);

	for( int pass = beginPass; pass <= endPass; ++pass )
	{
		if( pCamera->GetPassIgnore( (RRENDERPASS)pass) )
			continue;

		//Timer Reset & Start
		REngine::GetResourceProfiler().StartMeshProfiler();

		RPassRenderer* pPassRenderer = REngine::GetSceneManager().GetPassRenderer( (RRENDERPASS)pass );
		pPassRenderer->RenderDepthOnPass( pCamera, pCamera->GetViewFrustumCullingMgr()->GetRenderingList((RRENDERPASS)pass), this );
	}
}

void RDepthMapShadowStrategy::RenderDoubleShadowSceneNode(RCameraSceneNode* pCamera)
{
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	REngine::GetSceneManager().GetPassRenderer<RActorPassRenderer>()->RenderDoubleShadowDepthOnPass(pCamera, this);
}

void RDepthMapShadowStrategy::CombineDepthMap(int nDepthMapCount)
{
	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetTextureFilter(0, RTF_POINT);
	REngine::GetDevice().SetTextureFilter(1, RTF_POINT);

	REngine::GetDevice().SetRenderTarget(0, m_pRenderTarget);

	RTechnique tecRenderFloat = m_pShaderClearFloat->GetTechnique("CombineDepthMap");
	tecRenderFloat.Begin(0, TECH_PRESERVE_NOT);
	tecRenderFloat.BeginPass();

	for(int i=REUSE_MIN_INDEX; i<nDepthMapCount; ++i )
	{
		REngine::GetDevice().SetTexture(0, m_pReusedRT[i*2]);
		REngine::GetDevice().SetTexture(1, m_pReusedRT[i*2 +1]);
	
		RRenderHelper::RenderQuad(0, m_nTextureSize*i, m_nTextureSize, m_nTextureSize);
	}

	tecRenderFloat.EndPass();
	tecRenderFloat.End();

	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
}

void RDepthMapShadowStrategy::CombineDoubleShadowMap()
{
	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetTextureFilter(0, RTF_POINT);
	REngine::GetDevice().SetTextureFilter(1, RTF_POINT);

	RTexture* pRT0 = REngine::GetDevice().SetRenderTarget(0, m_pRenderTarget);
	RTexture* pDepth = REngine::GetDevice().SetDepthStencilBuffer(m_pDepthBuffer);

	REngine::GetDevice().SetTexture(0, m_pCommonShadowRT);
	REngine::GetDevice().SetTexture(1, m_pDoubleShadowRT);

	RTechnique tecRenderFloat = m_pShaderClearFloat->GetTechnique("CombineDoubleShadowMap");
	tecRenderFloat.Begin(0, TECH_PRESERVE_NOT);
	tecRenderFloat.BeginPass();
	RRenderHelper::RenderQuad(0, 0, m_pRenderTarget->GetInfo().nTextureWidth, m_pRenderTarget->GetInfo().nTextureHeight);
	tecRenderFloat.EndPass();
	tecRenderFloat.End();

	REngine::GetDevice().SetRenderTarget(0, pRT0);
	REngine::GetDevice().SetDepthStencilBuffer(pDepth);
	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetDepthEnable(true, true);
}

void RDepthMapShadowStrategy::GetCullPlanesForX(const RViewFrustum* pPrevFrustum, const RViewFrustum* pCurrentFrustum, float fDelta, RPlane* outPlanes)
{
	const RPlane* pPrevPlanes = pPrevFrustum->GetPlanes();
	const RPlane* pCurrentPlanes = pCurrentFrustum->GetPlanes();

	outPlanes[0] =pCurrentPlanes[RV_TOP];
	outPlanes[1] =pCurrentPlanes[RV_BOTTOM];			
	outPlanes[2] =fDelta>0 ? pCurrentPlanes[RV_LEFT] : pCurrentPlanes[RV_RIGHT];
	outPlanes[3] =fDelta>0 ? -pPrevPlanes[RV_LEFT] : -pPrevPlanes[RV_RIGHT];		
}

void RDepthMapShadowStrategy::GetCullPlanesForY(const RViewFrustum* pPrevFrustum, const RViewFrustum* pCurrentFrustum, float fDelta, RPlane* outPlanes)
{
	const RPlane* pPrevPlanes = pPrevFrustum->GetPlanes();
	const RPlane* pCurrentPlanes = pCurrentFrustum->GetPlanes();

	outPlanes[0] =pCurrentPlanes[RV_LEFT];
	outPlanes[1] =pCurrentPlanes[RV_RIGHT];
	outPlanes[2] =fDelta>0 ? pCurrentPlanes[RV_BOTTOM] : pCurrentPlanes[RV_TOP];
	outPlanes[3] =fDelta>0 ? -pPrevPlanes[RV_BOTTOM] : -pPrevPlanes[RV_TOP];
}

bool RDepthMapShadowStrategy::IsCastShadow(const RMatrix& matView, const RMatrix& matViewProj, const RSphere& boundingSphere)
{
	if(m_bUseRenderMinArea==false)
		return true;

	RVector3 vViewX(matView._11,matView._21,matView._31);	//View-space X축
	RVector3 vP0 = boundingSphere.center+vViewX*boundingSphere.radius;
	RVector3 vP1 = boundingSphere.center-vViewX*boundingSphere.radius;

	matViewProj.TransformVectHomogeneous(vP0);
	matViewProj.TransformVectHomogeneous(vP1);
	RVector3 vDelta = (vP0-vP1)*0.5f;		// [-1, 1] => [0, 1]
	
	float fRenderArea = vDelta.x*vDelta.x;

	return fRenderArea>m_fMinRenderArea;
}

void RDepthMapShadowStrategy::RenderDepthMapToScreen(int nX, int nY, int nWidth, int nHeight)
{
	if(!m_pShaderClearFloat)
		return;

	RTechnique tecRenderFloat = m_pShaderClearFloat->GetTechnique("RenderFloatRenderTarget");

	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetTexture(0, m_pRenderTarget);

	tecRenderFloat.Begin(NULL, TECH_PRESERVE_NOT);
	tecRenderFloat.BeginPass();
	RRenderHelper::RenderQuad(nX,nY,nWidth,nHeight);
	tecRenderFloat.EndPass();
	tecRenderFloat.End();

	REngine::GetDevice().SetDepthEnable(true, true);
}

}
