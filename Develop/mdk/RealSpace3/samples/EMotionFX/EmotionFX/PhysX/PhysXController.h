/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#ifndef __PHYSXCONTROLLER_H
#define __PHYSXCONTROLLER_H

// include PhysX
#include <NxPhysics.h>

// this include file is part of the Ageia PhysX Tools
// the header is located in PHYSXMAINDIRECTORY/Tools/NxuStream2/
#include "Nxu_Helper.h"

// include EMotion FX
#include <EMotionFX.h>

using namespace MCore;
using namespace EMotionFX;


namespace EMotionFX
{
	class PhysXController : public WorldSpaceController, NXU_userNotify, public NXU_errorReport
	{
	public:
		// the unique ID of this controller
		enum { TYPE_ID = 0x06666001 };	// a unique 32 bit ID number that identifies your world space controller type/class

		enum RagDollType
		{
			RAGDOLL_FULL = 0,		// death, unconsciousness
			RAGDOLL_BLENDED = 1,	// 
			PHYSICS_SCENE = 2
		};

		// the constructor. The controller is set to DISABLED on default.
		PhysXController(ActorInstance* actorInstance, NXU::NxuPhysicsCollection* nxCollection, NxPhysicsSDK* nxSDK, NxScene* nxScene);

		// the destructor
		~PhysXController();

		/**
		 * Activate the controller.
		 * This will cause it to fully blend in during a given amount of time.
		 * @param fadeInTime The time, in seconds, which it should take to smoothly blend in the controller.
		 */
		void Activate(const float fadeInTime=0.0f);

		/**
		 * Deactivate the controller.
		 * This will fade out the controller during a given amount of time.
		 * When the controller has been deactivated, it will not have any visual impact anymore.
		 * Controllers that are inactive (having a weight of 0) will not be processed.
		 * @param fadeOutTime The time it should take to smoothly fade out the controller.
		 */
		void Deactivate(const float fadeOutTime=0.0f);

		/**
		 * This method is being triggered when the weight of this controller reaches zero and we are done fading out the controller completely.
		 * You can overload this method in case your controller needs to do something specific when this happens.
		 */
		void OnFullyDeactivated();

		/**
		 * This method is being triggered when the weight of this controller reaches a weight of 1.0f, so when we are done with fading in the controller completely.
		 * You can overload this method in case your controller needs to do something specific when this happens.
		 */
		void OnFullyActivated();

		// update the controller (main method)
		void Update(WorldPose* outPose, const float timePassedInSeconds);

		// clone the controller
		WorldSpaceController* Clone(ActorInstance* targetActorInstance);

		// the type identification functions
		uint32 GetType() const																		{ return TYPE_ID; }
		const char* GetTypeString() const															{ return "PhysXController"; }

		// object access
		MCORE_INLINE NxActor* GetActor(const uint32 index);
		MCORE_INLINE NxJoint* GetJoint(const uint32 index);

		MCORE_INLINE NxActor* GetActorForNode(const uint32 nodeIndex);
		MCORE_INLINE NxJoint* GetJointForNode(const uint32 nodeIndex);

		MCORE_INLINE uint32 GetNumActors() const;
		MCORE_INLINE uint32 GetNumJoints() const;

		MCORE_INLINE void MoveWorldPoseForNode(const uint32 nodeIndex, const Matrix& worldMatrix);

		static NXU::NxuPhysicsCollection* LoadPhysicsCollection(const MCore::String& filename, const NXU::NXU_FileType fileType = NXU::FT_XML);
		static void ReleasePhysicsCollection(NXU::NxuPhysicsCollection* collection);

		void SetRagDollType(const RagDollType type);

		/**
		 * Get the world space pose blend buffer.
		 * The previous world pose which will be used to calculate the velocity of the bones
		 * from the animation system which then will be applied to the physics actors.
		 * @result A pointer to the world space pose buffer.
		 */
		MCORE_INLINE WorldPose* GetPrevWorldPose();

	private:
		// overloaded functions, collection callbacks
		void NXU_errorMessage(bool severity,const char *str)										{ LOG(str); }
		void NXU_notifyScene(NxScene *scene,const char *userProperties)								{ mNXScene = scene; }
		void NXU_notifyFluid(NxFluid *fluid,const char *userProperties) 							{}
		void NXU_notifyJoint(NxJoint *joint,const char *userProperties)								{ mJoints.Add(joint); }
		void NXU_notifyActor(NxActor *actor,const char *userProperties) 							{ mActors.Add(actor); }
		void NXU_notifyCloth(NxCloth *c,const char *userProperteis)									{}
		NxScene* NXU_preNotifyScene(unsigned int sno,NxSceneDesc &scene,const char *userProperties)	{ return 0; }
		bool NXU_preNotifyJoint(NxJointDesc &joint,const char *userProperties)						{ return true; }
		bool NXU_preNotifyActor(NxActorDesc &actor,const char *userProperties)						{ return true; }
		bool NXU_preNotifyCloth(NxClothDesc &cloth,const char *userProperties)						{ return false; }
		bool NXU_preNotifyFluid(NxFluidDesc &fluid,const char *userProperties)						{ return false;	}

		bool IsValid();
		bool CreateRagDoll();
		void DestroyRagDoll();

		uint32 RecursiveFindActor(const uint32 nodeIndex);

		MCORE_INLINE void SetRigidBodyWorldPoseByNode(const uint32 nodeIndex);
		MCORE_INLINE void SetJointLocalPoseByNode(const uint32 nodeIndex);

		// apply the current emfx character pose to the physics
		void SetWorldPose();

		/**
		 * Extracts a world_pose from the current state of the physical
		 * actors. Uses the local pose to create reasonable
		 * transforms for bones in the model's skeleton that don't have
		 * physical analogues.
		 */
		void GetWorldPose(WorldPose* outPose);

		void UpdateJointMotorTargets();

		WorldPose*					mPrevWorldPose;

		RagDollType					mRagDollType;

		Array<uint32>				mActorToNodeMap;
		Array<uint32>				mNodeToActorMap;
		Array<uint32>				mJointToNodeMap;
		Array<uint32>				mNodeToJointMap;

		uint32						mRootActor;	// index of first node which has a PhysX actor

		Array<NxJoint*>				mJoints;
		Array<NxActor*>				mActors;

		NxPhysicsSDK*				mNXSDK;
		NxScene*					mNXScene;
		NXU::NxuPhysicsCollection*	mNXCollection;

		bool						mConstructRagDoll;
	};

} // namespace EMotionFX

// include inline code
#include "PhysXController.inl"


#endif
