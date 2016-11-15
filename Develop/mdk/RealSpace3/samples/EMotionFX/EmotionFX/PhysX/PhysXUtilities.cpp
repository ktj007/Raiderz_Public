/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#ifndef __PHYSICSUTILITIES_H
#define __PHYSICSUTILITIES_H

#include "PhysXUtilities.h"

// include PhysX
#include <NxPhysics.h>

// include EMotion FX
#include <EMotionFX.h>
/*
// Enigma
#define ENIGMA_CLIENTSIDE		// Set to import headers
#include "../../../../Enigma/Source/Precompiled.h"
#include "../../../../Enigma/Source/Enigma.h"
*/
using namespace EMotionFX;

// Limit globals
NxReal gLinearLimit = 1.0f;
NxReal gSwing1Limit = NxPiF32 / 180.0f * 30.0f;
NxReal gSwing2Limit = NxPiF32 / 180.0f * 70.0f;
NxReal gTwistLowLimit = NxPiF32 / 180.0f * -90.0f;
NxReal gTwistHighLimit = NxPiF32 / 180.0f * 45.0f;
typedef NxVec3 Point;
typedef struct _Triangle { NxU32 p0; NxU32 p1; NxU32 p2; } Triangle;


NxD6Joint* CreateD6Joint(NxScene* nxScene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis, const char* name)
{
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;
	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);
	d6Desc.name = name;

	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.linearLimit.value = gLinearLimit;
	d6Desc.swing1Limit.value = gSwing1Limit;
	d6Desc.swing2Limit.value = gSwing2Limit;
	d6Desc.twistLimit.low.value =  gTwistLowLimit;
	d6Desc.twistLimit.high.value = gTwistHighLimit;

	// drive test
	/*
	d6Desc.drivePosition.x = 0.5f;
	d6Desc.xDrive.spring = 1000.0f;
	d6Desc.xDrive.damping = 1.0f;
	d6Desc.xDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_POSITION);

	d6Desc.driveLinearVelocity.z = 0.5f;
	d6Desc.zDrive.forceLimit = 1000.0f;
	d6Desc.zDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_VELOCITY);

	d6Desc.swingDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_POSITION);
	d6Desc.swingDrive.spring = 100.0f;
	d6Desc.swingDrive.damping = 1.0f;
	d6Desc.driveOrientation.fromAngleAxis(-40.0f, NxVec3(0,1,1));

	d6Desc.swingDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_VELOCITY);
	d6Desc.swingDrive.forceLimit = 1000.0f;
	d6Desc.driveAngularVelocity.y = 10.0f;
	*/	

	//	d6Desc.projectionMode = NX_JPM_NONE;
	d6Desc.projectionMode = NX_JPM_POINT_MINDIST;

	//	d6Desc.jointFlags |= NX_JF_COLLISION_ENABLED;

	NxJoint* d6Joint = nxScene->createJoint(d6Desc);

	return (NxD6Joint*)d6Joint->is(NX_JOINT_D6);
}



float GetTimeStep()
{
	return 1.0f / 60.0f;
}



// initialize PhysX
void InitPhysX(NxPhysicsSDK** physXSDK, NxScene** physXScene, bool* hardwareSimulationSupported, bool useHardwareSimulation)
{
	// initialize Physics SDK
	(*physXSDK) = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, NULL );
	if ((*physXSDK) == NULL)
		return;

	NxHWVersion hwCheck = (*physXSDK)->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) 
	{
		MCore::LogWarning("Unable to find a PhysX card, or PhysX card used by other application.");
		*hardwareSimulationSupported = false;
	}
	else
	{
		MCore::LogInfo("PhysX capable hardware found.");
		*hardwareSimulationSupported = true;
	}

	(*physXSDK)->setParameter( NX_VISUALIZATION_SCALE, 1.0f );
	(*physXSDK)->setParameter( NX_VISUALIZE_COLLISION_SHAPES, 1 );
	//(*physXSDK)->setParameter( NX_VISUALIZE_ACTOR_AXES, 1 );
	//(*physXSDK)->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);
	//(*physXSDK)->setParameter(NX_VISUALIZE_BODY_LIN_VELOCITY, 1);
	//(*physXSDK)->setParameter(NX_VISUALIZE_BODY_JOINT_GROUPS, 1);
	//(*physXSDK)->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);

	// create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3( 0.0f, -9.81f * 3.0f, 0.0f );

	if (useHardwareSimulation == true && *hardwareSimulationSupported == true)
	{
		MCore::LogInfo("Physics will be simulated in hardware.");
		sceneDesc.simType = NX_SIMULATION_HW;
	}
	else
	{
		if (useHardwareSimulation == true)
			MCore::LogInfo("Cannot simulate physics in hardware as no PhysX capable hardware has been found.");
		MCore::LogInfo("Physics will be simulated in software.");
		sceneDesc.simType = NX_SIMULATION_SW;
	}

	(*physXScene) = (*physXSDK)->createScene( sceneDesc );
	if (physXScene == NULL)
		return;

	// simulation timing
	(*physXScene)->setTiming(GetTimeStep(), 2, NX_TIMESTEP_FIXED);


	// set default material
	NxMaterial* defaultMaterial = (*physXScene)->getMaterialFromIndex( 0 );
	defaultMaterial->setRestitution( 0.7f );
	defaultMaterial->setStaticFriction( 0.5f );
	defaultMaterial->setDynamicFriction( 0.5f );

	// create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	//actorDesc.globalPose.t  = NxVec3(0.0f, -10.0f, 0.0f);
	actorDesc.shapes.pushBack(&planeDesc);
	(*physXScene)->createActor(actorDesc);
}


// destroy PhysX
void CleanupPhysX(NxPhysicsSDK* physXSDK, NxScene* physXScene)
{
	if (physXSDK != NULL)
	{
		if (physXScene != NULL)
			physXSDK->releaseScene( *physXScene );

		physXSDK->release();
	}
}


#endif