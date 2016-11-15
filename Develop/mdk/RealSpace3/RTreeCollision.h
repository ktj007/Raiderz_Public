#pragma once

#include "RTypes.h"


namespace rs3 {


enum RTreeCollisionType
{
	RTREECOLLISION_CAPSULE = 0,
	RTREECOLLISION_SPHERE,
	RTREECOLLISION_BOX
};

class RTreeResource;
class RViewFrustum;


class RS_API RTreeCollisionObject
{
public:
		RTreeCollisionType				m_Type;
		RVector							m_vPosition;
		RVector							m_vEulerRotate;
		float							m_fRadius;
		float							m_fHeight;		//z
		float							m_fWidth;		//x
		float							m_fLength;		//y

		RTreeCollisionObject();
};



class RS_API RTreeCollision
{
protected:
		bool							GetCollisionSphere(const RMatrix& mTransform, const RVector& origin, float length, const RVector& dir, float radius, RVector& out);
		bool							GetCollisionCapsule(const RMatrix& mTransform, const RVector& origin, float length, const RVector& dir, float radius, float fHeight, RVector& out);
		bool							GetCollisionBox(const RMatrix& mTransform, const RVector& origin, float length, const RVector& dir, float width, float width_y, float height, RVector& out);
		
public:
		vector<RTreeCollisionObject>	m_vCollisions;
		
		RTreeCollision();

		//void							UpdateCollisionObject(RTreeResource* pTree);
		void							UpdateCollisionObject(RTreeResource* _pTreeMesh);

		void							RenderCollisionObject(const RMatrix& mTransform);
		bool							GetCollision(const RMatrix& mTransform, const RVector& origin, const RVector& to, RVector& out);
		bool							FrustumTest(const RMatrix& mTransform, const RViewFrustum& frustum, const RBoundingBox& aabb);

		void							GetCollisionPolygons(const RMatrix& mTransform, vector<RVector>& out);
};




}