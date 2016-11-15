#include "StdAfx.h"
#include "RAlphaPassRenderer.h"
#include "RCameraSceneNode.h"
#include "RShaderCompositeManager.h"

// 디버그용
#include "RRenderHelper.h"
#include "RFont.h"
#include "RSUtil.h"

namespace rs3 {

RAlphaPassRenderer::RAlphaPassRenderer(void)
{
}

RAlphaPassRenderer::~RAlphaPassRenderer(void)
{
}

void RAlphaPassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{

}

void RAlphaPassRenderer::RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	const RCameraSceneNode* pCamera = pCurrentCamera;

	RRenderChain<RActorNodeKey>*		pAlphaActorNodeChain = pCurrentCamera->GetViewFrustumCullingMgr()->GetAlphaActorNodeChain();
	RRenderChainIterator<RActorNodeKey> ChainIter;
	RSceneNode* pSceneNode;

	typedef struct tagRK
	{
		RSceneNode* pNode;
		int iSubMaterial;
	}RK;

	typedef multimap<float, RK> DISTANCENODEMAP;
	DISTANCENODEMAP m_alphaNodes;	// 알파소팅이 필요한 노드

	for(ChainIter = pAlphaActorNodeChain->Begin(); ChainIter != pAlphaActorNodeChain->End() ; ChainIter.Next() )
	{
		SCENE_SET* pSet = ChainIter.GetSceneSet();
		if( pSet->empty() )	continue;
		
		RActorNodeKey key = ChainIter.GetKey();

		SCENE_SET::iterator ActorNodeIter;
		for(ActorNodeIter = pSet->begin() ; ActorNodeIter != pSet->end() ; ActorNodeIter++)
		{
			pSceneNode =(RSceneNode*) (*ActorNodeIter);
			RK rk;
			rk.pNode = pSceneNode;
			rk.iSubMaterial = key.m_nSubMaterial;

			if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
			RVector worldPosition(pSceneNode->GetWorldPosition());
			float fDistance = pCurrentCamera->GetDistanceTo(worldPosition);
			m_alphaNodes.insert( DISTANCENODEMAP::value_type(fDistance,rk) );

		}
	}

	SCENE_SET* pAlpha2PassActorSet = pCurrentCamera->GetViewFrustumCullingMgr()->GetAlpha2PassActor();
	for( SCENE_SET::iterator	Alpha2PassActorIter = pAlpha2PassActorSet->begin(); Alpha2PassActorIter != pAlpha2PassActorSet->end(); Alpha2PassActorIter++)
	{
		pSceneNode =(RSceneNode*) (*Alpha2PassActorIter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;

		RK rk = { pSceneNode,-1};
		RVector worldPosition(pSceneNode->GetWorldPosition());
		float fDistance = pCurrentCamera->GetDistanceTo(worldPosition);
		m_alphaNodes.insert( DISTANCENODEMAP::value_type(fDistance,rk) );
	}

	//bool bFogEnable = pCurrentCamera->GetFogEnable();

	
	//Timer Reset & Start
	REngine::GetResourceProfiler().StartMeshProfiler();


	DISTANCENODEMAP::reverse_iterator itr;
	for( itr = m_alphaNodes.rbegin(); itr!= m_alphaNodes.rend(); itr++)
	{
		RK rk = itr->second;
		RSceneNode* pNode	= rk.pNode;
		int iSubMaterial	= rk.iSubMaterial;
		pNode->RenderAlpha( pCurrentCamera,iSubMaterial, renderMethod);
	}	
}

void RAlphaPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	RenderOnPass( pCurrentCamera, pRenderingList, renderMethod);

	RPFC_GPU_CALL;
}

void RAlphaPassRenderer::RenderDebugInfo(RCameraSceneNode* pCamera, RFont* pFont)
{
	SCENE_SET* pCulledList = pCamera->GetViewFrustumCullingMgr()->GetCulledList(RRP_ALPHA);
	SCENE_SET::iterator	iter;

	typedef multimap<float, RSceneNode*> DISTANCENODEMAP;
	DISTANCENODEMAP m_alphaNodes;

	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; iter++)
	{
		RSceneNode *pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;
		RVector worldPosition(pSceneNode->GetWorldPosition());
		float fDistance = pCamera->GetDistanceTo(worldPosition);
		m_alphaNodes.insert( DISTANCENODEMAP::value_type(fDistance,pSceneNode) );
	}


	int nOrder = 0;
	DISTANCENODEMAP::reverse_iterator itr;
	for( itr = m_alphaNodes.rbegin(); itr!= m_alphaNodes.rend(); itr++,nOrder++)
	{
		RSceneNode* pNode = itr->second;
		RVector vecPosition = pNode->GetWorldPosition();

		if(pCamera->GetViewFrustum().Test(vecPosition))
		{
			DWORD dwColor = ( nOrder%2==0 ) ? 0xffff00ff : 0xffffff00;
			int textOffset = ( nOrder%2==0 ) ? -5 : 5;
			RMatrix matWorld;
			matWorld.SetTranslationMatrix( vecPosition);

			RRenderHelper::SetRenderState();
			RRenderHelper::RenderSphere(matWorld, 10.f, dwColor);

			vec2 screen_pos;
			GetScreenFromWorld(&screen_pos,&vecPosition,
				&REngine::GetDevice().GetViewport(), 
				&RMatrix::IDENTITY, 
				&pCamera->GetViewMatrix(), 
				&pCamera->GetProjectionMatrix());

			char szBuffer[256];
			sprintf(szBuffer,"%d , %s , %3.3f ", nOrder, pNode->GetNodeName().c_str(), itr->first );
			pFont->BeginFont();
			pFont->DrawText_UNICODE((int)screen_pos.x, (int)screen_pos.y + textOffset, MLocale::ConvAnsiToUTF16(szBuffer).c_str(), -1, dwColor);
			pFont->EndFont();
		}
	}
}

}