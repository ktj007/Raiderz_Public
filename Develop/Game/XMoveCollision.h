#pragma once
#ifndef _XMOVE_COLLISION_H
#define _XMOVE_COLLISION_H

#include "RCollision.h"
#include "XMoveCollisionDebug.h"

class XMoveCollision : public RCollision
{
private:
	float					m_fRadius;
	float					m_fMoveAllowedHeight;
	MVector3				m_vOrgfrom;

	vec3					m_vUpSphere;
	vec3					m_vDownSphere;

	bool					m_bMoveWater;

	wstring					m_strMaterialName;

	//////////////////////////////////////////////////////////////////////////
	// 통제구역
	bool					m_bRestrictedArea;
	float					m_fMin_x, m_fMin_y;
	float					m_fMax_x, m_fMax_y;

	//////////////////////////////////////////////////////////////////////////
	// 충돌 계산
	vector<RCollisionPolygon>	m_vecFrontColPolygon;	
	vector<vec3>				m_vecCollisionSphere;

	vector<RCollisionPolygon>*			m_pvecTerrainColObjectPolygon;

public:
	XMoveCollisionDebug			m_CollisionDebug;

private:
	void					ReBuildGroundCollisionPolygon(RCollisionPolygon& colPolygon);
	void					ReBuildWaterCollisionPolygon(RCollisionPolygon& colPolygon);

	void					BuildRectCollisionPolygon(RCollisionPolygon &colPolygon, vector<RCollisionPolygon>& frontColPolygon);
	void					BuildRectCollisionPolygon(const MVector3 &from,const MVector3 &to, vec3 v1, vec3 v2, vector<RCollisionPolygon>& frontColPolygon, vector<RCollisionPolygon>* pDebugColPolygon);
	bool					IsIntersectMovingSphereTriangle(const MVector3 &from,const MVector3 &to,const float fRadius,const MVector3 &v0,const MVector3 &v1,const MVector3 &v2, MVector3 &outNearest, int &debugFlag);

	bool					MoveTestFront(const MVector3 &from,const MVector3 &to, MVector3& vOut, bool bUpCol);
	bool					MoveTestDown(const MVector3 &from,const MVector3 &to, MVector3& vOut, MVector3& vOutNor, bool& bSliding);

	bool					CheckAllowedHeight(const MVector3& v1, const MVector3& v2, const MVector3& v3, const MVector3* pOrgFrom, const float* pAllowedHeight);
	bool					CheckAllowedDownSliding(const MVector3& v1, const MVector3& v2, const MVector3& v3);
	bool					CheckColPolycon(MVector3& from, MVector3& vDir, const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2);

	void					GetCollisionPolygon(const MVector3 &from,const MVector3 &to, bool bIgnoreCheckHeight);
	void					GetRestrictedAreaPoly( const MVector3 &from, const MVector3 & to, vector<RCollisionPolygon>& vecColPolygon );
	float					GetMovableDistance(const MVector3 &from,const MVector3 &to,const MVector3 &collisionPoint);	
	void					GetTerrainColObjectCollisionPolygon();

	void					SetMapCollisionInfo();

protected:
	virtual bool			IsCollidable(const MBox &box);
	virtual bool			CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute = 0);
	virtual bool			CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly);	// 이 폴리곤과 접하는가?

	virtual void			GetBoundingBox(MBox &outBox);
	virtual float			GetWalkableDistance(const RCollisionPolygon &collisionPolygon);
	virtual MVector3		GetContactPoint(const MVector3& vPos, const MVector3& vNorm);

public:
	XMoveCollision();
	virtual ~XMoveCollision();

	void					SetRadius(float fRadius);
	void					SetMoveAllowedHeight(float fHeight);
	void					SetRestrictedArea(float fMin_x, float fMin_y, float fMax_x, float fMax_y);
	void					SetTerrainColObject(vector<RCollisionPolygon>* pVec) { m_pvecTerrainColObjectPolygon = pVec; }

	float					GetRadius();
	float					GetMoveAllowedHeight();
	void					GetCollisionSphere(MCapsule capsuleCollision, float fUpValue, vector<vec3>& vecSphereCenter);
	wstring					GetMoveTestDownMaterialName() { return m_strMaterialName; }

	bool					GroundMoveTest(const MVector3 &from,const MVector3 &to, MVector3 orgForm, MCapsule capsuleCollision, MVector3& vOut, MVector3& vOutNor, bool *pSliding = NULL, bool *pFrontCol = NULL);
	bool					WaterMoveTest(const MVector3 &from, const MVector3 &to, MVector3 orgForm, MCapsule capsuleCollision, float fWaterHeightLimit, MVector3& vOut, MVector3& vOutNor);
	bool					FreeMoveTest(const MVector3 &from, const MVector3 &to, MVector3 orgForm, MCapsule capsuleCollision, MVector3& vOut, MVector3& vOutNor, bool *pSliding = NULL, bool *pFrontCol = NULL);
	bool					CameraMoveTest(const MVector3 &from,const MVector3 &to, MVector3& vOut, float *pMoved = NULL);
};

#endif