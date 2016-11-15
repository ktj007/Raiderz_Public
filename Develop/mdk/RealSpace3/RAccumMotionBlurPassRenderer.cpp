#include "StdAfx.h"
#include "RAccumMotionBlurPassRenderer.h"
#include "RSceneManager.h"

#include "RActor.h"
#include "RMeshNode.h"
#include "RMaterial.h"

#include "RCameraSceneNode.h"
#include "RProfileInfo.h"

#include "RShaderCompositeManager.h"
#include "RDirectionalLightSceneNode.h"
#include "mmsystem.h"

namespace rs3 {

RAccumMotionBlurPassRenderer::RAccumMotionBlurPassRenderer(void) 
: RActorPassRenderer()
{
}


void RAccumMotionBlurPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	REngine::GetResourceProfiler().StartMeshProfiler();
	RenderTypeOnPass(pCurrentCamera, pRenderingList, false, RRP_VBLUR);
}

void RAccumMotionBlurPassRenderer::UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor)
{
	RSCID rscComponentsForBlur = RSC_POSITION | RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM | RSC_PPL_WHITE;

	rscComponentsForBlur |= (rscComponents & RSC_PHYSIQUE);

	rscComponents = rscComponentsForBlur;
}

}