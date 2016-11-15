#pragma once


#include "RActorPassRenderer.h"



namespace rs3 {


class RS_API RAccumMotionBlurPassRenderer : public RActorPassRenderer
{
public:
	RAccumMotionBlurPassRenderer();
	virtual ~RAccumMotionBlurPassRenderer() {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_ABLUR );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// update / rendering
	//
	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

protected:
	virtual void		UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor) override;
};

}