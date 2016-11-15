#include "StdAfx.h"
#include "RPostEffectEdgePassRender.h"
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

	RPostEffectEdgePassRender::RPostEffectEdgePassRender(void) 
		: RActorPassRenderer()
	{
	}


	void RPostEffectEdgePassRender::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
	{
	//	REngine::GetResourceProfiler().StartMeshProfiler();
		RenderTypeOnPass(pCurrentCamera, pRenderingList, false, RRP_POSTEFFECT_EDGE);
	}

	void RPostEffectEdgePassRender::UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor)
	{
 		rscComponents |= RSC_TRANSFORM_CONSTANT;
		rscComponents |= RSC_TRANSFORM;
		rscComponents &= ~RSC_PPL_FOG;
 		rscComponents &= ~RSC_PPL_LIGHTING;
 		rscComponents &= ~RSC_TRANSFORM_VBLUR;
 		rscComponents |= RSC_PPL_CLIP_ALPHA;
		rscComponents &= ~RSC_PPL_CLIP_ALPHA;
		rscComponents &= ~RSC_INSTANCING;
		rscComponents &= ~RSC_INSTANCE_PARTSCOLOR;
		rscComponents |= RSC_PPL_PE_EDGE_MASK;
	}
}