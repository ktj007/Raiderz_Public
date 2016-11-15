#include "StdAfx.h"
#include "RTerrainPassRenderer.h"
#include "RTerrain.h"
#include "RTerrainShader.h"
#include "RShaderCompositeManager.h"

namespace rs3 {


RTerrainPassRenderer::RTerrainPassRenderer(void)
: m_pTerrainShader(NULL)
{
	m_pTerrainShader = new RTerrainShader();
}

RTerrainPassRenderer::~RTerrainPassRenderer(void)
{
	SAFE_DELETE( m_pTerrainShader);
}

void RTerrainPassRenderer::Init()
{
	if( m_pTerrainShader)
		m_pTerrainShader->Init();
}

void RTerrainPassRenderer::Destroy()
{
	if( m_pTerrainShader)
		m_pTerrainShader->Destroy();
}

bool RTerrainPassRenderer::CullByPlanes(RCameraSceneNode* pCamera,const RSceneNode* pNode, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	bool bRender = RPassRenderer::CullByPlanes(pCamera,pNode, cullDescs, pPlaneNear, pPlaneFar);
	if(bRender)
	{	
		RTerrain* pTerrain = MDynamicCast(RTerrain, pNode);
		if(pTerrain)
			pTerrain->Cull(pCamera,cullDescs, pPlaneNear, pPlaneFar);
	}

	return bRender;
}

bool RTerrainPassRenderer::CullByViewFrustum( const RSceneNode* pNode,RCameraSceneNode* pCamera )
{
	RPFC_THISFUNC;
	RTerrain* pTerrain = MDynamicCast(RTerrain, pNode);
	if(pTerrain)
	{
		const RPlane *pFrustumPlanes = pCamera->GetViewFrustum().GetPlanes();
		const RPlane *pPlaneNear	= &pFrustumPlanes[RV_NEAR];
		const RPlane *pPlaneFar		= &pFrustumPlanes[RV_FAR];
		RCULLDESC cullDesc;
		cullDesc.pPlanes = const_cast<RPlane*>(pFrustumPlanes);
		cullDesc.nPlanes = 4;
		vector<RCULLDESC> cullDescs;
		cullDescs.push_back( cullDesc );

		pTerrain->Cull(pCamera,cullDescs, pPlaneNear, pPlaneFar);
	}
	return true;
}


void RTerrainPassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	//Timer Reset & Start
	REngine::GetResourceProfiler().StartMeshProfiler();

	if (pCurrentCamera->GetPassIgnore(RRP_TERRAIN)) return;

	SCENE_SET::iterator iter;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RTerrain* pTerrain = MDynamicCast(RTerrain, pSceneNode);
		if(pTerrain)
		{
			pTerrain->RenderGStage( renderMethod, pCurrentCamera);
		}
	}

	RPFC_GPU_CALL;
}

void RTerrainPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	// 디퍼드에서는 M 스테이지따위 전재치 않아
//	if( renderMethod == RENDERING_DEFERRED)
//		return;


	if (pCurrentCamera->GetPassIgnore(RRP_TERRAIN)) return;

	SCENE_SET::iterator iter;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RTerrain* pTerrain = MDynamicCast(RTerrain, pSceneNode);
		if(pTerrain)
		{
			pTerrain->RenderMStage( pCurrentCamera, renderMethod );
		}
	}

	RPFC_GPU_CALL;
}


void RTerrainPassRenderer::RenderZStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	//Timer Reset & Start
	REngine::GetResourceProfiler().StartMeshProfiler();

	if (pCurrentCamera->GetPassIgnore(RRP_TERRAIN)) return;

	SCENE_SET::iterator iter;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RTerrain* pTerrain = MDynamicCast(RTerrain, pSceneNode);
		if(pTerrain)
		{
			pTerrain->RenderZStage( renderMethod, pCurrentCamera);
		}
	}

	RPFC_GPU_CALL;
}



void RTerrainPassRenderer::RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy )
{
	RShaderCompositeManager::m_cShadowValue.SetFloat(1);

	SCENE_SET::iterator	iter;
	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; ++iter)
	{
		RSceneNode *pSceneNode = (*iter);
		RTerrain* pTerrain = MDynamicCast(RTerrain, pSceneNode);
		if(pTerrain)
		{
			pTerrain->OnRenderShadowMap(pCurrentCamera, pShadowStrategy);
		}
	}
}

}