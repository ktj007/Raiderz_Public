#pragma once

#include "RStaticMeshRenderStrategy.h"

namespace rs3 {

class RStaticMeshGStageRenderStrategy : public RStaticMeshRenderStrategy
{
public:
	RStaticMeshGStageRenderStrategy( RStaticMeshSceneNode* pStaticMesh ) : RStaticMeshRenderStrategy(pStaticMesh) {}
	virtual ~RStaticMeshGStageRenderStrategy() {}

	virtual bool CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable );
};

}