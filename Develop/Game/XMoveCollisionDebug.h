#pragma once
#ifndef _XMOVE_COLLISION_DEBUG_H
#define _XMOVE_COLLISION_DEBUG_H

#include "RCollision.h"

struct stCollisionInvestigateData
{
	vec3						vFrontCheckStart;
	vec3						vFrontCheckEnd;
	vec3						vFrontResult;
	bool						bFrontCol;
	vec3						vFrontBackCheckStart;
	vec3						vFrontBackCheckEnd;
	vec3						vFrontBackResult;
	bool						bFrontBackCol;
	vec3						vDownCheckStart;
	vec3						vDownCheckEnd;
	vec3						vDownResult;
	bool						bDownCol;

	vector<RCollisionPolygon>	vecFront;
	vector<RCollisionPolygon>	vecCheckFront;
	vector<RCollisionPolygon>	vecDown;

	void init()
	{
		bFrontCol = false;
		bFrontBackCol = false;
		bDownCol = false;

		vFrontCheckStart.Set(vec3::ZERO);
		vFrontCheckEnd.Set(vec3::ZERO);;
		vFrontResult.Set(vec3::ZERO);;
		vFrontBackCheckStart.Set(vec3::ZERO);
		vFrontBackCheckEnd.Set(vec3::ZERO);;
		vFrontBackResult.Set(vec3::ZERO);;
		vDownCheckStart.Set(vec3::ZERO);
		vDownCheckEnd.Set(vec3::ZERO);;
		vDownResult.Set(vec3::ZERO);
		
		vecFront.clear();
		vecCheckFront.clear();
		vecDown.clear();
	}
};

class XCheckCollisionDebug : public RCollision
{
private:
	MBox						m_boxTerrainColReal;

public:
	XCheckCollisionDebug() : RCollision()		{}
	~XCheckCollisionDebug()						{}

	virtual void				GetBoundingBox(MBox &outBox);
	virtual float				GetWalkableDistance(const RCollisionPolygon &collisionPolygon);
	virtual MVector3			GetContactPoint(const MVector3& vPos, const MVector3& vNorm);
	virtual bool				CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly);
	virtual bool				CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute = 0);
	virtual bool				IsCollidable(const MBox &box);

	void						StartTest(vec3 vCurrentPos, RCollisionTree* pTargetCollisionTree, vector<RCollisionPolygon>& vecOutPolygon);
};

class XMoveCollisionDebug
{
private:
	bool						m_bShowCollisionSphere;
	bool						m_bShowMoveCol_ReBuildPoly;
	bool						m_bShowMoveCol_Real;
	bool						m_bShowMoveCol_Down;
	bool						m_bShowMoveCol_View;
	bool						m_bCollisionInvestigate;
	bool						m_bRenderCollisionInvestigate;

	float						m_fRadius;

	int							m_nRenderCollisionInvestigateIndex;

	XCheckCollisionDebug		m_checkCollisionDebug;

public:
	vector<vec3>				m_vecCollisionSphere;
	vector<RCollisionPolygon>	m_vecFrontDebugColReBuildPolygon;	
	vector<RCollisionPolygon>	m_vecFrontDebugColPolygon;	
	vector<RCollisionPolygon>	m_vecRestrictedAreaDebugColPolygon;	
	vector<RCollisionPolygon>	m_vecDownDebugColPolygon;	
	vector<RCollisionPolygon>	m_vecDownDebugColTestPolygon;	
	vector<RCollisionPolygon>	m_vecViewTerrainColPolygon;	

	vector<stCollisionInvestigateData> m_vecCollisionInvestigate;

	stCollisionInvestigateData	m_stCollisionInvestigateData;

private:
	void						RenderDebugTerrainPolycon(vector<RCollisionPolygon>& col, DWORD _dwColor);
	void						RenderDebugMoveBox(vec3 vStart, vec3 vEnd, DWORD _dwColor);
	void						RenderCollisionInvestigateData(int nIndex);

	void						AddCollisionInvestigate();

public:
	XMoveCollisionDebug();
	~XMoveCollisionDebug();

	void						SetRadius(float fRadius)		{ m_fRadius = fRadius; }
	void						SetCollisionSphere(vector<vec3>& vecSphere) { m_vecCollisionSphere = vecSphere; }
	void						SetCollisionInvestigateData_Front(vec3 vStart, vec3 vEnd, vec3 vResult, bool bCol);
	void						SetCollisionInvestigateData_FrontBack(vec3 vStart, vec3 vEnd, vec3 vResult, bool bCol);
	void						SetCollisionInvestigateData_Down(vec3 vStart, vec3 vEnd, vec3 vResult, bool bCol);

	void						Show(bool bSphere, bool bReBuildPoly, bool bDown, bool bReal, bool bView, bool bInvestigate, int nIndex);
	void						ShowCollisionSphere(bool bShow);
	void						ShowMoveCol_ReBuildPoly(bool bShow);
	void						ShowMoveCol_Down(bool bShow);
	void						ShowMoveCol_Real(bool bShow);
	void						ShowMoveCol_View(bool bShow);
	void						ShowCollisionInvestigate(bool bShow);

	void						Init();
	void						Render();
	void						Flip();

	void						AddFrontDebugColReBuildPolygon(RCollisionPolygon& col);
	void						AddFrontDebugColPolygon(RCollisionPolygon& col);
	void						AddDownDebugColPolygon(RCollisionPolygon& col);
	void						AddDownDebugColTestPolygon(RCollisionPolygon& col);
	void						AddCollisionInvestigateData_Front(RCollisionPolygon& col);
	void						AddCollisionInvestigateData_CheckFront(RCollisionPolygon& col);
	void						AddCollisionInvestigateData_Down(RCollisionPolygon& col);

	void						CalTerrainColReal(vec3 vCurrentPos, RCollisionTree*	pTargetCollisionTree);

	vector<RCollisionPolygon>*	GetRestrictedAreaDebugColPolygon();
};

#endif