#include "stdafx.h"
#include "MMath.h"
#include "MBox.h"
#include "MObox.h"
#include "MMatrix3.h"

MOBox::MOBox(const MMatrix &_transform)
{
	MVector3 axisX(_transform._11, _transform._21, _transform._31);
	MVector3 axisY(_transform._12, _transform._22, _transform._32);
	MVector3 axisZ(_transform._13, _transform._23, _transform._33);

	vRange.x = axisX.Length();
	vRange.y = axisY.Length();
	vRange.z = axisZ.Length();

	vAxis[0] = axisX / vRange.x;
	vAxis[1] = axisY / vRange.y;
	vAxis[2] = axisZ / vRange.z;

	vPosition = MVector3(_transform._41, _transform._42, _transform._43);
}

MOBox::MOBox(const MVector3 &_xaxis, const MVector3 &_yaxis, const MVector3 &_zaxis, const MVector3 &_range, const MVector3 &_position)
: vRange(_range)
, vPosition(_position)
{
	_ASSERT(MMath::Equals(_xaxis.Length(), 1.0f));
	_ASSERT(MMath::Equals(_yaxis.Length(), 1.0f));
	_ASSERT(MMath::Equals(_zaxis.Length(), 1.0f));

	vAxis[0] = _xaxis;
	vAxis[1] = _yaxis;
	vAxis[2] = _zaxis;
}

bool MOBox::Intersect( const MBox &aabb )
{
	MOBox obb;

	//AABB정보 obb형태로 변환
	obb.vPosition 	= (aabb.vmax + aabb.vmin) * 0.5f;
	obb.vAxis[0] = MVector3(1,0,0);
	obb.vAxis[1] = MVector3(0,1,0);
	obb.vAxis[2] = MVector3(0,0,1);
	obb.vRange[0] = (aabb.maxx - aabb.minx) * 0.5f;
	obb.vRange[1] = (aabb.maxy - aabb.miny) * 0.5f;
	obb.vRange[2] = (aabb.maxz - aabb.minz) * 0.5f;
	
	return intersect(*this,obb);
}

bool MOBox::Intersect( MOBox &rhs )
{
	return intersect(*this,rhs);
}

bool MOBox::intersect(MOBox &a, MOBox &b)
{
	float		ra, rb;
	MMatrix3	R, AbsR;

	// Compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R._m[i][j] = DotProduct(a.vAxis[i], b.vAxis[j]);

	// Compute translation vector t
	MVector3 t = b.vPosition - a.vPosition;
	// Bring translation into a's coordinate frame
	t = MVector3(DotProduct(t, a.vAxis[0]), DotProduct(t, a.vAxis[1]), DotProduct(t, a.vAxis[2]));

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR._m[i][j] = fabs(R._m[i][j]) + MMath::EPSILON;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {
		ra = a.vRange[i];
		rb = b.vRange[0] * AbsR._m[i][0] + b.vRange[1] * AbsR._m[i][1] + b.vRange[2] * AbsR._m[i][2];
		if (fabs(t[i]) > ra + rb) return 0;
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = a.vRange[0] * AbsR._m[0][i] + a.vRange[1] * AbsR._m[1][i] + a.vRange[2] * AbsR._m[2][i];
		rb = b.vRange[i];
		if (fabs(t[0] * R._m[0][i] + t[1] * R._m[1][i] + t[2] * R._m[2][i]) > ra + rb) return 0;
	}

	// Test axis L = A0 x B0
	ra = a.vRange[1] * AbsR._m[2][0] + a.vRange[2] * AbsR._m[1][0];
	rb = b.vRange[1] * AbsR._m[0][2] + b.vRange[2] * AbsR._m[0][1];
	if (fabs(t[2] * R._m[1][0] - t[1] * R._m[2][0]) > ra + rb) return 0;

	// Test axis L = A0 x B1
	ra = a.vRange[1] * AbsR._m[2][1] + a.vRange[2] * AbsR._m[1][1];
	rb = b.vRange[0] * AbsR._m[0][2] + b.vRange[2] * AbsR._m[0][0];
	if (fabs(t[2] * R._m[1][1] - t[1] * R._m[2][1]) > ra + rb) return 0;

	// Test axis L = A0 x B2
	ra = a.vRange[1] * AbsR._m[2][2] + a.vRange[2] * AbsR._m[1][2];
	rb = b.vRange[0] * AbsR._m[0][1] + b.vRange[1] * AbsR._m[0][0];
	if (fabs(t[2] * R._m[1][2] - t[1] * R._m[2][2]) > ra + rb) return 0;

	// Test axis L = A1 x B0
	ra = a.vRange[0] * AbsR._m[2][0] + a.vRange[2] * AbsR._m[0][0];
	rb = b.vRange[1] * AbsR._m[1][2] + b.vRange[2] * AbsR._m[1][1];
	if (fabs(t[0] * R._m[2][0] - t[2] * R._m[0][0]) > ra + rb) return 0;

	// Test axis L = A1 x B1
	ra = a.vRange[0] * AbsR._m[2][1] + a.vRange[2] * AbsR._m[0][1];
	rb = b.vRange[0] * AbsR._m[1][2] + b.vRange[2] * AbsR._m[1][0];
	if (fabs(t[0] * R._m[2][1] - t[2] * R._m[0][1]) > ra + rb) return 0;

	// Test axis L = A1 x B2
	ra = a.vRange[0] * AbsR._m[2][2] + a.vRange[2] * AbsR._m[0][2];
	rb = b.vRange[0] * AbsR._m[1][1] + b.vRange[1] * AbsR._m[1][0];
	if (fabs(t[0] * R._m[2][2] - t[2] * R._m[0][2]) > ra + rb) return 0;

	// Test axis L = A2 x B0
	ra = a.vRange[0] * AbsR._m[1][0] + a.vRange[1] * AbsR._m[0][0];
	rb = b.vRange[1] * AbsR._m[2][2] + b.vRange[2] * AbsR._m[2][1];
	if (fabs(t[1] * R._m[0][0] - t[0] * R._m[1][0]) > ra + rb) return 0;

	// Test axis L = A2 x B1
	ra = a.vRange[0] * AbsR._m[1][1] + a.vRange[1] * AbsR._m[0][1];
	rb = b.vRange[0] * AbsR._m[2][2] + b.vRange[2] * AbsR._m[2][0];
	if (fabs(t[1] * R._m[0][1] - t[0] * R._m[1][1]) > ra + rb) return 0;

	// Test axis L = A2 x B2
	ra = a.vRange[0] * AbsR._m[1][2] + a.vRange[1] * AbsR._m[0][2];
	rb = b.vRange[0] * AbsR._m[2][1] + b.vRange[1] * AbsR._m[2][0];
	if (fabs(t[1] * R._m[0][2] - t[0] * R._m[1][2]) > ra + rb) return 0;

	// Since no separating axis found, the OBBs must be intersecting
	return true;
}

bool MOBox::IsOut( MVector3& v )
{
	//v를 obb좌표계로
	MVector3 k = v - this->vPosition;
	MVector3 p (DotProduct(this->vAxis[0],k ) , DotProduct(this->vAxis[1],k ) , DotProduct(this->vAxis[2],k) );

	float absx = fabs(this->vRange.x);
	float absy = fabs(this->vRange.y);
	float absz = fabs(this->vRange.z);

	if( p.x < -absx || absx < p.x ) return true;
	if( p.y < -absy || absy < p.y ) return true;
	if( p.z < -absz || absz < p.z ) return true;

	return false;
}