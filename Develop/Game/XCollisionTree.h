#pragma once

#include "XGlobal.h"
#include "RTerrain.h"
#include "RCollisionTree.h"

typedef RCollisionTriangle CollisionAttribute;

using namespace rs3;
namespace rs3 {
	class RActorNode;
	class RPointLightSceneNode;
}

/// 월드의 충돌관련
class XCollisionTree : public RCollisionTree
{
	RTerrain*				m_pTerrain;

	/// collision 이 this에 잠재적으로 충돌가능한 폴리곤을 모읍니다
	virtual void GetCollidablePolygons(RCollision &collision, DWORD dwAttribute);

public:
	XCollisionTree();
	void SetTerrain(RTerrain* pTerrain)	{ m_pTerrain = pTerrain;}

	/// 지형을 구성하는 Triangle의 개수
	int GetTerrainPolygonCount();

	/// 해당 인덱스의 Triangle(CCW)을 가져온다. (0, 0, bottom), (0, 0, up), (1, 0, bottom) ... 순서로 인덱스 구성
	void GetTerrainTriangle(int index, RVector &v0, RVector &v1, RVector &v2);

	virtual bool Pick(RCollisionPickInfo& pickInfo, DWORD dwAttributeFlag, bool bNoCull = false);	// picking
};
