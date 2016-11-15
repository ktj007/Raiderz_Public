#include "stdafx.h"
#include "RAdditivePassRenderer.h"
#include "RSceneManager.h"
#include "RActor.h"
#include "RMeshNode.h"
#include "RMaterial.h"
#include "RActorNode.h"
#include "RCameraSceneNode.h"
#include "RProfileInfo.h"
#include "RShaderCompositeManager.h"
#include "RDirectionalLightSceneNode.h"
#include "mmsystem.h"
#include "RUtil.h"
#include "RActorNodeRenderStrategy.h"


namespace rs3 {

RAdditivePassRenderer::RAdditivePassRenderer() : RActorPassRenderer()
{
}

void RAdditivePassRenderer::Init()
{
	RActorPassRenderer::Init();
}

void RAdditivePassRenderer::Destroy()
{
	RActorPassRenderer::Destroy();
}

void RAdditivePassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	// 	TIMEPROFILE_THIS_FUNCTION;
	RPFC_THISFUNC;

	RRenderChain<RActorNodeKey>* pActorNodeRenderChain = pCurrentCamera->GetViewFrustumCullingMgr()->GetAlphaAdditiveActorNodeChain();

	RActorNodeRenderStrategyMStageInferred RenderStrategy;
	RActorPassRenderer::RenderOnPass( pCurrentCamera, &RenderStrategy, pActorNodeRenderChain);

	RPFC_GPU_CALL;
}

void RAdditivePassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
}

void RAdditivePassRenderer::RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy )
{
}

}
