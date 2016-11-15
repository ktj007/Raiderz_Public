#pragma once

#include <map>
#include "RPassRenderer.h"

namespace rs3 {

class RDecalPolygonPassRenderer : public RPassRenderer
{
public:
	RDecalPolygonPassRenderer(void);
	virtual ~RDecalPolygonPassRenderer(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_DECAL_POLYGON );

	virtual void RenderOnPass( RCameraSceneNode* pCurrentCamera,SCENE_SET* pRenderingList, RRENDERINGMETHOD eMothod) override;
	virtual void RenderMStageOnPass( RCameraSceneNode* pCurrentCamera,SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

};

}