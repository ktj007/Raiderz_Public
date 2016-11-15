#pragma once

#include "RPassRenderer.h"

namespace rs3 {

class RTerrainShader;

class RTerrainPassRenderer : public RPassRenderer
{
public:
	RTerrainPassRenderer(void);
	~RTerrainPassRenderer(void);

	DECLARE_PASS_RENDERER_ID( RRP_TERRAIN );

	virtual void		RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy);
	virtual void		RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod);
	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod);
	virtual void		RenderZStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod);

	RTerrainShader*	GetTerrainShader()	{ return m_pTerrainShader;}

protected:

	virtual void		Init();
	virtual void		Destroy();

	virtual bool		CullByPlanes(RCameraSceneNode* pCamera,const RSceneNode* pNode, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar) override;
	virtual	bool		CullByViewFrustum(const RSceneNode* pNode,RCameraSceneNode* pCamera);

	RTerrainShader*		m_pTerrainShader;		
};

}
