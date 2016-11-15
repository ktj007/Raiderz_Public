#pragma once

#include "MPhysXDef.h"


namespace physx {

class MPhysXResourceFactory
{
public:

	//////////////////////////////////////////////////////////////////////////
	// NxActor 생성 도우미 함수
	//
	static NxActor*				CreateGroundPlane(NxScene* scene);
	static NxActor*				CreateBox(NxScene* scene, const NxVec3& pos, const NxVec3& boxDim, const NxReal density);

	static NxActor*				CreateSphere(NxScene* scene, const NxVec3& pos, const NxReal radius, const NxReal density);

	static NxActor*				CreateCapsule(NxScene* scene, const NxVec3& actorpos, const NxVec3& capsuleLocalPos, const NxReal height, const NxReal radius, const NxReal density, void* pUserData = NULL);
	static NxActor*				CreateCapsule(NxScene* scene, const NxVec3& actorpos, const NxVec3& localTPos, const NxVec3& localBPos, const NxReal radius, const NxReal density, void* pUserData = NULL);
	static NxActor*				CreateCapsule(NxScene* scene, const NxVec3& actorpos, const NxReal height, const NxReal radius, const NxReal density);


	//////////////////////////////////////////////////////////////////////////
	// Shape Desc 생성 도우미 함수
	//
	static NxCapsuleShapeDesc*	CreateCapsuleShape(const NxVec3& localPos, const NxReal height, const NxReal radius, void* pUserData = NULL);
	static NxCapsuleShapeDesc*	CreateCapsuleShape(const NxVec3& localTPos, const NxVec3& localBPos, const NxReal radius, void* pUserData = NULL);


	//////////////////////////////////////////////////////////////////////////
	// Joint 생성 도우미 함수
	//
	static NxFixedJoint*		CreateFixedJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxRevoluteJoint*		CreateRevoluteJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxSphericalJoint*	CreateSphericalJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxPrismaticJoint*	CreatePrismaticJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxCylindricalJoint*	CreateCylindricalJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxPointOnLineJoint*	CreatePointOnLineJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxPointInPlaneJoint*	CreatePointInPlaneJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxPulleyJoint*		CreatePulleyJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& pulley0, const NxVec3& pulley1, const NxVec3& globalAxis, NxReal distance, NxReal ratio, const NxMotorDesc& motorDesc);
	static NxDistanceJoint*		CreateDistanceJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& anchor0, const NxVec3& anchor1, const NxVec3& globalAxis);

	static NxSphericalJoint*	CreateRopeSphericalJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxSphericalJoint*	CreateClothSphericalJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxSphericalJoint*	CreateBodySphericalJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxRevoluteJoint*		CreateWheelJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxRevoluteJoint*		CreateStepJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);

	static NxRevoluteJoint*		CreateChassisJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);
	static NxFixedJoint*		CreateCannonJoint(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);

	static NxSphericalJoint*	CreateBladeLink(NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis);

	static NxRevoluteJoint*		CreateRevoluteJoint2(NxScene* scene, NxActor* a0, NxActor* a1,const NxVec3& localAnchor0,const NxVec3& localAnchor1,const NxVec3& localAxis0,const NxVec3& localAxis1);
	
};

} // namespace physx {