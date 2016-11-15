#include "StdAfx.h"
#include "RDecalPolygonPassRenderer.h"
#include "RCameraSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RShaderComposite.h"

namespace rs3 {

RDecalPolygonPassRenderer::RDecalPolygonPassRenderer(void)
{
}

RDecalPolygonPassRenderer::~RDecalPolygonPassRenderer(void)
{
}

void RDecalPolygonPassRenderer::RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD renderMethod)
{
	typedef multimap<float, RSceneNode*> DISTANCENODEMAP;

	DISTANCENODEMAP m_sortingNodes;

	SCENE_SET::iterator	iter;
	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RVector worldPosition(pSceneNode->GetWorldPosition());
		float fDistance = ((RCameraSceneNode*)pCurrentCamera)->GetDistanceTo(worldPosition);
		m_sortingNodes.insert( DISTANCENODEMAP::value_type(fDistance,pSceneNode) );
	}


	//////////////////////////////////////////////////////////////////////////
	// State
	RDevice* pDev = REngine::GetDevicePtr();
	_ASSERT( pDev );

	// Set RenderState
	{
		pDev->SetFvF(RVt_pos_clr_tex1::FVF);

		pDev->SetDepthEnable(true, false);
		pDev->SetCullMode( RCULL_NONE );
		pDev->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		pDev->SetDepthBias(-1.f / 65535.f);//, m_fSlopeBias);
	}


	RShaderCompositeManager::m_cVisiblity.SetFloat(1.f);
	RShaderCompositeManager::SetTransformConstant(RMatrix::IDENTITY, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());
	//////////////////////////////////////////////////////////////////////////

	

	//////////////////////////////////////////////////////////////////////////
	// Render
	DISTANCENODEMAP::reverse_iterator itr;
	for( itr = m_sortingNodes.rbegin(); itr!= m_sortingNodes.rend(); ++itr)
	{
		RSceneNode* pNode = itr->second;
		pNode->Render( pCurrentCamera, -1, RENDERING_NULL);	/// 어떠한 랜더링 모드이든 상관이 없이 그리는 방법은 일괄적.
	}
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//Restore Render State
	pDev->SetDepthEnable(true, true);
	pDev->SetCullMode(RCULL_CW);
	pDev->SetBlending(RBLEND_NONE);
	pDev->SetDepthBias(0);
	pDev->SetTexture(0, R_NONE);
	//////////////////////////////////////////////////////////////////////////
}

void RDecalPolygonPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	REngine::GetResourceProfiler().StartMeshProfiler();

	RenderOnPass( pCurrentCamera, pRenderingList, renderMethod);
}
}