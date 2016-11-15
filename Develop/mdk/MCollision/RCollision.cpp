#include "StdAfx.h"
#include "RCollision.h"
#include "RCollisionTree.h"

const float SLIDING_LIMIT = 0.76f;	// AXISZ.dot(poly.normal) 의 수치. 이 이상이면 미끄러진다.
const float	HOVER_HEIGHT = 20.0f;	// 이만큼 띄워서 이동한다. 이 높이 + Radius 이하의 턱은 넘어감.
const float	STEEPPLANE_CUTOFF_HEIGHT = 50.0f;	// 가파른 경사면이면서 이것보다 높이가 낮은 폴리곤은 잘라내버린다.

namespace rs3 {

RCollision::RCollision(void) : m_pTargetCollisionTree(NULL)
{

}

RCollision::~RCollision(void)
{
}

void RCollision::Clear()
{
	m_colPolygons.erase(m_colPolygons.begin(),m_colPolygons.end());
}

bool RCollision::MoveTest(const MVector3 &from,const MVector3 &to, MVector3& vOut, float *pMoved)
{
	if (m_pTargetCollisionTree == NULL) return false;

	// 이전 충돌 폴리곤들을 삭제
	Clear();

	m_source = from;
	m_destination = to;

	CalcBoundingBox();
	
	m_pTargetCollisionTree->GetCollidablePolygons(*this, ~RCollisionTriangle::WATER);

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

const float COLLISION_TOLER	= 0.1f;


bool RCollision::MoveTestGravity( const MVector3 &from,const MVector3 &to, MVector3& vOut, bool *pSliding /*= NULL*/, MVector3* pOrgForm /*= NULL*/, float* pAllowedHeight /*= NULL*/ )
{
	MVector3 diff = to - from;
	MVector3 vDir = diff;

	vDir.z = 0;
	float ft = vDir.Length();

	MVector3 vRet, vTemp;

	vDir.Normalize();
	// 일단 여기까지, z방향으로의 이동은 나중에 하도록 제외한다.

	// 이동 중에 어딘가에 걸렸으면 False, 아니면 True
	bool bFreeMove = false;

	vRet = from;

	// 일단 일정 이상 띄워서 지형위에서 이동해본다. 
	bFreeMove = MoveTestTerrain(from, vDir, ft, vTemp, pOrgForm, pAllowedHeight);

	
	if (!bFreeMove)
	{	
		// 어딘가에 걸리거나 부드럽게 미끄러져야 할 필요가 있을때는 
 		float fMinMove = diff.Length();
		vDir = diff;
		vDir.Normalize();

		// 면따라 미끄러짐 적용해서 이동
		bFreeMove = MoveTestReal(vTemp, vDir, fMinMove, vRet);
	}
	else
		// 아니면 그냥 이동한다.
		bFreeMove = MoveTest(vTemp, vTemp + MVector3(0, 0, diff.z), vRet);


	vOut = vRet;

	// 경사가 급한 바닥이라 바닥밟고 미끄러져 떨어져야 하는가를 검사.
	if (pSliding != NULL)  *pSliding = true;
	// 단순히 -1만큼 내려가봐서 걸리는 면들중에 급경사가 있으면 미끄러진다고 간주한다.
	MoveTest(vRet, vRet + MVector3(0, 0, -1), vTemp);
	for(int i=0; i<(int)m_colPolygons.size(); i++ )
	{
		if (m_colPolygons[i].normal.DotProduct(MVector3::AXISZ) > SLIDING_LIMIT ||
			CheckAllowedHeight(m_colPolygons[i].v[0], m_colPolygons[i].v[1], m_colPolygons[i].v[2], pOrgForm, pAllowedHeight))		
		{
			if (pSliding != NULL) *pSliding = false;				
		}
	}


	return bFreeMove;

}

bool RCollision::MoveTestReal(const MVector3& vFrom, const MVector3& vDir, float fDist, MVector3& vOut)
{
	if (m_pTargetCollisionTree == NULL) return false;

	// 일단 현재의 시작점은 입력된 시작점
	MVector3 now = vFrom;

	for (int idx = 0; ; idx++)
	{
		// 이전 충돌 폴리곤들을 삭제
		Clear();

		// 시작점-끝점 세팅
		m_source = now;
		m_destination = now + vDir * fDist;

		CalcBoundingBox();

		// 어느어느 폴리곤에 걸리는지 알아본다.
		m_pTargetCollisionTree->GetCollidablePolygons(*this, ~RCollisionTriangle::WATER);

		// 걸리는 폴리곤이 없으면
		if(m_colPolygons.empty()) 
		{
			// 걸리는데가 없3
			vOut = m_destination;
			return false;
		}

		float fMinMove = fDist;

		vector<MPlane>	vPlanes;
		MVector3 *pContactPoint = NULL;

		for(int i=0; i<(int)m_colPolygons.size(); i++ )
		{
			RCollisionPolygon &poly = m_colPolygons[i];

			// 내 방향대로 평면까지 진행했을때 갈수있는 최대 거리
			float fMove = GetWalkableDistance(poly);

			// 최소값을 찾는다
			if(fMinMove>fMove) {
				fMinMove = fMove;
				pContactPoint = &(m_colPolygons[i].nearestPoint);
			}

			if (fMove <= COLLISION_TOLER)					// 이 면과 지금 접해있다면
			{
				if (vDir.DotProduct(poly.normal) < 0)		// 이 방향으로 이동했을때 이 면과 부딪히는가
				{
					MVector3 tmpNormal;
					if (poly.nDebugFlag == 0)				// 이동할때 면에 걸리는가
						tmpNormal = poly.normal;			// 면에 걸리면 그냥 면의 법선을 쓴다
					else
					{
						// 선에 걸리면 법선 계산 (점에 걸리면?)							
						tmpNormal = m_source - poly.nearestPoint;
						tmpNormal.Normalize();
					}

					// 미끄러질 평면을 계산해둔다
					vPlanes.push_back(MPlane(tmpNormal, poly.nearestPoint));
				}
			}
		}

		// 일단 가능한 거리만큼 움직임
		MVector3 ret = m_source + vDir * (fMinMove);			
		// 현재 위치를 수정
		m_source = ret;										
		// 간 거리만큼 갈 거리에서 뺀다.
		fDist -= fMinMove;									

		if(pContactPoint) 
		{
			MVector3 contactPointCenter = ret;

			MVector3 planeNormal;
			MVector3 ContactPoint;
			MVector3 slidingDestination;

			// 일단 걸리는 면의 갯수
			size_t colPolys = vPlanes.size();	

			if (colPolys <= 1)		// 걸리는 면이 한개뿐이면
			{
				planeNormal = (contactPointCenter - *pContactPoint).Normalize();

				// 그냥 평범하게 미끄러진다
				ContactPoint = *pContactPoint;
				MPlane plane(planeNormal ,ContactPoint);
				slidingDestination = m_destination + plane.DotNormal(contactPointCenter - m_destination) * plane.Normal();
				
			}
			else					// 걸리는 면이 두개 이상이면 (골치아픔)
			{
				// x번째 면 따라 미끄러졌을때 y번째 면에 걸리는가? 걸리면 true
				vector< vector<bool> >	vvIntersects;	
				// 같은 평면위거나 있는 폴리곤은 하나빼고 다 무시된다. true인것들만 체크
				vector<bool>	vValid(colPolys, true);
	
				// 어느 면 따라 미끄러졌을때 어느 면과 부딪히는가 검사한다.
				for (size_t i=0; i<colPolys; i++) 
				{
					// i번째 평면을 따라 미끄러졌을때 어느방향으로 미끄러지는가
					MVector3 moveVec, out;
					moveVec = m_destination + vPlanes[i].DotNormal(contactPointCenter - m_destination) * vPlanes[i].Normal();
					moveVec -= contactPointCenter;
					moveVec.Normalize();

					for (size_t j=0; j<colPolys; j++) 
					{
						// i번째 평면을 따라 미끄러졌을때 j번째 평면과 교차하는가
						if (i != j)
						{
							// 이동방향으로 쭉 연장해본다
							MVector3 t;
							t = contactPointCenter + moveVec * 10000;	
							// 그 면과 만났는가
							vvIntersects[i][j] = vPlanes[j].Intersect(out, contactPointCenter, t);
						}
						else	
							vvIntersects[i][j] = false;	// 무조건 안만난다
					}
				}

				// 어느 평면을 따라 미끄러질때 쭉 미끄러지지 못하고 다른 평면에 걸리는 평면은
				// 무효인 평면으로 처리한다.
				for (size_t i=0; i<colPolys; i++)
				{
					if (vValid[i])
					{
						for (size_t j=0; j<colPolys; j++) 
						{
							if ((i!=j) && (vValid[j]) && !vvIntersects[i][j])
								vValid[j] = false;
						}
					}
				}

				// 유효한 평면이 몇번몇번 평면인지 체크한다.	
				vector<int> vValidIndex;
				for (size_t i=0; i<colPolys; i++)
				{
					if (vValid[i])
						vValidIndex.push_back(static_cast<int>(i));
				}

				if (vValidIndex.size() == 1)	// 유효한 평면이 한개뿐이면
				{
					// 그 평면을 따라 미끄러진다.
					slidingDestination = m_destination + vPlanes[vValidIndex[0]].DotNormal(contactPointCenter - m_destination) * vPlanes[vValidIndex[0]].Normal();
				}
				else if (vValidIndex.size() == 2)	// 두개이면
				{
					// 두 평면의 교선을 따라 미끄러진다.
					MVector3 vIntersect = vPlanes[vValidIndex[0]].Normal().CrossProduct(vPlanes[vValidIndex[1]].Normal()); 
					vIntersect.Normalize();

					float fNextMove = vIntersect.DotProduct(vDir) * fDist;
					slidingDestination = contactPointCenter + vIntersect * fNextMove;
				}
				else	// 3개 이상이면 걸려서 더 이상 안움직인다. 여기서 종료
				{
					// 3개 이상의 평면이 평행인 경우도 있긴 하겠지만 일단 무시.
					// 나중에 필요해지면 별도 체크를 하겠음
					vOut = ret;
					return true;

					// 두 평면의 교선을 따라 미끄러진다.
// 					MVector3 vIntersect = vPlanes[vValidIndex[0]].Normal().CrossProduct(vPlanes[vValidIndex[2]].Normal()); 
// 					vIntersect.Normalize();
// 
// 					float fNextMove = vIntersect.DotProduct(vDir) * fDist;
// 					slidingDestination = contactPointCenter + vIntersect * fNextMove;
				}
			}

			MVector3 slidingOut;
			float f;

			// 위에서 알아낸 미끄러지는 방향에 따라 움직인다.
			// f에는 원래 갈려던 거리의 몇%나 움직였는지가 0~1까지 수치로 들어간다.
			bool b = MoveTest(contactPointCenter,slidingDestination,slidingOut, &f);
			vOut = slidingOut;

			// 움직이면서 아무데도 걸리지 않았다면 이대로 종료
			if (!b)
				return true;

			// 미끄러질때 바로 뭔가에 걸렸으면 거기서 정지
			// 그 폴리곤을 포함해서 같이 미끄러지는게 제일 좋지만 오차때문인지 잘안됨
			if (f <= 0.01)
				return true;

			// 아직 원래 갈려던 거리만큼(혹은 그렇게 갈려고 하다 걸려서 끝까지 미끄러지는 거리만큼)
			// 가지 못했을 경우 방금 이동한 거리를 이동할 거리에서 빼고 이동한 위치에서 다시 시작한다.
			fDist *= (1 - f);
			now = slidingOut;

			// 0.1정도 도달하지 못한건 애교로 봐주자.
			if (fDist < 0.1)
				return true;

		}
		else
		{
			// 충돌 의심 폴리곤이 있지만 정작 충돌하는 데는 없을경우 그냥 끝까지 간다.
			vOut = m_destination;
			// 충돌체크의 첫번째 루프(idx == 0)에서 걸리면 아예 전혀 충돌하지 않았기 때문에 false 리턴
			// idx가 0이 아닌 경우 true리턴
			return (idx != 0);
		}
	}

	// 충돌했3
	return true;
}


bool RCollision::MoveTestTerrain( const MVector3& vFrom, const MVector3& vDir, float fDist, MVector3& vOut, MVector3* pOrgForm /*= NULL*/, float* pAllowedHeight /*= NULL*/ )
{
	if (m_pTargetCollisionTree == NULL) return false;

	MVector3 tmpOrigin = vFrom;

	// 높이를 올려줄때... 허용된 높이값이 있으면 적용하고 아니면 기존값을 적용한다.
	float fUpHeight = HOVER_HEIGHT;
	if(pAllowedHeight && pOrgForm)
	{
		float fGap = tmpOrigin.z - pOrgForm->z;
		fUpHeight = *pAllowedHeight /*+ 3.0f*/ - fGap;
	}

	tmpOrigin.z += fUpHeight;

	// 이전 충돌 폴리곤들을 삭제
	Clear();

	// 시작점-끝점 세팅
	m_source = tmpOrigin;
	m_destination = tmpOrigin + vDir * fDist;

	CalcBoundingBox();

	// 어느어느 폴리곤에 걸리는지 알아본다.
	m_pTargetCollisionTree->GetCollidablePolygons(*this, ~RCollisionTriangle::WATER);

	// 아무데도 안걸리고 자유롭게 움직이는가?
	bool bFreeMove;

	MVector3 ret;

	// 걸리는 폴리곤이 없으면
	if(m_colPolygons.empty()) 
	{
		// 걸리는데가 없3
		ret = m_destination;
		bFreeMove = true;
	}
	else
	{
		float fMinMove = fDist;

		MVector3 *pContactPoint = NULL;

		bFreeMove = true;

		for(int i=0; i<(int)m_colPolygons.size(); i++ )
		{
			RCollisionPolygon &poly = m_colPolygons[i];

			bool bCheck = CheckAllowedHeight(poly.v[0], poly.v[1], poly.v[2], pOrgForm, pAllowedHeight);

			if(bCheck == false)
			{
				// 내 방향대로 평면까지 진행했을때 갈수있는 최대 거리
				float fMove = GetWalkableDistance(poly);

				// 최소값을 찾는다
				if(fMinMove>fMove) {
					fMinMove = fMove;
					pContactPoint = &(m_colPolygons[i].nearestPoint);
				}

				if ((poly.normal.DotProduct(MVector3::AXISZ) < SLIDING_LIMIT) && (fMove < 0.5f))
				{
					bFreeMove = false;
					vOut = vFrom;
					return bFreeMove;
				}
			}
		}

		// 일단 가능한 거리만큼 움직임
		ret = m_source + vDir * (fMinMove);	

	}

	// 위로 띄워서 이동했으니 도로 아래로 내려본다.
	MVector3 t = ret + MVector3(0, 0, -(fUpHeight+1));
	
	MVector3 tmpOut;

	MoveTest(ret, t, tmpOut);

	for(int i=0; i<(int)m_colPolygons.size(); i++ )
	{
		RCollisionPolygon &poly = m_colPolygons[i];
		if (poly.normal.DotProduct(MVector3::AXISZ) < SLIDING_LIMIT)		
		{
			bool bCheck = CheckAllowedHeight(poly.v[0], poly.v[1], poly.v[2], pOrgForm, pAllowedHeight);

			if (CheckTangency(tmpOut, poly) && bCheck == false)	// 접해있으면
				bFreeMove = false;
		}
	}

	vOut = tmpOut;

	if (!bFreeMove)
	{
		vOut = vFrom;
		return bFreeMove;
	}

	return bFreeMove;
}

void RCollision::CalcBoundingBox()
{
	MBox box;
	GetBoundingBox(box);

	m_bbMovement.minx = min( m_source.x + box.minx, m_destination.x + box.minx);
	m_bbMovement.miny = min( m_source.y + box.miny, m_destination.y + box.miny);
	m_bbMovement.minz = min( m_source.z + box.minz, m_destination.z + box.minz);
	m_bbMovement.maxx = max( m_source.x + box.maxx, m_destination.x + box.maxx);
	m_bbMovement.maxy = max( m_source.y + box.maxy, m_destination.y + box.maxy);
	m_bbMovement.maxz = max( m_source.z + box.maxz, m_destination.z + box.maxz);

}

bool RCollision::CheckAllowedHeight( const MVector3& v1, const MVector3& v2, const MVector3& v3, const MVector3* pOrgFrom, const float* pAllowedHeight )
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


// ===========================  sphere
RCollisionSphere::RCollisionSphere(float fRadius) : RCollision()
{
	SetRadius(fRadius);
}

RCollisionSphere::~RCollisionSphere()
{
}


void RCollisionSphere::SetRadius(float fRadius)
{ 
	m_fRadius = fRadius;
}

float RCollisionSphere::GetRadius()
{
	return m_fRadius;
}

MVector3 RCollisionSphere::GetContactPoint(const MVector3 &vPos, const MVector3 &vNorm)
{
	return vPos - (vNorm * (m_fRadius - 10));
}


float RCollisionSphere::GetWalkableDistance(const RCollisionPolygon &collisionPolygon)
{
	// (reverse-intersecting the sphere) 
	// sphere 가 source->destination 으로 이동하는 도중 collisionPoint 걸리는 지점을 찾는 것이므로
	// collisionPoint를 중심, 같은 반지름을 가진 sphere 에 source->destination 선분이 교차하는 지점과 같다.

	
	MVector3 dir = m_destination - m_source;
	float fDistance = dir.Length();
	dir.Normalize();		

	float fCollisionDistance;
	if(MMath::IsIntersectRaySphere(m_source, dir,collisionPolygon.nearestPoint,m_fRadius,fCollisionDistance)) {
		const float CONTACTTOLER = 0.01f;

		if(fabs(fCollisionDistance)<CONTACTTOLER) fCollisionDistance = 0;	// 접해있으면 더 진행할수 없다
		return min(fCollisionDistance , fDistance);
    }

	// 충돌하지 않으면 끝까지 갈수있다.
	return fDistance;
}

bool RCollisionSphere::IsCollidable(const MBox &box)
{
	return m_bbMovement.ContainsAABB(box);
}

bool RCollisionSphere::CheckPolygon(const MVector3 &v0,const MVector3 &v1,const MVector3 &v2, const DWORD dwAttribute /*= 0*/)
{
	MPlane plane(v0,v1,v2);

	MVector3 dir = m_destination - m_source;
	dir.Normalize();

	// 이동방향에 대해 backface 이면 체크하지 않는다.
	if(plane.DotNormal(dir)>=0) return false;

	int nDebugFlag;
	MVector3 nearestPoint;
	if(IsIntersectMovingSphereTriangle(m_source,m_destination,m_fRadius-0.1f,v0,v1,v2,nearestPoint,nDebugFlag)) {
		float dot = plane.Normal().DotProduct(MVector3::AXISZ);
		if ((dot < SLIDING_LIMIT) && (dot > 0.2))	// 수직폴리곤도 잘라내면 빠지는 경우가 생긴다.
		{											// 적당히 어중간한 애들만 잘라내게 0.2 체크 (적당히 하드코딩- _-)
			float fMaxHeight = FLT_MIN;
			if (fabs(v0.z - v1.z) > fMaxHeight) fMaxHeight = fabs(v0.z - v1.z);
			if (fabs(v0.z - v2.z) > fMaxHeight) fMaxHeight = fabs(v0.z - v2.z);
			if (fabs(v2.z - v1.z) > fMaxHeight) fMaxHeight = fabs(v2.z - v1.z);
 			if (fMaxHeight < STEEPPLANE_CUTOFF_HEIGHT) 
			{
				return true;
			}
		}
		m_colPolygons.push_back(RCollisionPolygon(v0, v1, v2, nearestPoint, plane.Normal(), nDebugFlag, dwAttribute));
	}

	return true;
}

void RCollisionSphere::GetBoundingBox(MBox &outBox)
{
	outBox.minx = outBox.miny = outBox.minz = -m_fRadius;
	outBox.maxx = outBox.maxy = outBox.maxz = m_fRadius;
}

/// 충돌 검사 펑션
// fRadius의 반경을 갖는 sphere가 from에서 to로 이동할때 삼각형(v0,v1,v2)와 충돌하는지 판단한다.
bool RCollisionSphere::IsIntersectMovingSphereTriangle(const MVector3 &from,const MVector3 &to,const float fRadius,const MVector3 &v0,const MVector3 &v1,const MVector3 &v2, MVector3 &outNearest, int &debugFlag)
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
		// from-to 선분위의 평면에 가장가까운 점
		nearestOnFromTo = from + min(1.0f,(fDistFromPlane - fRadius) / (fDistFromPlane - fDistToPlane)) * (to-from);

		// nearestPtOnFromTo 와 삼각형평면의 거리가 반지름을 초과하면 부딪힐수가 없다.
		if(plane.GetDistanceTo(nearestOnFromTo)>fRadius+INTERSECT_TOLER) return false;

		// nearestPtOnFromTo와 가장 가까운 평면위의 점 ( = from-to 선분과 가장 가까운 삼각형 평면위의 점)
		planeIntersectionPoint = MMath::GetNearestPoint(nearestOnFromTo,plane);
	}

	// 이 점이 삼각형 안에 있는지 검사
	// 각각의 edge를 포함하고 나머지 한 점쪽을 바라보는 평면들에 대해 모두 + 방향이면
	if(MPlane(CrossProduct(plane.Normal(),v1-v0),v0).GetDistanceTo(planeIntersectionPoint)>0 &&
		MPlane(CrossProduct(plane.Normal(),v2-v1),v1).GetDistanceTo(planeIntersectionPoint)>0 &&
		MPlane(CrossProduct(plane.Normal(),v0-v2),v2).GetDistanceTo(planeIntersectionPoint)>0 ) { // nearestPtOnPlane은 삼각형 내부의 점이다
			if(LengthSq(nearestOnFromTo-from)<LengthSq(to-from)) {
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
	if(DotProduct(revDir,(polygonIntersectionPoint-from))>- MMath::TOLER) return false;

	if(MMath::IsIntersectRaySphere( polygonIntersectionPoint, revDir, from, fRadius, colDistance)) {
		if(colDistance < (to-from).Length()) {
			outNearest = polygonIntersectionPoint;
			debugFlag = 1;
			return true;
		}
	}

	return false;

}



bool RCollisionSphere::CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly)
{
	float fDist = (vPos - poly.nearestPoint).Length();
	return (fDist <= GetRadius() + 0.01f);	// 0.01은 FP오차를 위한 보정
}

//////////////////////////////////////////////////////////////////////////
//Cone

RCollisionMovingCone::RCollisionMovingCone(float fHeght, float fRadius, float fDistance, MMatrix mView)
{
	m_fRadius = fRadius;
	m_fHeight = fHeght;
	m_View = mView;
	m_fInitDistance = m_fDistance = fDistance;
}

void RCollisionMovingCone::GetBoundingBox(MBox &outBox)
{
	outBox.minx = outBox.miny = -m_fRadius;
	outBox.minz = -m_fHeight;
	outBox.maxx = outBox.maxy = m_fRadius;
	outBox.maxz = 0.0f;
}

bool RCollisionMovingCone::IsCollidable(const MBox &box)
{
	MBox ConeBox;
	GetBoundingBox(ConeBox);

	MBox out;
	TransformBox(&out, box, m_View);

	return ConeBox.ContainsAABB(out);
}

bool RCollisionMovingCone::CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute /*= 0*/)
{
	MVector3 v0Transe, v1Transe, v2Transe;
	
	v0Transe = lv0 * m_View;
	v1Transe = lv1 * m_View;
	v2Transe = lv2 * m_View;

	float Dist;
	if(v0Transe.z <=0 || v1Transe.z <=0)
	{
		Dist = GetNearestDistance(v0Transe, v1Transe);
		if(Dist < m_fDistance)
		{
			if(Dist >0)
				m_fDistance = Dist;
		}
	}

	if(v1Transe.z <=0 || v2Transe.z <=0)
	{
		Dist = GetNearestDistance(v1Transe, v2Transe);
		if(Dist < m_fDistance)
		{
			if(Dist >0)
				m_fDistance = Dist;
		}
	}
	if(v2Transe.z <=0 || v0Transe.z <= 0)
	{
		Dist = GetNearestDistance(v2Transe, v0Transe);
		if(Dist < m_fDistance)
		{
			if(Dist >0)
				m_fDistance = Dist;
		}
	}
	return true;

}

float RCollisionMovingCone::GetNearestDistance(MVector3& vBegin, MVector3& vEnd)
{
	MVector3 vDirection = vEnd - vBegin;
	MVector3 vNearestPoint;

	MVector3 vProjBegin, vProjEnd, vProjDir;
	vProjBegin = MVector3(vBegin.x, vBegin.y, 0.0f);
	vProjEnd = MVector3(vEnd.x, vEnd.y, 0.0f);
	vProjDir = MVector3(vDirection.x, vDirection.y, 0.0f);

	if(vProjEnd == vProjBegin)	//z축만 다르고 x,y축이 같은 경우 (프로젝션 시키면 점으로 보인다.)
	{
		float d = max(vProjBegin.z, vProjEnd.z);
		if(d > 0) d = 0.0f;  // 큰값이 0보다 크면 0에 맞춰준다. 
		return vProjBegin.Length() * m_fHeight / m_fRadius - d;
	}

	MVector3 vProjBeginNorm = vProjBegin;
	vProjBeginNorm.Normalize();
	MVector3 vProjDirNorm = vProjDir;
	vProjDirNorm.Normalize();

	if(vProjDirNorm == vProjBeginNorm)		//원의 반지름 방향으로 두 점이 놓여있다면
	{
		MVector3 vDirNorm = vDirection;
		vDirNorm.Normalize();
		float fDirCos = DotProduct(MVector3(0,0,1), vDirNorm);
		float fConeCos = m_fHeight / sqrtf(m_fHeight*m_fHeight + m_fRadius* m_fRadius);
		if(fabs(fDirCos) > fConeCos) //더 가파르다면
		{
			vNearestPoint = vBegin.z > vEnd.z ? vBegin : vEnd;
		}
		else
		{
			vNearestPoint = vBegin.z < vEnd.z ? vBegin : vEnd;
		}
	}
	else
	{
		float t = - (vBegin.x * vDirection.x + vBegin.y * vDirection.y) / (vDirection.x * vDirection.x + vDirection.y * vDirection.y);

		if(t <=0)
		{
			vNearestPoint = vBegin;
		}
		else if(t >=1)
		{
			vNearestPoint = vEnd;
		}
		else
		{
			vNearestPoint = vBegin + vDirection * t;
		}
	}

	if(vNearestPoint.z <=0 )	//begin end의 z 값도 같이 비교해본다. 
	{
		float fDist = MVector3(vNearestPoint.x, vNearestPoint.y, 0.0f).Length() * m_fHeight / m_fRadius - vNearestPoint.z;

		MVector3 UpperPoint  = vBegin.z > vEnd.z ? vBegin : vEnd;
		MVector3 LowerPoint = vBegin.z <= vEnd.z ? vBegin : vEnd;
		if(UpperPoint.z > 0)
		{
			MVector3 vDir = vNearestPoint - UpperPoint;
			float k = - UpperPoint.z / vDir.z;
			UpperPoint = UpperPoint + vDir * k;		
		}

		float fUpperPointRaius = MVector3(UpperPoint.x, UpperPoint.y, 0.0f).Length();
		if(fUpperPointRaius <=  m_fRadius)
		{
			float fUpperPointDist = fUpperPointRaius * m_fHeight / m_fRadius - UpperPoint.z;
			fDist = min(fDist, fUpperPointDist);
		}
		else
		{
			float r1 = MVector3(LowerPoint.x, LowerPoint.y,0).Length();
			float r2 = m_fRadius;
			float r3 = fUpperPointRaius;
			if(r1 > m_fRadius)
			{
				return fDist;
			}
			float zt = (r2 - r1) * (fabs(LowerPoint.z) - fabs(UpperPoint.z)) / (r3 - r1) + fabs(UpperPoint.z);
			float fResult = m_fHeight + zt;
			fDist = min(fDist, fResult);
		}

		return fDist;
	}
	else		//찾은 점이 카메라 앞에 있다면 적어도 카메라랑 z 값이 같은 위치를 라인 상에서 찾아준다. 
	{
		MVector3 vUnderZVector = vBegin.z < vEnd.z ? vBegin : vEnd;
		if(vUnderZVector.z > 0) //말이 안되는 경우지만 혹시나 하는 마음에..두 점다 z위에 있는 경우는 스킵해버렸기 때문에..스킵했었나??
		{
			return m_fInitDistance;	
		}
		MVector3 vDir = vNearestPoint - vUnderZVector;
		float k = - vUnderZVector.z / vDir.z;
		MVector3 vZeroZVector = vUnderZVector + vDir * k;
		return MVector3(vZeroZVector.x, vZeroZVector.y, 0.0f).Length() * m_fHeight / m_fRadius;
	}
}

}

