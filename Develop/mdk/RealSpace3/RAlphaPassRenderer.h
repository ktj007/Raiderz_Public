#pragma once

#include <map>
#include "RPassRenderer.h"

namespace rs3 {

class RAlphaPassRenderer : public RPassRenderer
{
public:
	RAlphaPassRenderer(void);
	virtual ~RAlphaPassRenderer(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_ALPHA );


	virtual void RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD eMothod) override;
	virtual void RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;					

	void RenderDebugInfo(RCameraSceneNode* pCamera, RFont* pFont);
};

}