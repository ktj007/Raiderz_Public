#include "stdafx.h"
#include "RTreeCollision.h"
#include "RRenderHelper.h"
#include "MMath.h"
#include "RViewFrustum.h"
#include "RTypes.h"

#include "RTreeResource.h"

namespace rs3 
{

RTreeCollisionObject::RTreeCollisionObject() :	m_Type(RTREECOLLISION_CAPSULE), 
												m_vPosition(RVector(0, 0, 0)),
												m_vEulerRotate(RVector(0, 0, 0)),
												m_fRadius(0),
												m_fWidth(0),
												m_fLength(0),
												m_fHeight(0)
{
}


RTreeCollision::RTreeCollision()
{
}


void RTreeCollision::UpdateCollisionObject(RTreeResource* _pTreeMesh)
{
	if (_pTreeMesh == NULL) return;
	if (_pTreeMesh->m_pSpeedTree == NULL) return;
	int n = _pTreeMesh->m_pSpeedTree->GetNumCollisionObjects();

	m_vCollisions.resize(n);

	float pos[3];
	float dimension[3];
	float eular[3];
	CSpeedTreeRT::ECollisionObjectType type;

	for (int i=0; i<n; i++)
	{
		RTreeCollisionObject& obj = m_vCollisions[i];

		_pTreeMesh->m_pSpeedTree->GetCollisionObject(i, type, pos, dimension, eular);

		obj.m_vPosition = (RVector)pos;
		obj.m_vEulerRotate = (RVector)eular;

		if (type == CSpeedTreeRT::CO_CAPSULE)
		{
			obj.m_Type = RTREECOLLISION_CAPSULE;
			obj.m_fRadius = fabs(dimension[0]);
			obj.m_fHeight = dimension[1];
		}
		else if (type == CSpeedTreeRT::CO_SPHERE)
		{
			obj.m_Type = RTREECOLLISION_SPHERE;
			obj.m_fRadius = dimension[0];
		}
		else
		{
			obj.m_Type = RTREECOLLISION_BOX;
			obj.m_fWidth = dimension[0];
			obj.m_fLength = dimension[1];
			obj.m_fHeight = dimension[2];
		}
	}
}

void RTreeCollision::RenderCollisionObject(const RMatrix& mTransform)
{
	RPFC_THISFUNC;
	RRenderHelper::SetRenderState();
	RMatrix mTrans;
	RMatrix t, tx, ty, tz;
	RMatrix tt, tt1, ttt;

	RVector v, vv;

	for (size_t i=0; i<m_vCollisions.size(); i++)
	{
		RTreeCollisionObject& obj = m_vCollisions[i];

		mTrans = mTransform;

		tt1 = tt = t = tx = ty = tz = RMatrix::IDENTITY;

//		tt.SetRotationY(MMath::HALFPI);

		tx.SetRotationX(DegreeToRadian(obj.m_vEulerRotate.x));
		ty.SetRotationY(DegreeToRadian(obj.m_vEulerRotate.y));
		tz.SetRotationZ(DegreeToRadian(obj.m_vEulerRotate.z));
		t.SetTranslation(obj.m_vPosition);

		tx *= ty;
		tx *= tz;
		tx *= t;

		mTrans = tx * mTrans;

		if (obj.m_Type == RTREECOLLISION_SPHERE)
		{
			RRenderHelper::RenderSphere(mTrans, obj.m_fRadius);
		}
		else if (obj.m_Type == RTREECOLLISION_CAPSULE)
		{
			tt *= mTrans;
			RRenderHelper::RenderCapsule(tt, obj.m_fRadius, obj.m_fHeight, 0x66ff0000);
		}
		else
		{
			RBoundingBox b;
			b.vmin.Set(-obj.m_fWidth / 2, -obj.m_fLength / 2, 0);
			b.vmax.Set(obj.m_fWidth / 2, obj.m_fLength / 2, obj.m_fHeight);
			RRenderHelper::RenderBox(mTrans, b, 0xFFFF0000);
		}	
	}
	RRenderHelper::EndRenderState();
}

bool RTreeCollision::GetCollision(const RMatrix& mTransform, const RVector& origin, const RVector& to, RVector& out)
{
	RVector dir = to - origin;
	float len = dir.Length();
	dir = dir.Normalize();
	RMatrix mTrans;
	RMatrix t, tx, ty, tz;
	RMatrix tt, tt1, ttt;
	vector<RVector> ov;
	RVector o;

	bool finded = false;

	for (size_t i=0; i<m_vCollisions.size(); i++)
	{
		RTreeCollisionObject& obj = m_vCollisions[i];

		mTrans = mTransform;

		tt1 = tt = t = tx = ty = tz = RMatrix::IDENTITY;

		tt.SetRotationY(MMath::HALFPI);

		tx.SetRotationX(DegreeToRadian(obj.m_vEulerRotate.x));
		ty.SetRotationY(DegreeToRadian(obj.m_vEulerRotate.y));
		tz.SetRotationZ(DegreeToRadian(obj.m_vEulerRotate.z));
		t.SetTranslation(obj.m_vPosition);

		tx *= ty;
		tx *= tz;
		tx *= t;

		mTrans = tx * mTrans;

		bool f = false;

		if (obj.m_Type == RTREECOLLISION_SPHERE)
		{
			f = GetCollisionSphere(mTrans, origin, len, dir, obj.m_fRadius, o);
		}
		else if (obj.m_Type == RTREECOLLISION_CAPSULE)
		{
			f = GetCollisionCapsule(mTrans, origin, len, dir, obj.m_fRadius, obj.m_fHeight, o);
		}
		else
		{
			f = GetCollisionBox(mTrans, origin, len, dir, obj.m_fWidth, obj.m_fLength, obj.m_fLength, o);
		}	

		if (f)
		{
			finded = true;
			ov.push_back(o);
		}
	}

	if (finded)
	{
		float dist = 999999999;
		float d;
		for (size_t i=0; i<ov.size(); i++)
		{
			d = origin.DistanceTo(ov[i]);
			if (d < dist)
			{
				dist = d;
				out = ov[i];
			}	
		}
		return true;
	}

	return false;
}


bool RTreeCollision::GetCollisionSphere(const RMatrix& mTransform, const RVector& origin, float length, const RVector& dir, float radius, RVector& out)
{
	RMatrix invTransform;
	float dd;
	if (!mTransform.GetInverse(&invTransform, &dd))
		return false;

	RVector movedOrigin;

	invTransform.TransformVect(origin, movedOrigin);
	RVector movedDir;
	invTransform.TransformNormal(dir, movedDir);


	float a = movedDir.DotProduct(movedDir);
	float b = 2 * movedOrigin.DotProduct(movedDir);
	float c = movedOrigin.DotProduct(movedOrigin) - (radius * radius);

	float d = (b*b) - (4*a*c);

	out.Set(0, 0, 0);

	if (d >= 0.0f)
	{
		float t = (-b-sqrt(d)) / (2 * a);
		if ((t <= length) && (t >= 0.0f))
		{
			RVector o = movedOrigin + movedDir * t;
			mTransform.TransformVect(o, out);
			return true;
		}
	}
	return false;
}


bool RTreeCollision::GetCollisionCapsule(const RMatrix& mTransform, const RVector& origin, float length, const RVector& dir, float radius, float fHeight, RVector& out)
{
	RMatrix invTransform;
	float d;
	RVector to;

	to = origin + dir * length;

	if (!mTransform.GetInverse(&invTransform, &d))
		return false;

	RVector movedOrigin;

	invTransform.TransformVect(origin, movedOrigin);

	RVector movedDir, movedTo;
	invTransform.TransformVect(to, movedTo);

	float movedLength;
	movedDir = movedTo - movedOrigin;
	movedLength = movedDir.Length();
	movedDir.Normalize();

	out.Set(0, 0, 0);

	RCapsule c;
 	c = RCapsule(RVector(0, 0, 0), RVector(0, 0, fHeight), radius);

	float tr = movedLength;
	float tl = 0;
	float t;


	if (c.GetDistanceToSegment(movedOrigin, movedTo) < radius)
	{
		for (int i=0; i<50; i++)
		{
			t = (tr + tl) / 2;
			movedTo = movedOrigin + movedDir * t;
			float d = c.GetDistanceToSegment(movedOrigin, movedTo);
			if (d < radius)
			{
				tr = t;
			}
			else
			{
				if (d < radius + 1.0f)
					break;
				else
					tl = t;
			}
		}
		mTransform.TransformVect(movedTo, out);
		return true;
	}
	else
	{
		return false;
	}
}

bool RTreeCollision::GetCollisionBox(const RMatrix& mTransform, const RVector& origin, float length, const RVector& dir, float width, float width_y, float height, RVector& out)
{
	RMatrix invTransform;
	float d;

	if (!mTransform.GetInverse(&invTransform, &d))
		return false;

	RVector movedOrigin;

	invTransform.TransformVect(origin, movedOrigin);

	RVector movedDir;

	invTransform.TransformNormal(dir, movedDir);

	float t;

	if (fabs(movedDir.x) > FLT_EPSILON)
	{
		if (movedDir.x > 0)
		{
			t = (width / -2.0f - movedOrigin.x) / movedDir.x;
			if ((t <= length) && (t >= 0.0f))
			{
				RVector o = movedOrigin + movedDir * t;
				if ((o.y <= width_y / 2.0f) && (o.y >= width_y / -2.0f))
				{
					if ((o.z <= height) && (o.z >= 0))
					{
						mTransform.TransformVect(o, out);
						return true;
					}
				}
			}
		}
		else
		{
			t = (width / 2.0f - movedOrigin.x) / movedDir.x;
			if ((t <= length) && (t >= 0.0f))
			{
				RVector o = movedOrigin + movedDir * t;
				if ((o.y <= width_y / 2.0f) && (o.y >= width_y / -2.0f))
				{
					if ((o.z <= height) && (o.z >= 0))
					{
						mTransform.TransformVect(o, out);
						return true;
					}
				}
			}
		}
	}

	if (fabs(movedDir.y) > FLT_EPSILON)
	{
		if (movedDir.y > 0)
		{
			t = (width_y / -2.0f - movedOrigin.y) / movedDir.y;
			if ((t <= length) && (t >= 0.0f))
			{
				RVector o = movedOrigin + movedDir * t;
				if ((o.x <= width / 2.0f) && (o.x >= width / -2.0f))
				{
					if ((o.z <= height) && (o.z >= 0))
					{
						mTransform.TransformVect(o, out);
						return true;
					}
				}
			}
		}
		else
		{
			t = (width_y / 2.0f - movedOrigin.y) / movedDir.y;
			if ((t <= length) && (t >= 0.0f))
			{
				RVector o = movedOrigin + movedDir * t;
				if ((o.x <= width / 2.0f) && (o.x >= width / -2.0f))
				{
					if ((o.z <= height) && (o.z >= 0))
					{
						mTransform.TransformVect(o, out);
						return true;
					}
				}
			}
		}
	}

	if (fabs(movedDir.z) > FLT_EPSILON)
	{
		if (movedDir.z > 0)
		{
			t = -movedOrigin.z / movedDir.z;
			if ((t <= length) && (t >= 0.0f))
			{
				RVector o = movedOrigin + movedDir * t;
				if ((o.x <= width / 2.0f) && (o.x >= width / -2.0f))
				{
					if ((o.y <= width_y / 2.0f) && (o.y >= width_y / -2.0f))
					{
						mTransform.TransformVect(o, out);
						return true;
					}
				}
			}
		}
		else
		{
			t = (height - movedOrigin.z) / movedDir.z;
			if ((t <= length) && (t >= 0.0f))
			{
				RVector o = movedOrigin + movedDir * t;
				if ((o.x <= width / 2.0f) && (o.x >= width / -2.0f))
				{
					if ((o.y <= width_y / 2.0f) && (o.y >= width_y / -2.0f))
					{
						mTransform.TransformVect(o, out);
						return true;
					}
				}
			}
		}
	}


	return false;
}


bool RTreeCollision::FrustumTest(const RMatrix& mTransform, const RViewFrustum& frustum, const RBoundingBox& aabb)
{
	if (m_vCollisions.size() == 0)
		return frustum.TestBox(aabb);

	RMatrix mTrans;
	RMatrix t, tx, ty, tz;
	RMatrix tt, tt1, ttt;

	for (size_t i=0; i<m_vCollisions.size(); i++)
	{
		RTreeCollisionObject& obj = m_vCollisions[i];

		mTrans = mTransform;

		tt1 = tt = t = tx = ty = tz = RMatrix::IDENTITY;

		tt.SetRotationY(MMath::HALFPI);

		tx.SetRotationX(DegreeToRadian(obj.m_vEulerRotate.x));
		ty.SetRotationY(DegreeToRadian(obj.m_vEulerRotate.y));
		tz.SetRotationZ(DegreeToRadian(obj.m_vEulerRotate.z));
		t.SetTranslation(obj.m_vPosition);

		tx *= ty;
		tx *= tz;
		tx *= t;

		mTrans = tx * mTrans;

		RVector oPoint;

		mTrans.TransformVect(RVector::ZERO, oPoint);

		bool f = false;
		float scale = mTransform._11;

		if (obj.m_Type == RTREECOLLISION_SPHERE)
		{
			RBoundingSphere sp;
			sp.radius = obj.m_fRadius * scale;
			sp.center = oPoint;

			f = frustum.TestSphere(sp);
		}
		else 
		{
			RBox box;
			RVector t[8];
			if (obj.m_Type == RTREECOLLISION_CAPSULE)
			{
				t[0].Set(obj.m_fRadius, obj.m_fRadius, 0);
				t[1].Set(obj.m_fRadius, -obj.m_fRadius, 0);
				t[2].Set(-obj.m_fRadius, obj.m_fRadius, 0);
				t[3].Set(-obj.m_fRadius, -obj.m_fRadius, 0);
				t[4].Set(obj.m_fRadius, obj.m_fRadius, obj.m_fHeight);
				t[5].Set(obj.m_fRadius, -obj.m_fRadius, obj.m_fHeight);
				t[6].Set(-obj.m_fRadius, obj.m_fRadius, obj.m_fHeight);
				t[7].Set(-obj.m_fRadius, -obj.m_fRadius, obj.m_fHeight);
			}
			else
			{
				t[0].Set(obj.m_fWidth / 2, obj.m_fLength / 2, 0);
				t[1].Set(obj.m_fWidth / 2, -obj.m_fLength / 2, 0);
				t[2].Set(-obj.m_fWidth / 2, obj.m_fLength / 2, 0);
				t[3].Set(-obj.m_fWidth / 2, -obj.m_fLength / 2, 0);
				t[4].Set(obj.m_fWidth / 2, obj.m_fLength / 2, obj.m_fHeight);
				t[5].Set(obj.m_fWidth / 2, -obj.m_fLength / 2, obj.m_fHeight);
				t[6].Set(-obj.m_fWidth / 2, obj.m_fLength / 2, obj.m_fHeight);
				t[7].Set(-obj.m_fWidth / 2, -obj.m_fLength / 2, obj.m_fHeight);
			}

			box.Initialize();
			for (int i=0; i<8; i++)
			{
				box.Add(t[i]);
			}

			RMatrix inv;
			float d;

			 mTrans.GetInverse(&inv, &d);

			if (d == 0)
				continue;


			RViewFrustum tFrustum;
			const RPlane* p = frustum.GetPlanes();

			for (int i=0; i<6; i++)
			{
				RPlane tp;
				inv.TransformPlane(p[i], tp);
				tFrustum.SetPlane(i, tp);
			}
		
			f = tFrustum.TestBox(box);
//			f = false;
		}
		
		if (f)
			return true;
	}
	return false;
}



void RTreeCollision::GetCollisionPolygons(const RMatrix& mTransform, vector<RVector>& out)
{
	out.clear();

	RMatrix mTrans;
	RMatrix t, tx, ty, tz;
	RMatrix tt, tt1, ttt;

	RVector v, vv;

	for (size_t i=0; i<m_vCollisions.size(); i++)
	{
		RTreeCollisionObject& obj = m_vCollisions[i];

		mTrans = mTransform;

		tt1 = tt = t = tx = ty = tz = RMatrix::IDENTITY;

		tt.SetRotationY(MMath::HALFPI);

		tx.SetRotationX(DegreeToRadian(obj.m_vEulerRotate.x));
		ty.SetRotationY(DegreeToRadian(obj.m_vEulerRotate.y));
		tz.SetRotationZ(DegreeToRadian(obj.m_vEulerRotate.z));
		t.SetTranslation(obj.m_vPosition);

		tx *= ty;
		tx *= tz;
		tx *= t;

		mTrans = tx * mTrans;

		if (obj.m_Type == RTREECOLLISION_SPHERE)
		{
			// spherical coordinate(구면좌표계)공식 사용
			// vd .       . vc
			//
			// va .       . vb
			RVector va,vb,vc,vd;
			const int SPHERE_SEGMENT = 8;
			for(int i=0;i<SPHERE_SEGMENT;i++)
			{
				//z축으로의 각도 [ -90도 ~ 90 도 ]
				float fAngleZ  = -0.5f*MMath::PI +  i    * MMath::PI /(float)SPHERE_SEGMENT;
				float fAngleZ2 = -0.5f*MMath::PI + (i+1) * MMath::PI /(float)SPHERE_SEGMENT;
				for(int j=0;j<SPHERE_SEGMENT;j++)
				{
					//xy평면에서의 각도 [ 0도 ~ 360도 ]
					float fAngle=j*2*MMath::PI/(float)SPHERE_SEGMENT;
					float fAngle2=(j+1)*2*MMath::PI/(float)SPHERE_SEGMENT;

					RVector a=RVector( cos(fAngleZ )*cos(fAngle ),	cos(fAngleZ )*sin(fAngle ),	sin(fAngleZ ) ) * obj.m_fRadius;
					RVector b=RVector( cos(fAngleZ )*cos(fAngle2),	cos(fAngleZ )*sin(fAngle2),	sin(fAngleZ ) ) * obj.m_fRadius;
					RVector c=RVector( cos(fAngleZ2)*cos(fAngle2),	cos(fAngleZ2)*sin(fAngle2),	sin(fAngleZ2) ) * obj.m_fRadius;
					RVector d=RVector( cos(fAngleZ2)*cos(fAngle ),	cos(fAngleZ2)*sin(fAngle ),	sin(fAngleZ2) ) * obj.m_fRadius;
					
					mTrans.TransformVect(a,va);
					mTrans.TransformVect(b,vb);
					mTrans.TransformVect(c,vc);
					mTrans.TransformVect(d,vd);
				
					//ccw형태로 삼각형 폴리곤 2개를 만들어줌 => 밖에서 안으로 못들어옴
					out.push_back(vb);
					out.push_back(vc);
					out.push_back(va);
				
					out.push_back(vc);
					out.push_back(vd);
					out.push_back(va);
				}
			}
			
		}
		else if (obj.m_Type == RTREECOLLISION_CAPSULE)
		{
			const int CYLINDER_SEGMENT = 10;
			RVector	cylinderVertices[CYLINDER_SEGMENT*2];

			// TODO : sphere 처럼 한번만 초기화 하고 scale을 써서 렌더링하도록 변경
			for( int i=0; i<CYLINDER_SEGMENT; i++ )
			{
				float theta = (2*MMath::PI*i)/(CYLINDER_SEGMENT-1);
				cylinderVertices[2*i+0] = RVector(obj.m_fRadius * sinf(theta), obj.m_fRadius * cosf(theta), obj.m_fHeight);
				cylinderVertices[2*i+1] = RVector(obj.m_fRadius * sinf(theta), obj.m_fRadius * cosf(theta), 0);
			}

			for( int i=0; i<CYLINDER_SEGMENT*2; i++ )			
			{
				RVector v;
				mTrans.TransformVect(cylinderVertices[i], v);
				cylinderVertices[i] = v;
			}

			for( int i=0; i<CYLINDER_SEGMENT*2-2; i++ )			
			{
				if (i % 2 == 1)
				{
					out.push_back(cylinderVertices[i]);
					out.push_back(cylinderVertices[i+1]);
					out.push_back(cylinderVertices[i+2]);
				}
				else
				{
					out.push_back(cylinderVertices[i]);
					out.push_back(cylinderVertices[i+2]);
					out.push_back(cylinderVertices[i+1]);
				}
			}

		}
		else
		{
		}
	}
}


}