#include "stdafx.h"
#include "MPhysXResourceFactory.h"
#include "NxCooking.h"

namespace physx {

NxActor* MPhysXResourceFactory::CreateGroundPlane(NxScene* scene)
{
	// Create a plane with default descriptor
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;

	planeDesc.normal = NxVec3(0,1,0);

	actorDesc.shapes.pushBack(&planeDesc);
	return scene->createActor(actorDesc);
}

NxActor* MPhysXResourceFactory::CreateSphere( NxScene* scene, const NxVec3& pos, const NxReal radius, const NxReal density )
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0,radius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	return scene->createActor(actorDesc);
}

NxActor* MPhysXResourceFactory::CreateCapsule( NxScene* scene, const NxVec3& actorpos, const NxVec3& capsuleLocalPos, const NxReal height, const NxReal radius, const NxReal density, void* pUserData /*= NULL*/ )
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = capsuleLocalPos;
	capsuleDesc.userData	= pUserData;
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = actorpos;
	return scene->createActor(actorDesc);
}

NxActor* MPhysXResourceFactory::CreateCapsule( NxScene* scene, const NxVec3& actorpos, const NxVec3& localTPos, const NxVec3& localBPos, const NxReal radius, const NxReal density, void* pUserData /*= NULL*/ )
{
	NxReal height		= localTPos.distance(localBPos);
	NxVec3 vLocalCenter = (localTPos - localBPos) / 2.0f;
	NxVec3 pos			= localBPos + vLocalCenter;

	return CreateCapsule(scene, actorpos, pos, height, radius, density, pUserData);
}

NxActor* MPhysXResourceFactory::CreateCapsule( NxScene* scene, const NxVec3& actorpos, const NxReal height, const NxReal radius, const NxReal density )
{
	return CreateCapsule(scene, actorpos, NxVec3(0,radius,0), height, radius, density);
}

NxActor* MPhysXResourceFactory::CreateBox( NxScene* scene, const NxVec3& pos, const NxVec3& boxDim, const NxReal density )
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	actorDesc.shapes.pushBack(&boxDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return scene->createActor(actorDesc);
}

NxCapsuleShapeDesc * MPhysXResourceFactory::CreateCapsuleShape( const NxVec3& localPos, const NxReal height, const NxReal radius, void* pUserData /*= NULL*/ )
{
	NxCapsuleShapeDesc * pCapsuleDesc = new NxCapsuleShapeDesc;
	pCapsuleDesc->height = height;
	pCapsuleDesc->radius = radius;
	pCapsuleDesc->localPose.t = localPos;
	pCapsuleDesc->userData	= pUserData;

	return pCapsuleDesc;
}

NxCapsuleShapeDesc * MPhysXResourceFactory::CreateCapsuleShape( const NxVec3& localTPos, const NxVec3& localBPos, const NxReal radius, void* pUserData /*= NULL*/ )
{
	NxReal height		= localTPos.distance(localBPos);
	NxVec3 vLocalCenter = (localTPos - localBPos) / 2.0f;
	NxVec3 pos			= localBPos + vLocalCenter;

	return CreateCapsuleShape(pos, height, radius, pUserData);
}

NxSphericalJoint* MPhysXResourceFactory::CreateSphericalJoint( NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis )
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	return (NxSphericalJoint*)scene->createJoint(sphericalDesc);
}

NxRevoluteJoint* MPhysXResourceFactory::CreateRevoluteJoint( NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis )
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	return (NxRevoluteJoint*)scene->createJoint(revDesc);
}

NxRevoluteJoint* MPhysXResourceFactory::CreateRevoluteJoint2( NxScene* scene, NxActor* a0, NxActor* a1,const NxVec3& localAnchor0,const NxVec3& localAnchor1,const NxVec3& localAxis0,const NxVec3& localAxis1 )
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	//revDesc.setGlobalAnchor(globalAnchor);
	//revDesc.setGlobalAxis(globalAxis);
	revDesc.localAnchor[0]=localAnchor0;
	revDesc.localAnchor[1]=localAnchor1;
	revDesc.localAxis[0]  =localAxis0;
	revDesc.localAxis[1]  =localAxis1;
	revDesc.projectionMode = NX_JPM_POINT_MINDIST;
	return (NxRevoluteJoint*)scene->createJoint(revDesc);
}

NxSphericalJoint* MPhysXResourceFactory::CreateBodySphericalJoint( NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis )
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
	sphericalDesc.twistLimit.low.value = -(NxReal)0.025*NxPi;
	sphericalDesc.twistLimit.low.hardness = 0.5;
	sphericalDesc.twistLimit.low.restitution = 0.5;
	sphericalDesc.twistLimit.high.value = (NxReal)0.025*NxPi;
	sphericalDesc.twistLimit.high.hardness = 0.5;
	sphericalDesc.twistLimit.high.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
	sphericalDesc.swingLimit.value = (NxReal)0.25*NxPi;
	sphericalDesc.swingLimit.hardness = 0.5;
	sphericalDesc.swingLimit.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	sphericalDesc.twistSpring.spring = 0.5;
	sphericalDesc.twistSpring.damper = 1;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	sphericalDesc.swingSpring.spring = 0.5;
	sphericalDesc.swingSpring.damper = 1;

	//	sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
	//	sphericalDesc.jointSpring.spring = 0.5;
	//	sphericalDesc.jointSpring.damper = 1;

	sphericalDesc.projectionDistance = (NxReal)0.15;
	sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;

	return (NxSphericalJoint*)scene->createJoint(sphericalDesc);
}

} // namespace physx {