#include "StdAfx.h"
#include "RStaticMeshPassRenderer.h"
#include "RStaticMeshSceneNode.h"

namespace rs3 {

RStaticMeshPassRenderer::RStaticMeshPassRenderer(void)
{
}

RStaticMeshPassRenderer::~RStaticMeshPassRenderer(void)
{
}

bool RStaticMeshPassRenderer::CullByPlanes(RCameraSceneNode* pCamera,const RSceneNode* pNode, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	bool bRender = RPassRenderer::CullByPlanes(pCamera,pNode, cullDescs, pPlaneNear, pPlaneFar);
	if(bRender)
	{
		RStaticMeshSceneNode* pStaticMesh = MDynamicCast(RStaticMeshSceneNode, pNode);
		if(pStaticMesh)
			pStaticMesh->Cull(pCamera,cullDescs, pPlaneNear, pPlaneFar);
	}
	return bRender;
}

bool RStaticMeshPassRenderer::CullByViewFrustum( const RSceneNode* pNode,RCameraSceneNode* pCamera )
{
	RPFC_THISFUNC;

	RStaticMeshSceneNode* pStaticMesh = MDynamicCast(RStaticMeshSceneNode, pNode);
	if(pStaticMesh)
	{
		const RPlane *pFrustumPlanes = pCamera->GetViewFrustum().GetPlanes();
		const RPlane *pPlaneNear	= &pFrustumPlanes[RV_NEAR];
		const RPlane *pPlaneFar		= &pFrustumPlanes[RV_FAR];
		RCULLDESC cullDesc;
		cullDesc.pPlanes = const_cast<RPlane*>(pFrustumPlanes);
		cullDesc.nPlanes = 4;
		vector<RCULLDESC> cullDescs;
		cullDescs.push_back( cullDesc );

		pStaticMesh->Cull(pCamera,cullDescs, pPlaneNear, pPlaneFar);
	}
	return true;
}

void RStaticMeshPassRenderer::RenderDepthOnPass(  RCameraSceneNode* pCurrentCamera,SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy )
{
	if (((RCameraSceneNode*)pCurrentCamera)->GetPassIgnore(RRP_STATICMESH)) return;

	SCENE_SET::iterator	iter;
	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; ++iter)
	{
		RSceneNode *pSceneNode = (*iter);
		RStaticMeshSceneNode* pStaticMesh = MDynamicCast(RStaticMeshSceneNode, pSceneNode);
		if(pStaticMesh)
		{
			pStaticMesh->OnRenderShadowMap(pCurrentCamera, pShadowStrategy);
		}
	}
}


void RStaticMeshPassRenderer::RenderGStageOnPass(  RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	if (((RCameraSceneNode*)pCurrentCamera)->GetPassIgnore(RRP_STATICMESH)) return;

	SCENE_SET::iterator iter;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end(); ++iter)
	{
		RSceneNode* pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RStaticMeshSceneNode* pStaticMesh = MDynamicCast(RStaticMeshSceneNode, pSceneNode);
		if(pStaticMesh)
		{
			pStaticMesh->RenderGStage( pCurrentCamera);
		}
	}
}

void RStaticMeshPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	if (((RCameraSceneNode*)pCurrentCamera)->GetPassIgnore(RRP_STATICMESH)) return;

	SCENE_SET::iterator iter;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end(); ++iter)
	{
		RSceneNode* pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RStaticMeshSceneNode* pStaticMesh = MDynamicCast(RStaticMeshSceneNode, pSceneNode);
		if(pStaticMesh)
		{
			pStaticMesh->RenderMStage( pCurrentCamera);
		}
	}
}


}