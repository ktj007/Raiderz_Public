#pragma once

#include "RStaticMeshRenderStrategy.h"

namespace rs3 {

class RStaticMeshMStageRenderStrategy : public RStaticMeshRenderStrategy
{
public:
	RStaticMeshMStageRenderStrategy( RStaticMeshSceneNode* pStaticMesh ) : RStaticMeshRenderStrategy(pStaticMesh) {}
	virtual ~RStaticMeshMStageRenderStrategy() {}

	virtual bool CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable );
};

}