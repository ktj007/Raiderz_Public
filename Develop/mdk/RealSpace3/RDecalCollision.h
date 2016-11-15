#pragma once

#include <vector>
#include <string>
#include "MBox.h"
#include "RVType.h"
#include "RUtil.h"
#include "RCollision.h"

namespace rs3 {

class RCollisionTree;
class RTexture;
class RCollisionTree;
class RDecalCollision;

#define DECALFLAG_TERRAIN		0x001
//#define DECALFLAG_STATICMESH	0x002	// 없어졌음
#define DECALFLAG_ACTOR			0x008	// 캐릭터를 제외한 Actor (cl2에 포함되는 데이터)

class RDecalCollision : public RCollision
{
protected:
	virtual bool			IsCollidable(const MBox &box);
	virtual bool			CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute = 0);
	virtual bool			CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly);	// 이 폴리곤과 접하는가?

	virtual void			GetBoundingBox(MBox &outBox);
	virtual float			GetWalkableDistance(const RCollisionPolygon &collisionPolygon);
	virtual MVector3		GetContactPoint(const MVector3& vPos, const MVector3& vNorm);

public:
	RDecalCollision();
	~RDecalCollision();

	RBoundingBox			m_aabb;
	std::vector<RVt_pos>*	m_vecVertex;

	void					SetAABB( const RBoundingBox& rbb);
	void					SetVertex(std::vector<RVt_pos>* vecVertex);
	void					CalcBoundingBox();		// 이번 움직임 전체의 바운딩박스를 구한다
};

} //namespace rs3
