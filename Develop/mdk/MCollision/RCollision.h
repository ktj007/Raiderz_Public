#pragma once

#include "RLib.h"
#include "MMath.h"

namespace rs3 {

const float SPHERE_DEFAULT_RADIUS		=	1.f;

class RCollisionTree;
struct RCollisionTriangle;

struct RCollisionPolygon {
	RCollisionPolygon(const MVector3 &v0,const MVector3 &v1,const MVector3 &v2,const MVector3 &_nearestPoint, const MVector3 &_normal, int _nDebugFlag, DWORD dwAttribute) 
		: nearestPoint(_nearestPoint), normal(_normal), nDebugFlag(_nDebugFlag), dwPolygonAttribute(dwAttribute)
		
		{ v[0] = v0; v[1] = v1; v[2] = v2; }
	
	MVector3 v[3];
	MVector3 nearestPoint;
	MVector3 normal;

	int nDebugFlag;
	DWORD dwPolygonAttribute;
};

// 디버그용 충돌 폴리곤 출력 버텍스
struct RColVertex {
	MVector3 pos;
	DWORD	color;
};

class RCollision
{
protected:
	RCollisionTree*	m_pTargetCollisionTree;	// 충돌체크 대상 ( 월드 )
	MVector3	m_source;
	MVector3	m_destination;		// 도착하고 싶은곳
	MBox		m_bbMovement;		// 움직이는 전체의 바운딩박스

	vector<RCollisionPolygon> m_colPolygons;

private:
	bool CheckAllowedHeight(const MVector3& v1, const MVector3& v2, const MVector3& v3, const MVector3* pOrgFrom, const float* pAllowedHeight);

protected:
	// 충돌체를 만들때 구현해야 하는 펑션들
	// 이번 움직임 전체의 바운딩박스를 구한다
	void CalcBoundingBox();
	
	/// 바운딩박스 계산, 로컬좌표계
	virtual void GetBoundingBox(MBox &outBox) = 0;
	/// 충돌체가 destionation 까지 갈때 점 충돌지점에 도달할때까지 거리비율 ( = walkable distance / | destination - position | )
	virtual float GetWalkableDistance(const RCollisionPolygon &collisionPolygon) = 0;	

	virtual MVector3 GetContactPoint(const MVector3& vPos, const MVector3& vNorm) = 0;
	
	// from에서 vDir방향으로 fDist만큼 이동하면서 충돌을 검사한다.
	// MoveTest/Sliding함수는 재귀적으로 스스로 호출하기때문에 이동방향이 계속 바뀐다.
	// 따라서 같은방향의 힘이 계속 가해지는 상황을 제대로 표현할 수 없는 문제점이 있음.
	// vFrom에서 vDir방향으로 fDist만큼 이동했을때 결과물을 pOut에 출력해줌
	bool MoveTestReal(const MVector3& vFrom, const MVector3& vDir, float fDist, MVector3& vOut);


	// 지형고려해서 이동, x-y로만 이동하고 z는 지형을 따르는게 주요한 기능.
	bool MoveTestTerrain(const MVector3& vFrom, const MVector3& vDir, float fDist, MVector3& vOut, MVector3* pOrgForm = NULL, float* pAllowedHeight = NULL);

public:

	void Clear();

	RCollision(void);
	virtual ~RCollision(void);

	void SetTargetCollisionTree(RCollisionTree* pTargetTree);


	// 자신의 위치에서 to로 이동할때 처음 부딫히는 곳을 pOut으로 리턴, 부딫힌게 없다면 return false
	bool MoveTest(const MVector3 &from,const MVector3 &to, MVector3& vOut, float *pMoved = NULL);

	// 자신의 위치에서 to 로 이동할때 중력고려해서 가능한만큼을 pOut으로 리턴, 부딫힌게 없다면 return false
	bool MoveTestGravity(const MVector3 &from,const MVector3 &to, MVector3& vOut, bool *pSliding = NULL, MVector3* pOrgForm = NULL, float* pAllowedHeight = NULL);


	void GetMovementBoundingBox(MBox &outBox);	// 움직임의 전체 바운딩박스를 구한다, 월드좌표계	

	virtual bool CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly) = 0;	// 이 폴리곤과 접하는가?

public:
	/////////////////////////////////////////////////////
	// SceneNode 에서 충돌을 구현하기 위해 호출할 펑션들

//	void SetWorldMatrix(const RMatrix& matWorld);	// 충졸처리를 위한 월드행렬을 지정, 아래의 두 펑션에 영향을 준다
	///	이번움직임과 box가 잠재적 충돌가능성이 있는지
	virtual bool IsCollidable(const MBox &box) = 0;
	/// 주어진 폴리곤이 충돌하는지 검사
	virtual bool CheckPolygon(const MVector3 &v0,const MVector3 &v1,const MVector3 &v2, const DWORD dwAttribute = 0 ) = 0;
};


inline void RCollision::SetTargetCollisionTree(RCollisionTree* pTargetTree) { m_pTargetCollisionTree = pTargetTree; }
inline void RCollision::GetMovementBoundingBox(MBox &outBox) { outBox = m_bbMovement;}


/////////////////////
// Sphere 형 collider

class RCollisionSphere : public RCollision
{
	float	m_fRadius;

protected:
	virtual void GetBoundingBox(MBox &outBox);
	virtual float GetWalkableDistance(const RCollisionPolygon &collisionPolygon);	

	virtual MVector3 GetContactPoint(const MVector3& vPos, const MVector3& vNorm);

public:
	RCollisionSphere(float fRadius = SPHERE_DEFAULT_RADIUS);
	~RCollisionSphere();

	void SetRadius(float fRadius);
	float GetRadius();

	virtual bool IsCollidable(const MBox &box);
	virtual bool CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute = 0);

	static bool IsIntersectMovingSphereTriangle(const MVector3 &from,const MVector3 &to,const float fRadius,const MVector3 &v0,const MVector3 &v1,const MVector3 &v2, MVector3 &outNearest, int &debugFlag);

	virtual bool CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly);	// 이 폴리곤과 접하는가?
};

class RCollisionMovingCone : public RCollision
{
	MMatrix m_View;
	float m_fInitDistance;
	float m_fDistance;
	float m_fHeight;
	float m_fRadius;

protected:
	virtual void GetBoundingBox(MBox &outBox);
	virtual float GetWalkableDistance(const RCollisionPolygon &collisionPolygon){return true;}	

	virtual MVector3 GetContactPoint(const MVector3& vPos, const MVector3& vNorm){return MVector3(0,0,0);}

	float GetNearestDistance(MVector3& begin, MVector3& end);

public:
	RCollisionMovingCone(float fHeght, float fRadius, float fDistance, MMatrix mView);
	~RCollisionMovingCone(){}


	virtual bool IsCollidable(const MBox &box);
	virtual bool CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute = 0);

	virtual bool CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly){ return true;}

	float GetDistance(){return m_fDistance;}
};

}
	