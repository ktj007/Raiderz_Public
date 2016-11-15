#pragma once

#include <vector>
#include "VR_EtcCollision.h"
#include "VR_EtcDecal.h"
#include "RDecalProjectionSceneNode.h"
#include "RDecalPolygonSceneNode.h"

namespace rs3 {

class RToolDecal
{
public:
	RToolDecal(void);
	~RToolDecal(void);

	void Create(RToolSceneManager* pToolSceneManager, CRollupEtcDecal* pRollupDecal, RVector MousePosVec, CRollupEtcCollision* pCollision = NULL);
	void Destroy();

	std::vector<RDecalProjectionSceneNode*>	m_vecDecal_Projection;
	RDecalPolygonSceneNode*					m_pDecal_Polygon;

};

}
