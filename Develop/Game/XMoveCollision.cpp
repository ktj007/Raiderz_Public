#include "stdafx.h"
#include "XMoveCollision.h"
#include "MMath.h"
#include "RCollisionTree.h"
#include "XGameState.h"
#include "XCollisionTree.h"

//#ifdef _DEBUG
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
//#endif

//const float SLIDING_LIMIT = 0.76f;	// AXISZ.dot(poly.normal) 의 수치. 이 이상이면 미끄러진다.
//const float	HOVER_HEIGHT = 20.0f;	// 이만큼 띄워서 이동한다. 이 높이 + Radius 이하의 턱은 넘어감.
//const float	STEEPPLANE_CUTOFF_HEIGHT = 50.0f;	// 가파른 경사면이면서 이것보다 높이가 낮은 폴리곤은 잘라내버린다.
#define BUILD_RECT_SIZE 1000.0f
//////////////////////////////////////////////////////////////////////////
XMoveCollision::XMoveCollision()
{
	m_bMoveWater		= false;
	m_bRestrictedArea	= false;

	m_fMin_x			= 0.0f;
	m_fMin_y			= 0.0f;
	m_fMax_x			= 0.0f;
	m_fMax_y			= 0.0f;

	m_strMaterialName.clear();

	m_pvecTerrainColObjectPolygon	= NULL;
}

XMoveCollision::~XMoveCollision()
{
}

void XMoveCollision::SetRadius( float fRadius )
{
	m_fRadius = fRadius;

	m_CollisionDebug.SetRadius(fRadius);
}

float XMoveCollision::GetRadius()
{
	return m_fRadius;
}

void XMoveCollision::SetMoveAllowedHeight( float fHeight )
{
	m_fMoveAllowedHeight = fHeight;
}

float XMoveCollision::GetMoveAllowedHeight()
{
	return m_fMoveAllowedHeight;
}

bool XMoveCollision::IsIntersectMovingSphereTriangle( const MVector3 &from,const MVector3 &to,const float fRadius,const MVector3 &v0,const MVector3 &v1,const MVector3 &v2, MVector3 &outNearest, int &debugFlag )
{
	const float INTERSECT_TOLER	= 0.1f;

	// 삼각형을 포함하는 평면
	MPlane plane(v0,v1,v2);

	// 출발이 평면뒷편인경우 검사하지 않는다.
	float fDistFromPlane = plane.GetDistanceTo(from);
	if(fDistFromPlane<0) return false;

	float fDistToPlane = plane.GetDistanceTo(to);
	if(fabs(fDistToPlane-fDistFromPlane) < INTERSECT_TOLER ) return false;	// 평행하게 이동하면 검사하지 않는다.
	if(fDistToPlane>fDistFromPlane) return false;	// 멀어지는 방향이면 검사하지 않는다.

	// 1. from-to 선분을 따라 sphere가 삼각형평면까지 움직일때 충돌하는 점이 삼각형 내부에 놓이는 경우

	//if(fDistFromPlane < fRadius) // 이미 충돌해있다 
	//{
	//	outNearest = GetNearestPoint(from,plane);
	//	return true;
	//}

	MVector3 nearestOnFromTo;
	MVector3 planeIntersectionPoint;
	MVector3 sphereIntersectionPoint;

	if(fDistFromPlane < fRadius) // 이미 충돌해있다 ( plane embedded )
	{
		planeIntersectionPoint = MMath::GetNearestPoint(from,plane);
		nearestOnFromTo = planeIntersectionPoint + fRadius*plane.Normal();
	}
	else
	{
		vec3 vMoveDir = to - from;
		vec3 vSphereFace = from - (plane.Normal() * fRadius);

		// from-to 선분위의 평면에 가장가까운 점
		nearestOnFromTo = MMath::GetNearestPoint(vSphereFace, vSphereFace + vMoveDir, plane);

		// nearestPtOnFromTo 와 삼각형평면의 거리가 반지름을 초과하면 부딪힐수가 없다.
		if(plane.GetDistanceTo(nearestOnFromTo)>fRadius) return false;

		// nearestPtOnFromTo와 가장 가까운 평면위의 점 ( = from-to 선분과 가장 가까운 삼각형 평면위의 점)
		planeIntersectionPoint = MMath::GetNearestPoint(nearestOnFromTo,plane);
		nearestOnFromTo = planeIntersectionPoint + fRadius*plane.Normal();
	}

	// 이 점이 삼각형 안에 있는지 검사
	// 각각의 edge를 포함하고 나머지 한 점쪽을 바라보는 평면들에 대해 모두 + 방향이면
	if(MPlane(CrossProduct(plane.Normal(),v1-v0),v0).GetDistanceTo(planeIntersectionPoint)>0 &&
		MPlane(CrossProduct(plane.Normal(),v2-v1),v1).GetDistanceTo(planeIntersectionPoint)>0 &&
		MPlane(CrossProduct(plane.Normal(),v0-v2),v2).GetDistanceTo(planeIntersectionPoint)>0 ) 
	{ // nearestPtOnPlane은 삼각형 내부의 점이다
		vec3 vOrgDir = (to - from).Normalize();
		vec3 vRealDir = (nearestOnFromTo - from).Normalize();

		if(LengthSq(nearestOnFromTo-from)<LengthSq(to-from) ||
			vOrgDir.DotProduct(vRealDir) < 0 )						// 혹시 뒤로 갔습니까?
		{
			outNearest = planeIntersectionPoint;
			debugFlag = 0;
			return true;
		}
	}
	//	*/




	// 2. from-to 와 가장 가까운 점이 삼각형의 edge에 놓이는경우

	// 가장 가까운 삼각형의 edge위의 점을 찾는다 = polygonIntersectionPoint.
	float fNearest = FLT_MAX;
	float fDistToEdge;
	MVector3 polygonIntersectionPoint;
	MVector3 edgePoint;

	edgePoint = MMath::GetNearestPointOnLine(planeIntersectionPoint,v0,v1);
	fDistToEdge = (edgePoint -  planeIntersectionPoint).Length();
	if(fDistToEdge < fNearest) { fNearest = fDistToEdge; polygonIntersectionPoint = edgePoint; }
 
	edgePoint = MMath::GetNearestPointOnLine(planeIntersectionPoint,v1,v2);
	fDistToEdge = (edgePoint -  planeIntersectionPoint).Length();
	if(fDistToEdge < fNearest) { fNearest = fDistToEdge; polygonIntersectionPoint = edgePoint; }

	edgePoint = MMath::GetNearestPointOnLine(planeIntersectionPoint,v2,v0);
	fDistToEdge = (edgePoint -  planeIntersectionPoint).Length();
	if(fDistToEdge < fNearest) { fNearest = fDistToEdge; polygonIntersectionPoint = edgePoint; }

	// polygonIntersectionPoint에서부터 sphere로 reverse-intersection 
	float colDistance;
	MVector3 revDir = Normalize(from-to);

	// 멀어지는 방향이면 검사하지 않는다
	if(DotProduct(revDir,(polygonIntersectionPoint-from))>- 0.001f) return false;

	if(MMath::IsIntersectRaySphere( polygonIntersectionPoint, revDir, from, fRadius, colDistance)) {
		if(colDistance < (to-from).Length()) {
			outNearest = polygonIntersectionPoint;
			debugFlag = 1;
			return true;
		}
	}

	return false;
}

bool XMoveCollision::IsCollidable( const MBox &box )
{
	return m_bbMovement.ContainsAABB(box);
}

bool XMoveCollision::CheckPolygon( const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute /*= 0*/ )
{
	MPlane plane(lv0,lv1,lv2);

	MVector3 dir = m_destination - m_source;
	dir.Normalize();

	// 이동방향에 대해 backface 이면 체크하지 않는다.
	if(plane.DotNormal(dir)>0) return false;

	int nDebugFlag = -1;
	MVector3 nearestPoint;
	if(IsIntersectMovingSphereTriangle(m_source,m_destination,m_fRadius,lv0,lv1,lv2,nearestPoint,nDebugFlag)) 
	{
		vec3 vPlaneNor = plane.Normal();
		vPlaneNor.Normalize();
		m_colPolygons.push_back(RCollisionPolygon(lv0, lv1, lv2, nearestPoint, vPlaneNor, nDebugFlag, dwAttribute));
		return true;
	}

	return false;
}

bool XMoveCollision::CheckTangency( const MVector3& vPos, const RCollisionPolygon& poly )
{
	float fDist = (vPos - poly.nearestPoint).Length();
	return (fDist <= GetRadius() + 0.01f);	// 0.01은 FP오차를 위한 보정
}

bool XMoveCollision::CheckAllowedHeight( const MVector3& v1, const MVector3& v2, const MVector3& v3, const MVector3* pOrgFrom, const float* pAllowedHeight )
{
	if(pOrgFrom == NULL || pAllowedHeight == NULL)
		return false;

	// float형의 계산 오류때문에 약간의 +값을 적용한다.
	if((v1.z - pOrgFrom->z) <= *pAllowedHeight + 0.1f &&
		(v2.z - pOrgFrom->z) <= *pAllowedHeight + 0.1f &&
		(v3.z - pOrgFrom->z) <= *pAllowedHeight + 0.1f)
		return true;

	return false;
}

void XMoveCollision::GetBoundingBox( MBox &outBox )
{
	outBox.minx = outBox.miny = outBox.minz = -m_fRadius;
	outBox.maxx = outBox.maxy = outBox.maxz = m_fRadius;
}

float XMoveCollision::GetWalkableDistance( const RCollisionPolygon &collisionPolygon )
{
	return GetMovableDistance(m_source, m_destination, collisionPolygon.nearestPoint);
}

float XMoveCollision::GetMovableDistance( const MVector3 &from,const MVector3 &to,const MVector3 &collisionPoint )
{
	// (reverse-intersecting the sphere) 
	// sphere 가 source->destination 으로 이동하는 도중 collisionPoint 걸리는 지점을 찾는 것이므로
	// collisionPoint를 중심, 같은 반지름을 가진 sphere 에 source->destination 선분이 교차하는 지점과 같다.


	MVector3 dir = to - from;
	float fDistance = dir.Length();
	dir.Normalize();		

	float fCollisionDistance;
	if(MMath::IsIntersectRaySphere(m_source, dir,collisionPoint,m_fRadius,fCollisionDistance)) {
		const float CONTACTTOLER = 0.01f;

		if(fabs(fCollisionDistance)<CONTACTTOLER) fCollisionDistance = 0;	// 접해있으면 더 진행할수 없다
		return min(fCollisionDistance , fDistance);
	}



	// 충돌하지 않으면 끝까지 갈수있다.
	return fDistance;
}

MVector3 XMoveCollision::GetContactPoint( const MVector3& vPos, const MVector3& vNorm )
{
	return vPos - (vNorm * (m_fRadius - 10));
}

void XMoveCollision::GetCollisionPolygon( const MVector3 &from,const MVector3 &to, bool bIgnoreCheckHeight )
{
	MVector3 ret;

	// 시작점-끝점 세팅
	m_source = from;
	m_destination = to;

	// 이동 범위에 대한 충돌 폴리곤을 가져온다.
	MVector3 vDir = m_destination - m_source;
	float fDist = vDir.Length();
	vDir.Normalize();

	m_colPolygons.clear();

	CalcBoundingBox();

	m_pTargetCollisionTree->GetCollidablePolygons(*this, ~RCollisionTriangle::WATER);

	GetTerrainColObjectCollisionPolygon();

	// 걸리는 폴리곤이 없으면
	if(m_colPolygons.empty() == false) 
	{
		float fMinMove = fDist;

		for(int i=0; i<(int)m_colPolygons.size(); i++ )
		{
			RCollisionPolygon &poly = m_colPolygons[i];


			bool bCheck = CheckAllowedHeight(poly.v[0], poly.v[1], poly.v[2], &m_vOrgfrom, &m_fMoveAllowedHeight);

			if(bCheck == false || bIgnoreCheckHeight == true)
			{
				// 내 방향대로 평면까지 진행했을때 갈수있는 최대 거리
				float fMove = GetMovableDistance(m_source, m_destination, poly.nearestPoint);

				// 최소값을 찾는다
				if(fDist > fMove) 
				{
					fMinMove = fMove;
		
					//if(m_bMoveWater)
					//	ReBuildWaterCollisionPolygon(poly);
					//else
						ReBuildGroundCollisionPolygon(poly);
				}
			}
		}
	}
}

bool XMoveCollision::MoveTestFront( const MVector3 &from,const MVector3 &to, MVector3& vOut, bool bUpCol )
{
	vec3 ret;
	bool bMoveCol = false;

	vec3 source = from;
	vec3 destination = to;

	for(int itr = 0; ; itr++)
	{
		MVector3 vDir = destination - source;
		float fDist = vDir.Length();
		vDir.Normalize();

		m_vecFrontColPolygon.clear();
		vec3 vTestSource = m_source;
		for(vector<vec3>::iterator itSphere = m_vecCollisionSphere.begin(); itSphere != m_vecCollisionSphere.end(); itSphere++)
		{
			(*itSphere).x = m_source.x;
			(*itSphere).y = m_source.y;

			// 정면
			vec3 vTempTo = (*itSphere) + (vDir * fDist);
			GetCollisionPolygon((*itSphere) - vDir, vTempTo,  bUpCol);
		}

		GetRestrictedAreaPoly(source, destination, m_vecFrontColPolygon);

		m_source = source;
		m_destination = destination;

		float fMinMove = fDist;
		RCollisionPolygon* pPlaneNear = NULL;

		if(m_vecFrontColPolygon.empty())
		{
			ret = m_destination;
			break;
		}

		vec3 vNearPos = vec3(0, 0, 0);
		for(vector<RCollisionPolygon>::iterator itPolygon = m_vecFrontColPolygon.begin(); itPolygon != m_vecFrontColPolygon.end(); itPolygon++)
		{
			vec3 vNearPoint;

			float fDot = vDir.DotProduct((*itPolygon).normal);
			if(fDot < 0)
			{
				int nTemp;
				if(IsIntersectMovingSphereTriangle(m_source, m_destination, m_fRadius, (*itPolygon).v[0], (*itPolygon).v[1], (*itPolygon).v[2], vNearPoint, nTemp))
				{
					// 내 방향대로 평면까지 진행했을때 갈수있는 최대 거리
					float fMove = GetMovableDistance(m_source, m_destination, vNearPoint);

					// 최소값을 찾는다
					if(fMinMove > fMove) 
					{
						fMinMove = fMove;
						pPlaneNear = &(*itPolygon);
						vNearPos = vNearPoint;
						bMoveCol = true;
						m_CollisionDebug.AddCollisionInvestigateData_Front(*itPolygon);
					}
				}
				else
					m_CollisionDebug.AddCollisionInvestigateData_CheckFront(*itPolygon);
			}
		}

		ret = m_source + (vDir * fMinMove);

		if(pPlaneNear)
		{
			// 슬라이딩이 가능한 노말인가?
			//vec3 vMoveDir = vDir;
			//vMoveDir.z = 0.0f;
			//vMoveDir.Normalize();
			//pPlaneNear->normal.Normalize();
			//float fDot = pPlaneNear->normal.DotProduct(vMoveDir);
			if(itr == 0)
			{
				// 슬라이딩을 해야 한다.
				//bSliding = true;

				vec3 vPlaneSlidingPos = m_destination + pPlaneNear->normal.DotProduct(ret - m_destination) * pPlaneNear->normal;
				source = ret;
				destination = vPlaneSlidingPos;
			}
			else
			{
				// 슬라이딩 더이상 못해
				break;
			}
		}
		else
		{
			break;
		}
	}

	vOut = ret;

	return bMoveCol;
}

bool XMoveCollision::MoveTestDown( const MVector3 &from,const MVector3 &to, MVector3& vOut, MVector3& vOutNor, bool& bSliding )
{
	MVector3 ret;
	MVector3 nor;
	bool bMoveCol = false;

	// 시작점-끝점 세팅
	m_source = from;
	m_destination = to;
	nor.Set(vec3::ZERO);

	RCollisionPolygon* pPolygonTest = NULL;
	for(int itr = 0; ; itr++)
	{
		// 이동 범위에 대한 충돌 폴리곤을 가져온다.
		MVector3 vDir = m_destination - m_source;
		float fDist = vDir.Length();
		vDir.Normalize();

		m_colPolygons.clear();

		CalcBoundingBox();

		m_pTargetCollisionTree->GetCollidablePolygons(*this, ~RCollisionTriangle::WATER);

		GetRestrictedAreaPoly(m_source, m_destination, m_colPolygons);

		// 걸리는 폴리곤이 없으면
		if(m_colPolygons.empty()) 
		{
			// 걸리는데가 없3
			ret = m_destination;
			//bFreeMove = true;
			break;
		}
		else
		{
			float fMinMove = fDist;

			RCollisionPolygon* pPolygon = NULL;

			for(int i=0; i<(int)m_colPolygons.size(); i++ )
			{
				RCollisionPolygon &poly = m_colPolygons[i];

				m_CollisionDebug.AddDownDebugColTestPolygon(poly);

				// 내 방향대로 평면까지 진행했을때 갈수있는 최대 거리
				float fMove = GetMovableDistance(m_source, m_destination, poly.nearestPoint);

				// 최소값을 찾는다
				if(fMinMove>fMove) 
				{
					fMinMove = fMove;
					pPolygon = &poly;
					bMoveCol = true;
					pPolygonTest = pPolygon;
					m_CollisionDebug.AddCollisionInvestigateData_Down(poly);
				}
			}

			// 일단 가능한 거리만큼 움직임
			ret = m_source + vDir * (fMinMove);
			bSliding = false;

			if(pPolygon)
			{
				nor = pPolygon->normal;
				float fDot = pPolygon->normal.DotProduct(vec3::AXISZ);

				if(fDot <= SLIDING_LIMIT &&
					CheckAllowedDownSliding(pPolygon->v[0], pPolygon->v[1], pPolygon->v[2]))		// 슬라이딩이 가능한 면인가?
				{
					// 슬라이딩을 해야 한다.
					bSliding = true;

					MPlane plane(pPolygon->normal, pPolygon->nearestPoint);
					vec3 vPlaneSlidingPos = m_destination + plane.DotNormal(ret- m_destination) * plane.Normal();
					m_source = ret;
					m_destination = vPlaneSlidingPos/* + plane.Normal() * m_fRadius*/;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	if(gg.currentgamestate && pPolygonTest)
	{
		m_CollisionDebug.AddDownDebugColPolygon(*pPolygonTest);

		// 메터리얼 정보 가져오기
		vec3 vCheckPos = ret;
		if(pPolygonTest->nDebugFlag == 1)
		{
			vCheckPos = pPolygonTest->v[0] + (((pPolygonTest->v[2] + ((pPolygonTest->v[1] - pPolygonTest->v[2]) / 2.0f)) - pPolygonTest->v[0]) / 2.0f);
		}

		m_strMaterialName = gg.currentgamestate->GetWorld()->GetMaterialName(vCheckPos);

		// 최종 위치 지형 노말값을 가져온다. 위에 있는 바닥의 노말값이 아닐 수 있다.
		gg.currentgamestate->GetWorld()->PickCollision(ret, vec3(ret.x, ret.y, to.z - 500.0f ), NULL, &vOutNor);
		vOutNor.Normalize();
	}

	vOut = ret;

	return bMoveCol;
}

void XMoveCollision::GetCollisionSphere( MCapsule capsuleCollision, float fUpValue, vector<vec3>& vecSphereCenter )
{
	vecSphereCenter.clear();

	// 무조건 있어야 하는 캡슐
	vec3 vCapseuleUpDwonDir = (capsuleCollision.top - capsuleCollision.bottom).Normalize();
	vec3 vUpSphere = capsuleCollision.top - (vCapseuleUpDwonDir * (m_fRadius - capsuleCollision.radius));
	vec3 vDownSphere = capsuleCollision.bottom + (vCapseuleUpDwonDir * (m_fRadius - capsuleCollision.radius));

	vDownSphere.z += fUpValue;

	vecSphereCenter.push_back(vDownSphere);

	float fCenterHeight = (vUpSphere - vDownSphere).Length();
	if(fCenterHeight <= 0)
		return;

	int nSphereCount = (int)(fCenterHeight / (m_fRadius * 2));

	if(nSphereCount <= 0)
		return;

	if(fCenterHeight - (nSphereCount * m_fRadius * 2) >= 0)
		nSphereCount++;

	nSphereCount++;

	float fHeightGap = fCenterHeight / nSphereCount;
	vec3 vSphereCenter = vDownSphere;
	for(int i = 0; i < nSphereCount; i++)
	{
		vSphereCenter += (vCapseuleUpDwonDir * fHeightGap);
		vecSphereCenter.push_back(vSphereCenter);
	}
}

bool XMoveCollision::GroundMoveTest( const MVector3 &from,const MVector3 &to, MVector3 orgForm, MCapsule capsuleCollision, MVector3& vOut, MVector3& vOutNor, bool *pSliding /*= NULL*/, bool *pFrontCol /*= NULL*/ )
{
	SetMapCollisionInfo();

	m_bMoveWater = false;
	m_strMaterialName.clear();

	// 이동 중에 어딘가에 걸렸으면 False, 아니면 True
	m_vecFrontColPolygon.clear();

	m_CollisionDebug.Init();

	m_vOrgfrom = orgForm;

	MVector3 vTempFrom, vTempTo, vTempUp, vTempFront, vTempDown;
	MVector3 vDownColNor;
	MVector3 diff = to - from;
	MVector3 vDir = diff;
	vDir.z = 0.0f;
	float ft = vDir.Length();
	vDir.Normalize();

	// 위쪽 충돌구 위치 계산
	vec3 vCapseuleUpDwonDir = (capsuleCollision.top - capsuleCollision.bottom).Normalize();
	m_vUpSphere = capsuleCollision.top - (vCapseuleUpDwonDir * (m_fRadius - capsuleCollision.radius));

	// 위쪽 총돌을 위해서 검사할 최대 높이 설정
	bool bUpMove = false;
	vTempFrom = m_vUpSphere;
	if((m_fMoveAllowedHeight - m_fRadius) > (to.z - from.z))
		vTempFrom.z += (m_fMoveAllowedHeight - m_fRadius);
	else
	{
		vTempFrom.z += (to.z - from.z);
		bUpMove = true;
	}

	// 위
	bool bUpCol = MoveTest(m_vUpSphere - vec3(0, 0, 1), vTempFrom, vTempUp);

	if(vTempUp.z <= from.z)
		vTempUp.z = from.z;

	float fUpGap = vTempUp.z - m_vUpSphere.z;

	if(bUpCol)
		fUpGap = 0.0f;

	// 충돌구 만들기
	GetCollisionSphere(capsuleCollision, fUpGap, m_vecCollisionSphere);

	m_CollisionDebug.SetCollisionSphere(m_vecCollisionSphere);

	// 정면
	vTempTo = m_vecCollisionSphere[0] + (vDir * ft);
	bool bFrontCol = MoveTestFront(m_vecCollisionSphere[0] - vDir, vTempTo, vTempFront, bUpCol);
	m_CollisionDebug.SetCollisionInvestigateData_Front(m_vecCollisionSphere[0] - vDir, vTempTo, vTempFront, bFrontCol);

	if(bFrontCol)
	{
		// 반대?
		vec3 vFrontMoveDir = (vTempFront - m_vecCollisionSphere[0]).Normalize();
		vec3 vFrontColDir = (vTempFront - vTempTo).Normalize();
		if(vFrontColDir.DotProduct(vFrontMoveDir) >= 0.0f)
		{
			// 한번더!!! 빠지지 않기 위해서
			vec3 vFrontBack = vTempFront;
			bFrontCol = MoveTestFront(m_vecCollisionSphere[0] + vDir, vFrontBack, vTempFront, bUpCol);
			m_CollisionDebug.SetCollisionInvestigateData_FrontBack(m_vecCollisionSphere[0] + vDir, vFrontBack, vTempFront, bFrontCol);
		}
	}

	// 밑에
	vec3 vDownStart = vTempFront;
	if(from.z > vTempFront.z)
		vDownStart.z = from.z;

	vec3 vDownTo = vTempFront;
	vDownTo.z = to.z;

	// 위쪽으로 충돌이 났다면 검사할 z값 변경
	if(bUpMove && bUpCol)
		vDownTo.z = from.z + (vTempUp.z - m_vUpSphere.z);

	bool bSliding = false;
	vDownColNor.Set(vec3::AXISZ);

	bool bDownCol = MoveTestDown(vDownStart + vec3(0, 0, 1), vDownTo, vTempDown, vDownColNor, bSliding);

	m_CollisionDebug.SetCollisionInvestigateData_Down(vDownStart + vec3(0, 0, 1), vDownTo, vTempDown, bDownCol);

	// 슬라이딩 체크
	if(pSliding &&
		(from.z != vTempDown.z))
	{
		*pSliding = bSliding;

		// 슬라이딩인데 올라가버리네?
		if(bSliding && vTempDown.z > from.z)
		{
			vec3 vReTempDown = vTempDown;
			vReTempDown.z = from.z;
			vDownStart.z = from.z;
			bFrontCol = MoveTestFront(vDownStart, vReTempDown, vTempDown, bUpCol);


			// 최종 위치 지형 노말값을 가져온다. 위에 있는 바닥의 노말값이 아닐 수 있다.
			vec3 vFinalDir = (vReTempDown - vDownStart).Normalize();
			gg.currentgamestate->GetWorld()->PickCollision(vDownStart, vDownStart + (vDir * 100.0f), NULL, &vDownColNor);
			vDownColNor.Normalize();
		}
	}

	// 정면 충돌
	if(pFrontCol)
		*pFrontCol = bFrontCol;

	vOut = vTempDown;
	vOutNor = vDownColNor;

	// 지형 디버그용
	m_CollisionDebug.CalTerrainColReal(vOut, m_pTargetCollisionTree);

	m_CollisionDebug.Flip();
	return bDownCol;
}

bool XMoveCollision::WaterMoveTest( const MVector3 &from, const MVector3 &to, MVector3 orgForm, MCapsule capsuleCollision, float fWaterHeightLimit, MVector3& vOut, MVector3& vOutNor )
{
	SetMapCollisionInfo();

	m_bMoveWater = true;

	// 이동 중에 어딘가에 걸렸으면 False, 아니면 True
	m_vecFrontColPolygon.clear();
	m_vecCollisionSphere.clear();

	m_CollisionDebug.Init();

	m_vOrgfrom = orgForm;

	bool bSliding;
	MVector3 vTempFrom, vTempTo, vTempUp, vTempFront, vTempDown;
	MVector3 vDownColNor;
	MVector3 diff = to - from;
	MVector3 vDir = diff;
	vDir.z = 0.0f;
	float ft = vDir.Length();
	vDir.Normalize();

	// 충돌구 만들기...
	GetCollisionSphere(capsuleCollision, m_fMoveAllowedHeight - m_fRadius, m_vecCollisionSphere);

	m_CollisionDebug.SetCollisionSphere(m_vecCollisionSphere);

	// 정면
	vTempTo = m_vecCollisionSphere[0] + (vDir * ft);
	bool bFronCol = MoveTestFront(m_vecCollisionSphere[0] - vDir, vTempTo, vTempFront, false);

	// 밑에
	vec3 vDownStart = vTempFront;
	vec3 vDownTo = vTempFront;
	vDownTo.z = to.z;

	vDownColNor.Set(vec3::ZERO);
	bool bDownCol = MoveTestDown(vDownStart + vec3(0, 0, 1), vDownTo, vTempDown, vDownColNor, bSliding);

	vTempFront.z -= (m_fMoveAllowedHeight - m_fRadius);
	vOut = vTempFront;
	vOutNor = vDownColNor;

	m_CollisionDebug.Flip();
	return (bDownCol || bFronCol);
}

void XMoveCollision::ReBuildGroundCollisionPolygon( RCollisionPolygon& colPolygon )
{
	float fPandoraDot = colPolygon.normal.DotProduct(vec3::AXISZ);						

	m_CollisionDebug.AddFrontDebugColPolygon(colPolygon);

	// 충돌용 폴리곤 가공
	if(colPolygon.dwPolygonAttribute & RCollisionTriangle::TERRAIN ||
		fPandoraDot > SLIDING_LIMIT)
	{
		m_vecFrontColPolygon.push_back(colPolygon);
	}
	else
	{
		BuildRectCollisionPolygon(colPolygon, m_vecFrontColPolygon);
	}
}

void XMoveCollision::ReBuildWaterCollisionPolygon( RCollisionPolygon& colPolygon )
{
	float fPandoraDot = colPolygon.normal.DotProduct(vec3::AXISZ);						

	m_CollisionDebug.AddFrontDebugColPolygon(colPolygon);

	// 충돌용 폴리곤 가공
	if(fPandoraDot > SLIDING_LIMIT/* &&
		fPandoraDot > -SLIDING_LIMIT*/)
	{
		m_vecFrontColPolygon.push_back(colPolygon);
	}
	else
	{
		//BuildRectCollisionPolygon(colPolygon, m_vecFrontColPolygon);
	}
}

void XMoveCollision::BuildRectCollisionPolygon( RCollisionPolygon &colPolygon, vector<RCollisionPolygon>& frontColPolygon )
{
	vec3 vX = colPolygon.normal.CrossProduct(vec3::AXISZ);
	vX.Normalize();
	vec3 vResult = vec3::AXISZ.CrossProduct(vX);
	vResult.Normalize();

	vec3 v[3];
	v[0] = colPolygon.v[0];
	v[1] = colPolygon.v[1];
	v[2] = colPolygon.v[2];

	vec3 vMax = vec3(-100000.0f, -100000.0f, -100000.0f);
	vec3 vMin = vec3(100000.0f, 100000.0f, 100000.0f);
	vec3 vCal[3];
	MMatrix matWorld;
	MMatrix matWorldInver;

	matWorld.SetLocalMatrix(v[0], -colPolygon.normal, vec3::AXISZ);
	matWorld.GetInverse(&matWorldInver);

	for(int i = 0; i < 3; i++)
	{
		matWorldInver.TransformVect(v[i], vCal[i]);

		if(vCal[i].x > vMax.x)
			vMax.x = vCal[i].x;
		if(vCal[i].y > vMax.y)
			vMax.y = vCal[i].y;
		if(vCal[i].z > vMax.z)
			vMax.z = vCal[i].z;

		if(vCal[i].x < vMin.x)
			vMin.x = vCal[i].x;
		if(vCal[i].y < vMin.y)
			vMin.y = vCal[i].y;
		if(vCal[i].z < vMin.z)
			vMin.z = vCal[i].z;
	}

	vMax.z += BUILD_RECT_SIZE;
	vMin.z -= BUILD_RECT_SIZE;

	vec3 vHalfLine = (vMin - vMax) / 2.0f;
	vec3 vRectCenter = vMax + vHalfLine;

	vec3 vTempNor = vHalfLine;
	vTempNor.Normalize();
	vec3 vRightDir = vec3(-vTempNor.x, -vTempNor.y, vTempNor.z);
	float fLen = vHalfLine.Length();
	vec3 vRight = vRectCenter + (vRightDir * vHalfLine.Length());

	float fHalfWid = (vMin - vRight).Length() / 2.0f;
	float fHalfHei = (vMax - vRight).Length() / 2.0f;

	vec3 vTempRecCenter = vRectCenter;
	matWorld.TransformVect(vTempRecCenter, vRectCenter);

	// 스피어 충돌 위치 구하기
	vec3 vSphereColPos = colPolygon.nearestPoint;
	// 각이 밑으로 있을경우에만 이런 계산하자.
	if(colPolygon.normal.DotProduct(vec3::AXISZ) < 0)
	{
		vec3 vSphereCenter = colPolygon.nearestPoint + (colPolygon.normal * m_fRadius);
		vec3 vCalDir = -colPolygon.normal;
		XMath::NormalizeZ0(vCalDir);
		vSphereColPos = vSphereCenter + (vCalDir * m_fRadius);
	}

	// 센터 위치 구하기
	vec3 vColNearRectCenter;
	vec3 vCenterToNearPoint = (vSphereColPos - vRectCenter).Normalize();
	float fReCenterDot = vResult.DotProduct(vCenterToNearPoint);
	vec3 vCenterMoveDir = vResult;
	if(fReCenterDot < 0.0f)
	{
		// 뒤로
		vCenterMoveDir = -vResult;
	}

	vec3 v2DNearPos = vSphereColPos;
	vec3 v2DRectCenter = vRectCenter;
	v2DNearPos.z = v2DRectCenter.z = 0.0f;
	vec3 vMoveNewCenter = MMath::GetNearestPointOnRay(v2DNearPos, v2DRectCenter, vCenterMoveDir);
	float fCenterMoveLen = (v2DRectCenter - vMoveNewCenter).Length();

	vColNearRectCenter = vRectCenter + (vCenterMoveDir * fCenterMoveLen);

	vec3 vRect[4];
	vRect[0] = vColNearRectCenter + vX * - fHalfWid + vec3::AXISZ * + fHalfHei;
	vRect[1] = vColNearRectCenter + vX * + fHalfWid + vec3::AXISZ * + fHalfHei;
	vRect[2] = vColNearRectCenter + vX * - fHalfWid + vec3::AXISZ * - fHalfHei;
	vRect[3] = vColNearRectCenter + vX * + fHalfWid + vec3::AXISZ * - fHalfHei;

	frontColPolygon.push_back(RCollisionPolygon(vRect[0], vRect[1], vRect[2], vSphereColPos, vResult, colPolygon.nDebugFlag, colPolygon.dwPolygonAttribute));
	frontColPolygon.push_back(RCollisionPolygon(vRect[1], vRect[3], vRect[2], vSphereColPos, vResult, colPolygon.nDebugFlag, colPolygon.dwPolygonAttribute));

	m_CollisionDebug.AddFrontDebugColReBuildPolygon(RCollisionPolygon(vRect[0], vRect[1],vRect[2], vSphereColPos, vResult, colPolygon.nDebugFlag, colPolygon.dwPolygonAttribute));
	m_CollisionDebug.AddFrontDebugColReBuildPolygon(RCollisionPolygon(vRect[1], vRect[3], vRect[2], vSphereColPos, vResult, colPolygon.nDebugFlag, colPolygon.dwPolygonAttribute));
}

void XMoveCollision::BuildRectCollisionPolygon( const MVector3 &from,const MVector3 &to, vec3 v1, vec3 v2, vector<RCollisionPolygon>& frontColPolygon, vector<RCollisionPolygon>* pDebugColPolygon )
{
	float fCenter_z = abs(from.z + (abs(from.z - to.z) / 2.0f));

	vec3 vDir = (v2 - v1).Normalize();
	vec3 vNormal = vDir.CrossProduct(vec3::AXISZ);

	vec3 vZ = vec3(0, 0, fCenter_z);
	vec3 vRect[4];

	vRect[0] = v2 + vZ * + BUILD_RECT_SIZE;
	vRect[1] = v1 + vZ * + BUILD_RECT_SIZE;
	vRect[2] = v2 + vZ * - BUILD_RECT_SIZE;
	vRect[3] = v1 + vZ * - BUILD_RECT_SIZE;

	vec3 vNearPoint;
	int nTemp;
	
	if(IsIntersectMovingSphereTriangle(from, to, m_fRadius, vRect[0], vRect[1], vRect[2], vNearPoint, nTemp))
		frontColPolygon.push_back(RCollisionPolygon(vRect[0], vRect[1], vRect[2], vNearPoint, vNormal, 0, RCollisionTriangle::TERRAIN));
	if(IsIntersectMovingSphereTriangle(from, to, m_fRadius, vRect[1], vRect[3], vRect[2], vNearPoint, nTemp))
		frontColPolygon.push_back(RCollisionPolygon(vRect[1], vRect[3], vRect[2], vNearPoint, vNormal, 0, RCollisionTriangle::TERRAIN));

	if(pDebugColPolygon)
	{
		pDebugColPolygon->push_back(RCollisionPolygon(vRect[0], vRect[1],vRect[2], vRect[1], vNormal, 0, RCollisionTriangle::TERRAIN));
		pDebugColPolygon->push_back(RCollisionPolygon(vRect[1], vRect[3], vRect[2], vRect[1], vNormal, 0, RCollisionTriangle::TERRAIN));
	}
}

void XMoveCollision::SetMapCollisionInfo()
{
	// 충돌 정보 입력
	if (gg.currentgamestate)
		SetTargetCollisionTree(gg.currentgamestate->GetWorld()->GetCollisionTree());
}

bool XMoveCollision::CheckAllowedDownSliding( const MVector3& v1, const MVector3& v2, const MVector3& v3 )
{
	// 높이 차가 m_fRadius값 보다 작다면... 슬라이딩을 무시하자!!
	// 면이 작아서... 무시해도 될것 같아서...

	float fMaxZ = max(max(v1.z, v2.z), v3.z);
	float fMinZ = min(min(v1.z, v2.z), v3.z);

	float fCheckRadius = m_fRadius;
	if(m_fMoveAllowedHeight > m_fRadius)
		fCheckRadius = m_fMoveAllowedHeight;

	if((fMaxZ - fMinZ) > fCheckRadius)
		return true;

	return false;
}

void XMoveCollision::SetRestrictedArea( float fMin_x, float fMin_y, float fMax_x, float fMax_y )
{
	m_bRestrictedArea = false;

	if(fMin_x == 0.0f &&
		fMin_y == 0.0f &&
		fMax_x == 0.0f &&
		fMax_y == 0.0f)
		return;

	m_bRestrictedArea = true;

	m_fMin_x = fMin_x;
	m_fMin_y = fMin_y;
	m_fMax_x = fMax_x;
	m_fMax_y = fMax_y;
}

void XMoveCollision::GetRestrictedAreaPoly( const MVector3 &from, const MVector3 & to, vector<RCollisionPolygon>& vecColPolygon )
{
	// 통제구역인가?
	if(m_bRestrictedArea)
	{
		const float fSectorSize = 30000.0f;
		if(m_fMin_x + fSectorSize >= from.x)
			BuildRectCollisionPolygon(from, to, vec3(m_fMin_x, m_fMin_y, 0), vec3(m_fMin_x, m_fMax_y, 0), vecColPolygon, m_CollisionDebug.GetRestrictedAreaDebugColPolygon());
		if(m_fMax_x - fSectorSize <= from.x)
			BuildRectCollisionPolygon(from, to,vec3(m_fMax_x, m_fMax_y, 0), vec3(m_fMax_x, m_fMin_y, 0), vecColPolygon, m_CollisionDebug.GetRestrictedAreaDebugColPolygon());
		if(m_fMin_y + fSectorSize >= from.y)
			BuildRectCollisionPolygon(from, to, vec3(m_fMax_x, m_fMin_y, 0), vec3(m_fMin_x, m_fMin_y, 0), vecColPolygon, m_CollisionDebug.GetRestrictedAreaDebugColPolygon());
		if(m_fMax_y - fSectorSize <= from.y)
			BuildRectCollisionPolygon(from, to, vec3(m_fMin_x, m_fMax_y, 0), vec3(m_fMax_x, m_fMax_y, 0), vecColPolygon, m_CollisionDebug.GetRestrictedAreaDebugColPolygon());
	}
}

bool XMoveCollision::FreeMoveTest( const MVector3 &from, const MVector3 &to, MVector3 orgForm, MCapsule capsuleCollision, MVector3& vOut, MVector3& vOutNor, bool *pSliding /*= NULL*/, bool *pFrontCol /*= NULL*/ )
{
	SetMapCollisionInfo();

	m_bMoveWater = false;
	m_strMaterialName.clear();

	// 이동 중에 어딘가에 걸렸으면 False, 아니면 True
	m_vecFrontColPolygon.clear();

	m_CollisionDebug.Init();

	m_vOrgfrom = orgForm;

	MVector3 vTempFrom, vTempTo, vTempUp, vTempFront, vTempDown;
	MVector3 vDownColNor;
	MVector3 diff = to - from;
	MVector3 vDir = diff;
	vDir.z = 0.0f;
	float ft = vDir.Length();
	vDir.Normalize();

	// 위쪽 충돌구 위치 계산
	vec3 vCapseuleUpDwonDir = (capsuleCollision.top - capsuleCollision.bottom).Normalize();
	m_vUpSphere = capsuleCollision.top - (vCapseuleUpDwonDir * (m_fRadius - capsuleCollision.radius));

	// 위쪽 총돌을 위해서 검사할 최대 높이 설정
	bool bUpMove = false;
	vTempFrom = m_vUpSphere;
	if((m_fMoveAllowedHeight - m_fRadius) > (to.z - from.z))
		vTempFrom.z += (m_fMoveAllowedHeight - m_fRadius);
	else
	{
		vTempFrom.z += (to.z - from.z);
		bUpMove = true;
	}

	// 위
	bool bUpCol = MoveTest(m_vUpSphere - vec3(0, 0, 1), vTempFrom, vTempUp);

	if(vTempUp.z <= from.z)
		vTempUp.z = from.z;

	float fUpGap = vTempUp.z - m_vUpSphere.z;

	if(bUpCol)
		fUpGap = 0.0f;

	// 충돌구 만들기
	GetCollisionSphere(capsuleCollision, fUpGap, m_vecCollisionSphere);

	m_CollisionDebug.SetCollisionSphere(m_vecCollisionSphere);

	// 정면
	vTempTo = m_vecCollisionSphere[0] + (vDir * ft);
	bool bFrontCol = MoveTestFront(m_vecCollisionSphere[0] - vDir, vTempTo, vTempFront, bUpCol);
	m_CollisionDebug.SetCollisionInvestigateData_Front(m_vecCollisionSphere[0] - vDir, vTempTo, vTempFront, bFrontCol);

	if(bFrontCol)
	{
		// 반대?
		vec3 vFrontMoveDir = (vTempFront - m_vecCollisionSphere[0]).Normalize();
		vec3 vFrontColDir = (vTempFront - vTempTo).Normalize();
		if(vFrontColDir.DotProduct(vFrontMoveDir) >= 0.0f)
		{
			// 한번더!!! 빠지지 않기 위해서
			vec3 vFrontBack = vTempFront;
			bFrontCol = MoveTestFront(m_vecCollisionSphere[0] + vDir, vFrontBack, vTempFront, bUpCol);
			m_CollisionDebug.SetCollisionInvestigateData_FrontBack(m_vecCollisionSphere[0] + vDir, vFrontBack, vTempFront, bFrontCol);
		}
	}

	// 밑에
	vec3 vDownStart = vTempFront;
	if(from.z > vTempFront.z)
		vDownStart.z = from.z;

	vec3 vDownTo = vTempFront;
	vDownTo.z = to.z;

	// 위쪽으로 충돌이 났다면 검사할 z값 변경
	if(bUpMove && bUpCol)
		vDownTo.z = from.z + (vTempUp.z - m_vUpSphere.z);

	bool bSliding = false;
	vDownColNor.Set(vec3::AXISZ);

	bool bDownCol = MoveTestDown(vDownStart + vec3(0, 0, 1), vDownTo, vTempDown, vDownColNor, bSliding);

	m_CollisionDebug.SetCollisionInvestigateData_Down(vDownStart + vec3(0, 0, 1), vDownTo, vTempDown, bDownCol);

	// 슬라이딩 체크
	if(pSliding)
		*pSliding = bSliding;

	// 정면 충돌
	if(pFrontCol)
		*pFrontCol = bFrontCol;

	vOut = vTempDown;
	vOutNor = vDownColNor;

	m_CollisionDebug.Flip();
	return (bDownCol || bFrontCol);
}

void XMoveCollision::GetTerrainColObjectCollisionPolygon()
{
	// npc형 오브젝트 충돌 처리
	// npc에서 박스로된 충돌처리를 지형에서 처리한다.
	// 캡슐은 기존 오브젝트 충돌에서 처리...
	if(m_pvecTerrainColObjectPolygon == NULL)
		return;

	for(vector<RCollisionPolygon>::iterator it = m_pvecTerrainColObjectPolygon->begin(); it != m_pvecTerrainColObjectPolygon->end(); ++it)
	{				
		CheckPolygon(it->v[0], it->v[1], it->v[2]);
	}

}

bool XMoveCollision::CameraMoveTest( const MVector3 &from,const MVector3 &to, MVector3& vOut, float *pMoved /*= NULL*/ )
{
	if (m_pTargetCollisionTree == NULL) return false;

	// 이전 충돌 폴리곤들을 삭제
	Clear();

	m_source = from;
	m_destination = to;

	CalcBoundingBox();

	m_pTargetCollisionTree->GetCollidablePolygons(*this, ~RCollisionTriangle::WATER);

	GetTerrainColObjectCollisionPolygon();

	if(m_colPolygons.empty()) 
	{
		vOut = to;
		return false;
	}

	MVector3 diff = m_destination - m_source;

	float fMinMove = diff.Length();
	float fLen = fMinMove;

	// 각 평면에 대해 최대로 갈수있는 거리들중 최소값이 갈수있는 거리다.
	for(int i=0; i<(int)m_colPolygons.size(); i++ )
	{
		RCollisionPolygon &poly = m_colPolygons[i];

		// 내 방향대로 평면까지 진행했을때 갈수있는 최대 거리
		float fMove = GetWalkableDistance(poly);

		fMinMove = min(fMinMove ,fMove);
	}

	fMinMove = max(0, fMinMove);

	vOut = m_source + Normalize(diff) * (fMinMove);

	if (pMoved != NULL)
		*pMoved = fMinMove / fLen;

	return true;
}
