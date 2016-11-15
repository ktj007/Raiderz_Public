#include "StdAfx.h"
#include "RPixelMotionBlurPassRenderer.h"
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

RPixelMotionBlurPassRenderer::RPixelMotionBlurPassRenderer(void) 
: RActorPassRenderer()
{
}


void RPixelMotionBlurPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	RenderTypeOnPass(pCurrentCamera, pRenderingList, false, RRP_VBLUR);
}

void RPixelMotionBlurPassRenderer::UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor)
{
	RSCID rscComponentsForBlur = RSC_POSITION | RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM_VBLUR | RSC_NORMAL | RSC_PPL_VELOCITY;

	rscComponentsForBlur |= (rscComponents & RSC_PHYSIQUE);

	rscComponents = rscComponentsForBlur;
}


}