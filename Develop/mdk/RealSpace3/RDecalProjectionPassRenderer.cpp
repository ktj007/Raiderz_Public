#include "StdAfx.h"
#include "RDecalProjectionPassRenderer.h"
#include "RCameraSceneNode.h"
#include "RShaderCompositeManager.h"

namespace rs3 {

RDecalProjectionPassRenderer::RDecalProjectionPassRenderer(void)
{
}

RDecalProjectionPassRenderer::~RDecalProjectionPassRenderer(void)
{
}

void RDecalProjectionPassRenderer::RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
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

	// RenderState
	pDev->SetDepthEnable(true, false);
	pDev->SetCullMode( RCULL_NONE );
	pDev->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);

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
	//////////////////////////////////////////////////////////////////////////
}

void RDecalProjectionPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	REngine::GetResourceProfiler().StartMeshProfiler();

	RenderOnPass( pCurrentCamera, pRenderingList, renderMethod);
}
}