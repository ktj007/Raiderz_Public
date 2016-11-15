#include "StdAfx.h"
#include "REdgePassRender.h"
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

REdgePassRenderer::REdgePassRenderer(void) 
: RActorPassRenderer()
{
}


void REdgePassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RenderTypeOnPass(pCurrentCamera, pRenderingList, false, RRP_EDGE);
}

void REdgePassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) {}

void REdgePassRenderer::UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor)
{

	rscComponents |= RSC_TRANSFORM_CONSTANT;
//	rscComponents |= RSC_GSTAGE_EDGE;
	rscComponents |= RSC_NORMAL;
	rscComponents |= RSC_PPL_EDGE_COLOR;

	
	rscComponents &= ~RSC_PPL_LIGHTING;
	rscComponents &= ~RSC_TRANSFORM;
	rscComponents &= ~RSC_TRANSFORM_VBLUR;
	rscComponents &= ~RSC_PPL_VBLUR;
	rscComponents |= RSC_TRANSFORM_EDGE;


	rscComponents &= ~RSC_PPL_CLIP_ALPHA;

	rscComponents |= RSC_M_CONSTANT;
//	rscComponents |= RSC_PPL_VISIBLITY;
//	rscComponents |= RSC_POSITION;
//	rscComponents &= ~RSC_TRANSFORM
}

}