#pragma once


#include "RActorPassRenderer.h"



namespace rs3 {


class RS_API REdgePassRenderer : public RActorPassRenderer
{
public:
	REdgePassRenderer();
	virtual ~REdgePassRenderer() {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_EDGE );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// update / rendering
	//

	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void		RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

protected:
	virtual void		UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor) override;
};

}