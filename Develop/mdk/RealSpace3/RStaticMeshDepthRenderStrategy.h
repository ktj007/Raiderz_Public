#pragma once

#include "RStaticMeshRenderStrategy.h"

namespace rs3 {

class RStaticMeshDepthRenderStrategy : public RStaticMeshRenderStrategy
{
public:
	RStaticMeshDepthRenderStrategy( RStaticMeshSceneNode* pStaticMesh ) : RStaticMeshRenderStrategy(pStaticMesh) {}
	virtual ~RStaticMeshDepthRenderStrategy() {}

	virtual bool CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable );
};

}