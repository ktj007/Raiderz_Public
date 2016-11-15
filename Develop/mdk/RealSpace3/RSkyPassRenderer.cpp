#include "StdAfx.h"
#include "RSkyPassRenderer.h"
#include "RSkyManager.h"

namespace rs3 {

RSkyPassRenderer::RSkyPassRenderer()
{
}

RSkyPassRenderer::~RSkyPassRenderer()
{
}

void RSkyPassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	Render( pCurrentCamera, pRenderingList, renderMethod, m_GStageRenderStrategy );

	RPFC_GPU_CALL;
}

void RSkyPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	Render( pCurrentCamera, pRenderingList, renderMethod, m_MStageRenderStrategy );

	RPFC_GPU_CALL;
}

void RSkyPassRenderer::Render( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod, const RSkyRenderStrategy& renderStrategy )
{
	RPFC_THISFUNC;
	for( int i = 0; i < RSkyManager::eRENDERORDER_NUM; ++i )
	{
		SCENE_SET::iterator	iter;
		for( iter = pRenderingList->begin() ; iter != pRenderingList->end() ; ++iter)
		{
			RSceneNode *pNode = (*iter);
			if( !pNode->GetVisible() || !pNode->GetUsable() || pNode->GetOcclued() )
				continue;

			RSkyManager* pSkyManager = MDynamicCast( RSkyManager, pNode );
			_ASSERT( pSkyManager );

			if( pSkyManager->GetRenderOrder() == i )
				renderStrategy.Render( pNode, renderMethod, pCurrentCamera );
		}
	}
}

void RSkyPassRenderer::RSkyGStageRenderStrategy::Render( RSceneNode* pNode, RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCamera ) const
{
	RPFC_THISFUNC;
	pNode->RenderGStage( pCamera, -1, renderMethod );
}

void RSkyPassRenderer::RSkyMStageRenderStrategy::Render( RSceneNode* pNode, RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCamera ) const
{
	RPFC_THISFUNC;
	pNode->Render( pCamera, -1, renderMethod );
}

}