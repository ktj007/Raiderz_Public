#pragma once

#include "RPassRenderer.h"
#include "RCameraSceneNode.h"
#include "RPostEffectDistortion.h"


namespace rs3 {

class RDistortionPassRenderer : public RPassRenderer
{
public:
	DECLARE_PASS_RENDERER_ID( RRP_DISTORTION );

	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
};

}