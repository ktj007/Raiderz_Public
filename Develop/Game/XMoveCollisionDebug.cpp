#include "stdafx.h"
#include "XMoveCollisionDebug.h"
#include "RCollisionTree.h"

//#ifdef _DEBUG
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
//#endif

//////////////////////////////////////////////////////////////////////////
void XCheckCollisionDebug::GetBoundingBox( MBox &outBox )
{
	outBox.minx = outBox.miny = outBox.minz = -2000;
	outBox.maxx = outBox.maxy = outBox.maxz = 2000;
}

void XCheckCollisionDebug::StartTest( vec3 vCurrentPos, RCollisionTree* pTargetCollisionTree, vector<RCollisionPolygon>& vecOutPolygon )
{
	m_source = vCurrentPos;
	m_destination = vCurrentPos + vec3(0, 0, -1);

	m_colPolygons.clear();

	CalcBoundingBox();

	pTargetCollisionTree->GetCollidablePolygons(*this, RCollisionTriangle::ALL);

	vecOutPolygon = m_colPolygons;
}

float XCheckCollisionDebug::GetWalkableDistance( const RCollisionPolygon &collisionPolygon )
{
	// (reverse-intersecting the sphere) 
	// sphere 가 source->destination 으로 이동하는 도중 collisionPoint 걸리는 지점을 찾는 것이므로
	// collisionPoint를 중심, 같은 반지름을 가진 sphere 에 source->destination 선분이 교차하는 지점과 같다.


	MVector3 dir = m_destination - m_source;
	float fDistance = dir.Length();
	dir.Normalize();		

	float fCollisionDistance;
	if(MMath::IsIntersectRaySphere(m_source, dir,collisionPolygon.nearestPoint,2000,fCollisionDistance)) {
		const float CONTACTTOLER = 0.01f;

		if(fabs(fCollisionDistance)<CONTACTTOLER) fCollisionDistance = 0;	// 접해있으면 더 진행할수 없다
		return min(fCollisionDistance , fDistance);
	}



	// 충돌하지 않으면 끝까지 갈수있다.
	return fDistance;
}

MVector3 XCheckCollisionDebug::GetContactPoint( const MVector3& vPos, const MVector3& vNorm )
{
	return vPos - (vNorm * (2000 - 10));
}

bool XCheckCollisionDebug::CheckTangency( const MVector3& vPos, const RCollisionPolygon& poly )
{
	float fDist = (vPos - poly.nearestPoint).Length();
	return (fDist <= 2000 + 0.01f);	// 0.01은 FP오차를 위한 보정
}

bool XCheckCollisionDebug::IsCollidable( const MBox &box )
{
	return m_bbMovement.ContainsAABB(box);
}

bool XCheckCollisionDebug::CheckPolygon( const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute /*= 0*/ )
{
	MPlane plane(lv0,lv1,lv2);

	int nDebugFlag = -1;
	MVector3 nearestPoint;
	vec3 vPlaneNor = plane.Normal();
	vPlaneNor.Normalize();
	m_colPolygons.push_back(RCollisionPolygon(lv0, lv1, lv2, nearestPoint, vPlaneNor, nDebugFlag, dwAttribute));
	return true;
}

//////////////////////////////////////////////////////////////////////////
XMoveCollisionDebug::XMoveCollisionDebug()
{
	m_fRadius					= 25.0f;
	m_nRenderCollisionInvestigateIndex = 0;

	m_bShowCollisionSphere		= false;
	m_bShowMoveCol_ReBuildPoly	= false;
	m_bShowMoveCol_Real			= false;
	m_bShowMoveCol_Down			= false;
	m_bShowMoveCol_View			= false;
	m_bCollisionInvestigate		= false;
	m_bRenderCollisionInvestigate	= false;
}

XMoveCollisionDebug::~XMoveCollisionDebug()
{
	m_vecCollisionInvestigate.clear();
}

void XMoveCollisionDebug::ShowCollisionInvestigate( bool bShow )
{
	if(bShow == m_bCollisionInvestigate)
		return;

	m_bRenderCollisionInvestigate = false;

	if(bShow)
		m_vecCollisionInvestigate.clear();
	if(bShow == false)
		m_bRenderCollisionInvestigate = true;

	m_bCollisionInvestigate = bShow;
}

void XMoveCollisionDebug::ShowCollisionSphere( bool bShow )
{
	if(bShow == m_bShowCollisionSphere)
		return;

	m_vecCollisionSphere.clear();

	m_bShowCollisionSphere = bShow;
}

void XMoveCollisionDebug::ShowMoveCol_ReBuildPoly( bool bShow )
{
	if(bShow == m_bShowMoveCol_ReBuildPoly)
		return;

	m_vecFrontDebugColReBuildPolygon.clear();

	m_bShowMoveCol_ReBuildPoly = bShow;
}

void XMoveCollisionDebug::ShowMoveCol_Down( bool bShow )
{
	if(bShow == m_bShowMoveCol_Down)
		return;

	m_vecDownDebugColPolygon.clear();
	m_vecDownDebugColTestPolygon.clear();

	m_bShowMoveCol_Down = bShow;
}

void XMoveCollisionDebug::ShowMoveCol_Real( bool bShow )
{
	if(bShow == m_bShowMoveCol_Real)
		return;

	m_vecFrontDebugColPolygon.clear();
	m_vecRestrictedAreaDebugColPolygon.clear();
	m_vecDownDebugColTestPolygon.clear();

	m_bShowMoveCol_Real = bShow;
}

void XMoveCollisionDebug::Render()
{
	if(m_bShowMoveCol_ReBuildPoly)
	{
		RenderDebugTerrainPolycon(m_vecFrontDebugColReBuildPolygon, D3DCOLOR_XRGB(255, 0, 255));
	}

	if(m_bShowMoveCol_Real)
	{
		RenderDebugTerrainPolycon(m_vecFrontDebugColPolygon, D3DCOLOR_XRGB(0, 255, 255));
		RenderDebugTerrainPolycon(m_vecRestrictedAreaDebugColPolygon, D3DCOLOR_XRGB(0, 255, 255));
		RenderDebugTerrainPolycon(m_vecDownDebugColTestPolygon, D3DCOLOR_XRGB(255, 255, 0));
	}

	if(m_bShowMoveCol_View)
	{
		MBox checkBox;
		m_checkCollisionDebug.GetMovementBoundingBox(checkBox);
		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( MMatrix::IDENTITY, checkBox, 0.1f, D3DCOLOR_XRGB(255, 0, 0));
		RenderDebugTerrainPolycon(m_vecViewTerrainColPolygon, D3DCOLOR_XRGB(191, 0, 191));
	}

	if(m_bShowMoveCol_Down)
	{
		RenderDebugTerrainPolycon(m_vecDownDebugColTestPolygon, D3DCOLOR_XRGB(85, 125, 255));
		RenderDebugTerrainPolycon(m_vecDownDebugColPolygon, D3DCOLOR_XRGB(0, 0, 255));
	}

	if(m_bShowCollisionSphere)
	{
		for(vector<vec3>::iterator itSphere = m_vecCollisionSphere.begin(); itSphere != m_vecCollisionSphere.end(); itSphere++)
		{
			RMatrix mat;
			mat.MakeIdentity();
			mat.SetTranslation(*itSphere);

			REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugSphere(mat, m_fRadius, 0.1f, D3DCOLOR_XRGB(0, 255, 255));
		}
	}

	RenderCollisionInvestigateData(m_nRenderCollisionInvestigateIndex);
}

void XMoveCollisionDebug::RenderDebugTerrainPolycon( vector<RCollisionPolygon>& col, DWORD _dwColor )
{
	if(col.size() == 0)
		return;

	RVector* debugVector = NULL;

	debugVector = new RVector[col.size() * 3];
	int vecCount = 0;
	for(vector<RCollisionPolygon>::iterator it = col.begin(); it != col.end(); it++)
	{
		debugVector[vecCount] = it->v[0] + it->normal;
		vecCount++;
		debugVector[vecCount] = it->v[1] + it->normal;
		vecCount++;
		debugVector[vecCount] = it->v[2] + it->normal;
		vecCount++;
	}

	REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugPolycon(MMatrix::IDENTITY, debugVector, vecCount, 0.1f, _dwColor);

	SAFE_DELETE(debugVector);
}

void XMoveCollisionDebug::Init()
{
	m_vecCollisionSphere.clear();
	m_vecFrontDebugColReBuildPolygon.clear();
	m_vecFrontDebugColPolygon.clear();
	m_vecRestrictedAreaDebugColPolygon.clear();
	m_vecDownDebugColPolygon.clear();
	m_vecDownDebugColTestPolygon.clear();
	m_vecViewTerrainColPolygon.clear();

	m_stCollisionInvestigateData.init();
}

void XMoveCollisionDebug::AddFrontDebugColReBuildPolygon( RCollisionPolygon& col )
{
	if(m_bShowMoveCol_ReBuildPoly)
		m_vecFrontDebugColReBuildPolygon.push_back(col);
}

void XMoveCollisionDebug::AddFrontDebugColPolygon( RCollisionPolygon& col )
{
	if(m_bShowMoveCol_Real)
		m_vecFrontDebugColPolygon.push_back(col);
}

void XMoveCollisionDebug::AddDownDebugColPolygon( RCollisionPolygon& col )
{
	if(m_bShowMoveCol_Down)
		m_vecDownDebugColPolygon.push_back(col);
}

void XMoveCollisionDebug::AddDownDebugColTestPolygon( RCollisionPolygon& col )
{
	if(m_bShowMoveCol_Down || m_bShowMoveCol_Real)
		m_vecDownDebugColTestPolygon.push_back(col);
}

vector<RCollisionPolygon>* XMoveCollisionDebug::GetRestrictedAreaDebugColPolygon()
{
	if(m_bShowMoveCol_Real)
		return &m_vecRestrictedAreaDebugColPolygon;

	return NULL;
}

void XMoveCollisionDebug::Show( bool bSphere, bool bReBuildPoly, bool bDown, bool bReal, bool bView, bool bInvestigate, int nIndex )
{
	ShowCollisionSphere(bSphere);
	ShowMoveCol_ReBuildPoly(bReBuildPoly);
	ShowMoveCol_Down(bDown);
	ShowMoveCol_Real(bReal);
	ShowMoveCol_View(bView);
	ShowCollisionInvestigate(bInvestigate);

	m_nRenderCollisionInvestigateIndex = nIndex;
}

void XMoveCollisionDebug::AddCollisionInvestigate()
{
	if(m_bCollisionInvestigate)
		m_vecCollisionInvestigate.push_back(m_stCollisionInvestigateData);
}

void XMoveCollisionDebug::Flip()
{
	AddCollisionInvestigate();
	Render();
}

void XMoveCollisionDebug::SetCollisionInvestigateData_Front( vec3 vStart, vec3 vEnd, vec3 vResult, bool bCol )
{
	if(m_bCollisionInvestigate == false)
		return;

	m_stCollisionInvestigateData.vFrontCheckStart	= vStart;
	m_stCollisionInvestigateData.vFrontCheckEnd		= vEnd;
	m_stCollisionInvestigateData.vFrontResult		= vResult;
	m_stCollisionInvestigateData.bFrontCol			= bCol;
}

void XMoveCollisionDebug::SetCollisionInvestigateData_FrontBack( vec3 vStart, vec3 vEnd, vec3 vResult, bool bCol )
{
	if(m_bCollisionInvestigate == false)
		return;

	m_stCollisionInvestigateData.vFrontBackCheckStart	= vStart;
	m_stCollisionInvestigateData.vFrontBackCheckEnd		= vEnd;
	m_stCollisionInvestigateData.vFrontBackResult		= vResult;
	m_stCollisionInvestigateData.bFrontBackCol			= bCol;
}

void XMoveCollisionDebug::SetCollisionInvestigateData_Down( vec3 vStart, vec3 vEnd, vec3 vResult, bool bCol )
{
	if(m_bCollisionInvestigate == false)
		return;

	m_stCollisionInvestigateData.vDownCheckStart	= vStart;
	m_stCollisionInvestigateData.vDownCheckEnd		= vEnd;
	m_stCollisionInvestigateData.vDownResult		= vResult;
	m_stCollisionInvestigateData.bDownCol			= bCol;
}

void XMoveCollisionDebug::AddCollisionInvestigateData_Front( RCollisionPolygon& col )
{
	if(m_bCollisionInvestigate)
		m_stCollisionInvestigateData.vecFront.push_back(col);
}

void XMoveCollisionDebug::AddCollisionInvestigateData_Down( RCollisionPolygon& col )
{
	if(m_bCollisionInvestigate)
		m_stCollisionInvestigateData.vecDown.push_back(col);
}

void XMoveCollisionDebug::RenderCollisionInvestigateData( int nIndex )
{
	int nCount = m_vecCollisionInvestigate.size();
	if(m_bRenderCollisionInvestigate == false ||
		nIndex >= nCount ||
		nIndex < 0)
		return;

	RenderDebugMoveBox(m_vecCollisionInvestigate[nIndex].vFrontCheckStart, m_vecCollisionInvestigate[nIndex].vFrontCheckEnd, D3DCOLOR_XRGB(0, 255, 255));
	RenderDebugMoveBox(m_vecCollisionInvestigate[nIndex].vFrontBackCheckStart, m_vecCollisionInvestigate[nIndex].vFrontBackCheckEnd, D3DCOLOR_XRGB(255, 255, 0));
	RenderDebugMoveBox(m_vecCollisionInvestigate[nIndex].vDownCheckStart, m_vecCollisionInvestigate[nIndex].vDownCheckEnd, D3DCOLOR_XRGB(0, 0, 120));

	RenderDebugTerrainPolycon(m_vecCollisionInvestigate[nIndex].vecFront, D3DCOLOR_ARGB(120, 120, 0, 120));
	RenderDebugTerrainPolycon(m_vecCollisionInvestigate[nIndex].vecCheckFront, D3DCOLOR_ARGB(120, 255, 0, 255));
	RenderDebugTerrainPolycon(m_vecCollisionInvestigate[nIndex].vecDown, D3DCOLOR_ARGB(120, 0, 0, 255));

	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation(m_vecCollisionInvestigate[nIndex].vFrontResult);
	if(m_vecCollisionInvestigate[nIndex].bFrontCol)
		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 4, 0.1f, D3DCOLOR_XRGB(255, 0, 0));
	if(m_vecCollisionInvestigate[nIndex].bFrontBackCol)
		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 4, 0.1f, D3DCOLOR_XRGB(0, 255, 0));
	if(m_vecCollisionInvestigate[nIndex].bDownCol)
		mat.SetTranslation(m_vecCollisionInvestigate[nIndex].vDownResult);
		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 5, 0.1f, D3DCOLOR_XRGB(120, 120, 0));
}

void XMoveCollisionDebug::RenderDebugMoveBox( vec3 vStart, vec3 vEnd, DWORD _dwColor )
{
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation(vStart);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 3, 0.1f, _dwColor);
	mat.SetTranslation(vEnd);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 3, 0.1f, D3DCOLOR_XRGB(255, 255, 255));
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugLine( vStart, vEnd, 0.1f, _dwColor);
}

void XMoveCollisionDebug::AddCollisionInvestigateData_CheckFront( RCollisionPolygon& col )
{
	if(m_bCollisionInvestigate)
		m_stCollisionInvestigateData.vecCheckFront.push_back(col);
}

void XMoveCollisionDebug::CalTerrainColReal( vec3 vCurrentPos, RCollisionTree* pTargetCollisionTree )
{
	if(pTargetCollisionTree == NULL ||
		m_bShowMoveCol_View == false)
		return;

	m_checkCollisionDebug.StartTest(vCurrentPos, pTargetCollisionTree, m_vecViewTerrainColPolygon);
}

void XMoveCollisionDebug::ShowMoveCol_View( bool bShow )
{
	if(bShow == m_bShowMoveCol_View)
		return;

	m_vecViewTerrainColPolygon.clear();

	m_bShowMoveCol_View = bShow;
}
