/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */


namespace EMotionFX
{

MCORE_INLINE void PhysXController::MoveWorldPoseForNode(const uint32 nodeIndex, const Matrix& worldMatrix)
{
	assert(IsValid());

	NxActor* actor = GetActorForNode( nodeIndex );
	assert(actor->readBodyFlag(NX_BF_KINEMATIC));

	if (actor)
	{
		NxMat34 globalPose;
		globalPose.setColumnMajor44( worldMatrix.m16 );
		actor->moveGlobalPose( globalPose );
	}
}


MCORE_INLINE void PhysXController::SetRigidBodyWorldPoseByNode(const uint32 nodeIndex)
{
	NxActor* nxActor = GetActorForNode( nodeIndex );

	if (nxActor != NULL)
	{
		// only do this for non-static actors
		if (nxActor->isDynamic())
		{
			NxMat34 globalPose;
			globalPose.setColumnMajor44( mActorInstance->GetTransformData()->GetWorldTM(nodeIndex).m16 );

			nxActor->setGlobalPose( globalPose );
		}
	}
}


MCORE_INLINE void PhysXController::SetJointLocalPoseByNode(const uint32 nodeIndex)
{
	NxJoint* nxJoint = GetJointForNode( nodeIndex );

	if (nxJoint != NULL)
	{
		//nxJoint->setGlobalAnchor( mActorInstance->GetTransformData()->GetLocalPos( nodeIndex ) );
		//nxJoint->setGlobalAxis();
		//TODO: not working yet
	}
}


MCORE_INLINE NxActor* PhysXController::GetActorForNode(const uint32 nodeIndex)
{
	//assert( IsValid() );

	if (mNodeToActorMap[nodeIndex] != MCORE_INVALIDINDEX32)
		return mActors[ mNodeToActorMap[nodeIndex] ];

	return NULL;
}


MCORE_INLINE NxJoint* PhysXController::GetJointForNode(const uint32 nodeIndex)
{
	//assert( IsValid() );

	if (mNodeToJointMap[nodeIndex] != MCORE_INVALIDINDEX32)
		return mJoints[ mNodeToJointMap[nodeIndex] ];

	return NULL;
}


MCORE_INLINE NxActor* PhysXController::GetActor(const uint32 index)
{
	return mActors[index];
}


MCORE_INLINE NxJoint* PhysXController::GetJoint(const uint32 index)
{
	return mJoints[index];
}


MCORE_INLINE uint32 PhysXController::GetNumActors() const
{
	return mActors.GetLength();
}


MCORE_INLINE uint32 PhysXController::GetNumJoints() const
{
	return mJoints.GetLength();
}


MCORE_INLINE WorldPose* PhysXController::GetPrevWorldPose()
{
	return mPrevWorldPose;
}

} // namespace EMotionFX
