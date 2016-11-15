#pragma once

#include "RSceneNode.h"

namespace rs3 {

class RCollisionTreeBuilder;

class RS_API RSceneNodePolygonCollector_Collision : public RSceneNodePolygonCollector
{
public:
	RSceneNodePolygonCollector_Collision(RCollisionTreeBuilder* pTreeBuilder);
	virtual ~RSceneNodePolygonCollector_Collision();

	virtual void HandleError(const std::string& strError);
	virtual DWORD GetZoneMaterialID(const std::string &strMaterialName);
	virtual void OnAddPolygon( const RVector& p1, const RVector& p2, const RVector& p3, WORD wNodeFlag, WORD wUserFlag, DWORD dwMaterial );

	RCollisionTreeBuilder*		m_pTreeBuilder;
};

}