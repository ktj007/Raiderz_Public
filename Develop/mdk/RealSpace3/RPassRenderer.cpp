#include "stdafx.h"
#include "RPassRenderer.h"

namespace rs3 {

bool RPassRenderer::CullByPlanes(RCameraSceneNode* pCamera, const RSceneNode* pNode, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar )
{
	const RBox& aabb = pNode->GetAABB();

	if( pPlaneNear && !aabb.isInPlane( *pPlaneNear ) )
		return false;

	if( pPlaneFar && !aabb.isInPlane( *pPlaneFar ) )
		return false;

	return CullDefault( aabb, cullDescs );
}

bool RPassRenderer::CullDefault( const RBox& aabb, const vector<RCULLDESC>& cullDescs )
{
	size_t cullDescsSize = cullDescs.size();
	if( cullDescsSize )
	{
		for( size_t i = 0; i < cullDescsSize; ++i )
		{
			if( aabb.isInPlanes( cullDescs[i].pPlanes, cullDescs[i].nPlanes ) )
				return true;
		}

		return false;
	}

	return true;
}

bool RPassRenderer::CullByViewFrustum( const RSceneNode* pNode,RCameraSceneNode* pCamera )
{
	return true;
}


void RPassRenderer::RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD renderMethod)
{

}


void RPassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	REngine::GetResourceProfiler().StartMeshProfiler();

	SCENE_SET::iterator	iter;
	for( iter = pRenderingList->begin() ; iter != pRenderingList->end() ; ++iter)
	{
		RSceneNode *pNode = (*iter);
		if( !pNode->GetVisible() || !pNode->GetUsable() || pNode->GetOcclued() )	continue;
		pNode->RenderGStage( pCurrentCamera, -1, renderMethod);
	}

	RPFC_B( "RPassRenderer G pass Restore RenderState");

	// 렌더 스테이트 복구
	REngine::GetDevice().RestoreForRenderInstancing();
	RPFC_E;

	RPFC_GPU_CALL;
}



void RPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	REngine::GetResourceProfiler().StartMeshProfiler();

	RPFC_B( "RPassRenderer M pass Render Loop");
	SCENE_SET::iterator	iter;
	for( iter = pRenderingList->begin() ; iter != pRenderingList->end() ; ++iter)
	{
		RSceneNode *pNode = (*iter);
		if( !pNode->GetVisible() || !pNode->GetUsable() || pNode->GetOcclued() )	continue;
		pNode->Render( pCurrentCamera, -1, renderMethod);
	}
	RPFC_E;

	RPFC_B( "RPassRenderer M pass Restore RenderState");
	
	// 렌더 스테이트 복구
	REngine::GetDevice().RestoreForRenderInstancing();
	RPFC_E;

	RPFC_GPU_CALL;
}


void RPassRenderer::RenderZStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	return;
}


void RPassRenderer::RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy )
{
	return;
}


}