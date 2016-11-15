#pragma once

#include "RPassRenderer.h"

namespace rs3 {

class RStaticMeshPassRenderer : public RPassRenderer
{
public:
	RStaticMeshPassRenderer(void);
	~RStaticMeshPassRenderer(void);

	DECLARE_PASS_RENDERER_ID( RRP_STATICMESH );

protected:
	virtual bool		CullByPlanes(RCameraSceneNode* pCamera,const RSceneNode* pNode, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar) override;
	virtual	bool		CullByViewFrustum(const RSceneNode* pNode,RCameraSceneNode* pCamera);

	virtual void		RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy);
	virtual void		RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

};

}
