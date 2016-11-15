#include "StdAfx.h"
#include "RVertexMotionBlurPassRenderer.h"
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

RVertexMotionBlurPassRenderer::RVertexMotionBlurPassRenderer(void) 
: RActorPassRenderer()
{
}


void RVertexMotionBlurPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RenderTypeOnPass(pCurrentCamera, pRenderingList, true, RRP_VBLUR);
}

void RVertexMotionBlurPassRenderer::UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor)
{
	if( (rscComponents & RSC_PHYSIQUE).IsNonZero() )
	{
		rscComponents &= ~RSC_PHYSIQUE;
		rscComponents |= RSC_PHYSIQUE_VBLUR;
	}

	rscComponents |= RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM_VBLUR;
	rscComponents |= RSC_PPL_VBLUR;
	rscComponents |= RSC_NORMAL;

	rscComponents &= ~RSC_TRANSFORM;
	rscComponents &= ~RSC_PPL_CLIP_ALPHA;	// 자연스레 꼬리가 옅어져야 하므로 버텍스 모션 블러의 경우는 알파 테스팅을 하지 않는다. 
}

}