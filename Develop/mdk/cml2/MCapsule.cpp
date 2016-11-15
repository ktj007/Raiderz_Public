#include "stdafx.h"
#include "MCapsule.h"
#include "MMath.h"

bool MCapsule::CollisionSphere(const MVector3 &center, float rad) const
{
	return (MMath::GetDistancePointLineSegment(center, bottom, top) < radius + rad);
}

bool MCapsule::CollisionCapsule(const MCapsule &capsule) const
{
	if ((height < FLT_EPSILON) && (capsule.height < FLT_EPSILON))
		return bottom.DistanceTo(capsule.bottom) < radius + capsule.radius;
	else if (capsule.height < FLT_EPSILON)
		return CollisionSphere(capsule.bottom, capsule.radius);
	else if (height < FLT_EPSILON)
		return capsule.CollisionSphere(bottom, radius);

	return (MMath::GetDistanceSegment(bottom, top, capsule.bottom, capsule.top) < radius + capsule.radius);
}

bool MCapsule::CollisionSegment(const MVector3& a, const MVector3& b) const
{
	return GetDistanceToSegment(a, b) < radius;
}

float MCapsule::GetDistanceToSegment(const MVector3& a, const MVector3& b) const
{
	if (height < FLT_EPSILON)
		return (MMath::GetDistancePointLineSegment(bottom, a, b));
	else
		return (MMath::GetDistanceSegment(bottom, top, a, b));
}

bool MCapsule::IntersectRay(const MVector3& rayOrigin, const MVector3& rayDir, float &outDistance) const
{
	MVector3 c = MMath::GetNearestPointOnLineRay(bottom, top, rayOrigin, rayDir);

	MVector3 originToC = c - rayOrigin;

	// originToCenter를 직선으로 투영한 거리
	float fRayProjOriginToCenter = originToC.DotProduct(rayDir);

	// 방향이 반대인 경우
	if (fRayProjOriginToCenter < 0.0f)
	{
		outDistance = c.DistanceTo(rayOrigin);
		return false;
	}

	// 직선과 구의 center와의 거리의 제곱
	float distanceRayToCenterSq = originToC.LengthSq() - fRayProjOriginToCenter*fRayProjOriginToCenter;

	// 직선과 구의 중심과 가장가까운점 - 충돌지점의 거리의 제곱
	float d = radius * radius - distanceRayToCenterSq;

	if(d < 0 ) 
	{
		outDistance = MMath::GetDistancePointRay(c, rayOrigin, rayDir) - radius;
		return false;
	}
	else
	{
		outDistance = fRayProjOriginToCenter - sqrt(d);
		return true;
	}

	return true;
}

void MCapsule::SetHeight(float h)
{
	MVector3 dir = top - bottom;

	dir.Normalize();

	top = bottom + dir * h;
}

void MCapsule::CalcHeight()
{
	height = (top - bottom).Length();
}

MVector3 MCapsule::GetDir() const
{
	MVector3 dir = top - bottom;

	dir.Normalize();
	return dir;
}

MCapsule MCapsule::Multiply(const MMatrix& m) const
{
	MCapsule ret;

	m.TransformVect(bottom, ret.bottom);
	m.TransformVect(top, ret.top);

	ret.radius = radius;

	ret.CalcHeight();

	if(ret.height != 0.0f &&
		height != 0.0f)
	{
		ret.radius *= ret.height / height;
	}

	return ret;
}


void MCapsule::Scale(float f)
{
	bottom = bottom * f;
	top = top * f;
	radius = radius * f;
	height = height * f;
}

MVector3 MCapsule::GetCenter() const
{
	MVector3 velHeight = bottom - top;
	if(velHeight != MVector3::ZERO)
	{
		return (velHeight / 2) + top;				// 센터
	}

	return bottom;
}


void MCapsule::makeOBB( MOBox *pOBB )
{
	_ASSERT(NULL != pOBB);
	pOBB->vAxis[1] = (this->top - this->bottom).Normalize();
	if(	MMath::Equals(pOBB->vAxis[1],MVector3(0,0, 1),0.2f)	|| 
		MMath::Equals(pOBB->vAxis[1],MVector3(0,0,-1),0.2f)		)
	{
		pOBB->vAxis[0] = pOBB->vAxis[1].CrossProduct(MVector3(0,1,0));
		pOBB->vAxis[2] = pOBB->vAxis[0].CrossProduct(pOBB->vAxis[1]);
	}
	else
	{
		pOBB->vAxis[0] = pOBB->vAxis[1].CrossProduct(MVector3(0,0,1));
		pOBB->vAxis[2] = pOBB->vAxis[0].CrossProduct(pOBB->vAxis[1]);
	}
	pOBB->vPosition = (this->top + this->bottom) * 0.5f ;
	pOBB->vRange = MVector3(this->radius,this->height*0.5f + this->radius,this->radius);
}