/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#ifndef __PHYSICSUTILITIES_H
#define __PHYSICSUTILITIES_H

// include EMotion FX
#include <EMotionFX.h>

// include PhysX
#include <NxPhysics.h>

NxD6Joint* CreateD6Joint(NxScene* nxScene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis, const char* name);

// initialize PhysX
void CleanupPhysX(NxPhysicsSDK* physXSDK, NxScene* physXScene);
void InitPhysX(NxPhysicsSDK** physXSDK, NxScene** physXScene, bool* hardwareSimulationSupported, bool useHardwareSimulation);
float GetTimeStep();



#endif