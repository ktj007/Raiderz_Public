/*
 * EMotion FX 3 - Animation System
 * Copyright (c) 2001-2006 Mystic Game Development - http://www.mysticgd.com
 * All Rights Reserved.
 */

#include "PhysXController.h"

// TODO: HACK: remove again
#include "PhysXUtilities.h"


// use the Core namespace
using namespace MCore;


namespace EMotionFX
{

// constructor
PhysXController::PhysXController(ActorInstance* actorInstance, NXU::NxuPhysicsCollection* nxCollection, NxPhysicsSDK* nxSDK, NxScene* nxScene) : WorldSpaceController(actorInstance)
{
	// set the Ageia PhysX members
	mNXSDK				= nxSDK;
	mNXScene			= nxScene;
	mNXCollection		= nxCollection;
	mRootActor			= MCORE_INVALIDINDEX32;
	mConstructRagDoll	= false;
	mRagDollType		= RAGDOLL_FULL;

	// let the controller system know what nodes are being modified (set the node mask to true for each node that you modify)
	// also keep in mind that if you modify the transform of say the upper leg, the lower leg and feet also have to be updated
	// the RecursiveSetNodeMask method will mark the upper leg, and everything that is below that in the hierarchy (lower leg, feet, toes)
	Actor* actor = mActorInstance->GetActor();
	const uint32 numNodes = actor->GetNumNodes();
	for (uint32 i=0; i<numNodes; ++i)
		RecursiveSetNodeMask(i, true);	// the node with node index and all its child nodes down the hierarchy will be marked as being modified

	// construct the previous world pose which will be used to calculate the velocity of the bones
	// from the animation system and which then will be applied to the physics actors
	mPrevWorldPose = new WorldPose();

	// init the matrix data for the pose from the actor instance
	mPrevWorldPose->InitFromActorInstance( actorInstance );
}


// destructor
PhysXController::~PhysXController()
{
	// get rid of the physics actor
	DestroyRagDoll();
}


// activate the PhysX controller
void PhysXController::Activate(const float fadeInTime)
{
	// check if the controller is already active, if yes return directly
	if (WorldSpaceController::IsActive() || mIsChangingWeight)
		return;

	// needed, else the current pose might not be taken correctly
	mActorInstance->UpdateLocalMatrices();
	mActorInstance->UpdateWorldMatrices();

	// store the current pose as previous pose for the next frame where we actually construct the rag-doll
	mPrevWorldPose->InitFromActorInstance( mActorInstance );
	mConstructRagDoll = true;

	// activate the worldspace controller
	WorldSpaceController::Activate(fadeInTime);
}


// activate the PhysX controller
void PhysXController::Deactivate(const float fadeOutTime)
{
	// deactivate the worldspace controller
	WorldSpaceController::Deactivate(fadeOutTime);
}


void PhysXController::SetRagDollType(const RagDollType type)
{
	if (mRagDollType != type)
	{
		if (WorldSpaceController::IsActive() || mIsChangingWeight)
		{
			Deactivate(0.0f);
			DestroyRagDoll();

			mRagDollType = type;

			Activate(0.0f);
		}

		mRagDollType = type;
	}
}


// this method is being triggered when the weight of this controller reaches zero and we are done fading out
void PhysXController::OnFullyDeactivated()
{
	// check if the controller is inactive and destroy the physics actor in this case
	// the destruction needs to take place here as if the fade out time isn't 0.0f we still
	// need to update the physics until the physics is completely faded out
	DestroyRagDoll();
}


// this method is being triggered when the weight of this controller reaches a weight of 1.0f
void PhysXController::OnFullyActivated()
{
}


// clone the controller
WorldSpaceController* PhysXController::Clone(ActorInstance* targetActorInstance)
{
	// create the clone
	PhysXController* clone = new PhysXController( targetActorInstance, NULL, NULL, NULL );

	// TODO: not supported yet

	// copy the WorldSpaceController base class settings
	clone->CopyBaseClassWeightSettings( this );

	return clone;
}


// update the controller (main method)
// in this method you have to output the transformations for all nodes that you enabled
// the node mask for inside the constructor
void PhysXController::Update(WorldPose* outPose, const float timePassedInSeconds)
{
	// construct the rag doll one frame after the activation so that we can calculate the ragdoll velocity
	if (mConstructRagDoll)
	{
		// needed, else the current pose might not be taken correctly
		mActorInstance->UpdateLocalMatrices();
		mActorInstance->UpdateWorldMatrices();

		// create our physics actors
		if (!CreateRagDoll())
			LOG( "PhysXController::Activate(): Either we tried to construct a rag-doll while it was already there or something went wrong during the rag-doll creation." );

		// apply the current pose to the physics actors
		SetWorldPose();

		switch (mRagDollType)
		{
			case RAGDOLL_FULL:
			{
				// avoid division by zero
				if (timePassedInSeconds > Math::epsilon)
				{
					TransformData* transformData	= mActorInstance->GetTransformData();
					Matrix* actorWorldMatrices		= transformData->GetWorldMatrices();
					Matrix* prevWorldMatrices		= mPrevWorldPose->GetWorldMatrices();
					const uint32 numTransforms		= mPrevWorldPose->GetNumTransforms();

					// iterate through all nodes, find their physics actors and apply the velocities from the current animation
					for (uint32 i=0; i<numTransforms; ++i)
					{
						Node*			node		= mActorInstance->GetActor()->GetNode(i);
						const uint32	nodeIndex	= node->GetNodeIndex();
						NxActor*		nxActor		= GetActorForNode( nodeIndex );

						if (nxActor != NULL)
						{
							// calculate the world space velocity based on the position change since the previous
							// and the current pose (constant motion: velocity = distance / time)
							// keep in mind that the other bodies in the ragdoll may drag the motion, through
							// the joints, but since they are also being driven, it is a small contribution, and
							// will be corrected every frame.
							Vector3 posDelta = actorWorldMatrices[i].GetTranslation() - prevWorldMatrices[i].GetTranslation();
							Vector3 velocity = posDelta / timePassedInSeconds;

							//LOG("velocity: %f %f %f", velocity.x, velocity.y, velocity.z);
							nxActor->addForce( NxVec3(velocity.x, velocity.y, velocity.z), NX_VELOCITY_CHANGE );
						}
					}
				}

				break;
			}

			case RAGDOLL_BLENDED:
			{
				// convert all joints to 6DOF joints so that we can apply the motors
				// TODO: This is only a temp solution. the exporter should create the 6DOF (and only them) already
				/*const uint32 numJoints = mJoints.GetLength();
				for (uint32 i=0; i<numJoints; ++i)
				{
					NxJoint* nxBaseJoint = mJoints[i];

					NxActor* a1 = NULL;
					NxActor* a2 = NULL;
					nxBaseJoint->getActors(&a1, &a2);

					NxScene& jointScene	= nxBaseJoint->getScene();
					NxVec3 jointAnchor	= nxBaseJoint->getGlobalAnchor();
					NxVec3 jointAxis	= nxBaseJoint->getGlobalAxis();
					String jointName	= nxBaseJoint->getName();

					jointScene.releaseJoint( *nxBaseJoint );

					mJoints[i] =  CreateD6Joint( &jointScene, a1, a2, jointAnchor, jointAxis, jointName.AsChar() );
				}

				for (uint32 i=0; i<numJoints; ++i)
				{
					NxJoint* nxBaseJoint = mJoints[i];

					// make our joint a rotation motored joint
					if (nxBaseJoint->getType() == NX_JOINT_D6)
					{
						// get the current 6dof joint
						NxD6Joint* nxJoint = (NxD6Joint*)nxBaseJoint;

						// get the corresponding physics actors
						NxActor* nxActor0 = NULL;
						NxActor* nxActor1 = NULL;
						nxBaseJoint->getActors( &nxActor0, &nxActor1 );

						// get the current joint description
						NxD6JointDesc jointDesc;
						nxJoint->saveToDesc( jointDesc );

						jointDesc.twistMotion = NX_D6JOINT_MOTION_LIMITED;
						jointDesc.swing1Motion = NX_D6JOINT_MOTION_LIMITED;
						jointDesc.swing2Motion = NX_D6JOINT_MOTION_LIMITED;

						jointDesc.xMotion = NX_D6JOINT_MOTION_LOCKED;
						jointDesc.yMotion = NX_D6JOINT_MOTION_LOCKED;
						jointDesc.zMotion = NX_D6JOINT_MOTION_LOCKED;

						jointDesc.swing1Limit.value = 0.3*NxPi;
						jointDesc.swing1Limit.restitution = 0;
						jointDesc.swing1Limit.spring = 0;
						jointDesc.swing1Limit.damping = 0;

						jointDesc.swing2Limit.value = 0.3*NxPi;
						jointDesc.swing2Limit.restitution = 0;
						jointDesc.swing2Limit.spring = 0;
						jointDesc.swing2Limit.damping = 0;

						jointDesc.twistLimit.low.value = -0.05*NxPi;
						jointDesc.twistLimit.low.restitution = 0;
						jointDesc.twistLimit.low.spring = 0;
						jointDesc.twistLimit.low.damping = 0;

						jointDesc.twistLimit.high.value = 0.05*NxPi;
						jointDesc.twistLimit.high.restitution = 0;
						jointDesc.twistLimit.high.spring = 0;
						jointDesc.twistLimit.high.damping = 0;

						// setup our motor

						// swing drive - orientation target
						jointDesc.flags					= 0;
						jointDesc.swingDrive.driveType	= NX_D6JOINT_DRIVE_POSITION;
						jointDesc.swingDrive.spring		= 200;
						jointDesc.swingDrive.damping	= 0;
						NxQuat q1;
						q1.fromAngleAxis(90,NxVec3(0,1,0));
						jointDesc.driveOrientation		= q1;

						// swing drive - angular velocity target
						jointDesc.flags					= 0;
						jointDesc.swingDrive.driveType	= NX_D6JOINT_DRIVE_VELOCITY;
						jointDesc.swingDrive.forceLimit	= FLT_MAX;
						jointDesc.driveAngularVelocity	= NxVec3(0,5,0);

						// twist drive - orientation target
						jointDesc.flags					= 0;
						jointDesc.twistDrive.driveType	= NX_D6JOINT_DRIVE_POSITION;
						jointDesc.twistDrive.spring		= 200;
						jointDesc.twistDrive.damping	= 0;
						NxQuat q2;
						q2.fromAngleAxis(90,NxVec3(1,0,0));
						jointDesc.driveOrientation	= q2;

						// twist drive - angular velocity target
						jointDesc.flags					= 0;
						jointDesc.twistDrive.driveType	= NX_D6JOINT_DRIVE_VELOCITY;
						jointDesc.twistDrive.forceLimit	= FLT_MAX;
						jointDesc.driveAngularVelocity	= NxVec3(5,0,0);

						// set the modified joint description
						nxJoint->loadFromDesc( jointDesc );


						//physicsActor->raiseBodyFlag(NX_BF_KINEMATIC); // do for both actors of the joiont
						*/

					/*	void UpdateJointMotorTarget()
						{
							...
					else if (gJointType == 5)  // Rotational Motor 
							{
								NxD6JointDesc d6Desc;
								d6Joint->saveToDesc(d6Desc);
								...
									NxMat34 pose2 = capsule2->getGlobalPose();
								NxVec3 axis2;
								pose2.M.getRow(1,axis2);

								if (d6Desc.flags == NX_D6JOINT_SLERP_DRIVE)  // Slerp Angular Drive
								{
									...
								}
								else  // Swing-twist Angular Drive 
								{
									if (d6Desc.zDrive.driveType == NX_D6JOINT_DRIVE_POSITION)  // Orientation Target
									{
										if ((axis2.x > 0.5) || (axis2.x < -0.5))
										{
											if (axis2.x > 0.5)
											{
												NxQuat q;
												q.fromAngleAxis(-90, NxVec3(0,1,0));
												d6Desc.driveOrientation = q; 		       
											}
											else if (axis2.x < -0.5)
											{
												NxQuat q;
												q.fromAngleAxis(90, NxVec3(0,1,0));
												d6Desc.driveOrientation = q; 	       
											}
											d6Joint->loadFromDesc(d6Desc);
										}
									}
								}
							}
						}
					}
				}*/

				break;
			}
		}

		// avoid reconstructing the ragdoll in the next frames
		mConstructRagDoll = false;
	}


	switch (mRagDollType)
	{
		case RAGDOLL_FULL:
		{
			break;
		}

		case RAGDOLL_BLENDED:
		{
			//UpdateJointMotorTargets();
			break;
		}
	}

	//Timer physicsTimer;
	GetWorldPose(outPose);
	//LOG("PhysXController::Update(): %f ms", physicsTimer.GetTime() * 1000);

	switch (mRagDollType)
	{
		case RAGDOLL_FULL:
		{
			break;
		}

		case RAGDOLL_BLENDED:
		{
			Actor*			actor			= mActorInstance->GetActor();
			TransformData*	transformData	= mActorInstance->GetTransformData();
			const uint32	numNodes		= actor->GetNumNodes();

			NxMat34 globalPose;
			for (uint32 i=0; i<numNodes; ++i)
			{
				Node*			node		= actor->GetNode(i);
				const uint32	nodeIndex	= node->GetNodeIndex();
				NxActor*		nxActor		= GetActorForNode( nodeIndex );

				if (nxActor != NULL)
				{
					Vector3 nodePos = transformData->GetWorldPos( nodeIndex );
					NxVec3 deltaPos = NxVec3(nodePos.x, nodePos.y, nodePos.z) - nxActor->getGlobalPosition();
					NxVec3 newVelocity = deltaPos / timePassedInSeconds;
					LOG("nxActor='%s': delta: %f %f %f", node->GetName(), deltaPos.x, deltaPos.y, deltaPos.z);
					LOG("nxActor='%s': velocity: %f %f %f", node->GetName(), newVelocity.x, newVelocity.y, newVelocity.z);

					// check if the actor is a non-kinematic one
					if (!nxActor->readBodyFlag(NX_BF_KINEMATIC))
					{
						// TODO: HACK: why two paths?
						static bool m_applyVelocity = true;
						if (m_applyVelocity)
						{
							nxActor->setLinearVelocity(newVelocity);
							NxVec3 forceSize = newVelocity * 10;
						}
						else
						{
							//	NxVec3 acceleration = (newVelocity - nxActor->getLinearVelocity()) * m_stepMgr->oneOverDT();
							//	NxVec3 forceSize = nxActor->getMass() * acceleration;
							//	nxActor->addForce(forceSize);
						}
					}
				}
			}

			//UpdateJointMotorTargets();
			break;
		}
	}
}


void PhysXController::UpdateJointMotorTargets()
{
	if (mRagDollType == RAGDOLL_BLENDED)
	{
		Actor*			actor			= mActorInstance->GetActor();
		TransformData*	transformData	= mActorInstance->GetTransformData();

		// get the number of joints/nodes of our character and iterate through them
		const uint32 numJoints	= mJoints.GetLength();
		const uint32 numNodes	= actor->GetNumNodes();

		for (uint32 i=0; i<numNodes; ++i)
		{
			// get the base joint so that we can check if we are dealing with a 6 dof one
			NxJoint* nxBaseJoint = GetJointForNode( i );

			// skip nodes that don't have a joint assigned to them
			if (nxBaseJoint == NULL)
				continue;

			if (nxBaseJoint->getType() == NX_JOINT_D6)
			{
				// get the current 6dof joint
				NxD6Joint* nxJoint = (NxD6Joint*)nxBaseJoint;

				// get the corresponding node and its orientation
				Node* node = actor->GetNode( i );
				Quaternion nodeOrient = transformData->GetLocalRot( i );

				// get the current joint description
				NxD6JointDesc jointDesc;
				nxJoint->saveToDesc( jointDesc );

				jointDesc.flags					= 0;

				// swing drive - orientation target
				jointDesc.swingDrive.driveType	= NX_D6JOINT_DRIVE_POSITION;
				jointDesc.swingDrive.spring		= 200;
				jointDesc.swingDrive.damping	= 0;
				jointDesc.driveOrientation		= NxQuat( NxVec3( nodeOrient.x, nodeOrient.y, nodeOrient.z ), nodeOrient.w );

				// swing drive - angular velocity target
				jointDesc.swingDrive.driveType	= NX_D6JOINT_DRIVE_VELOCITY;
				jointDesc.swingDrive.forceLimit	= FLT_MAX;
				jointDesc.driveAngularVelocity	= NxVec3(0,5,0);

				// twist drive - orientation target
				jointDesc.twistDrive.driveType	= NX_D6JOINT_DRIVE_POSITION;
				jointDesc.twistDrive.spring		= 200;
				jointDesc.twistDrive.damping	= 0;
				jointDesc.driveOrientation		= NxQuat( NxVec3( nodeOrient.x, nodeOrient.y, nodeOrient.z ), nodeOrient.w );

				// twist drive - angular velocity target
				jointDesc.twistDrive.driveType	= NX_D6JOINT_DRIVE_VELOCITY;
				jointDesc.twistDrive.forceLimit	= FLT_MAX;
				jointDesc.driveAngularVelocity	= NxVec3(5,0,0);

				// set the modified joint description
				nxJoint->loadFromDesc( jointDesc );
			}
		}
	}
}


NXU::NxuPhysicsCollection* PhysXController::LoadPhysicsCollection(const MCore::String& filename, const NXU::NXU_FileType fileType)
{
	NXU::NxuPhysicsCollection* result = NXU::loadCollection( filename, fileType );
	return result;
}


void PhysXController::ReleasePhysicsCollection(NXU::NxuPhysicsCollection* collection)
{
	NXU::releaseCollection(collection);
}


bool PhysXController::IsValid()
{
	if (mNXSDK == NULL)
		return false;

	if (mNXScene == NULL)
		return false;

	if (mActorInstance == NULL)
		return false;

	// check if the actor to node map stores an index for each PhysX actor
	if (mActors.GetLength() != mActorToNodeMap.GetLength())
		return false;

	// check if the node to actor map stores an index for each node
	if (mNodeToActorMap.GetLength() != mActorInstance->GetNumNodes())
		return false;

	uint32 i;
	// check if all PhysX actors are valid
	const uint32 numActors = mActors.GetLength();
	for (i=0; i<numActors; ++i)
	{
		if (mActors[i] == NULL)
		return false;
	}

	// check if all PhysX joints are valid
	const uint32 numJoints = mJoints.GetLength();
	for (i=0; i<numJoints; ++i)
	{
		if (mJoints[i] == NULL)
			return false;
	}

	// validate all binding indices
	for (i=0; i<mActorToNodeMap.GetLength(); ++i)
	{
		if (mActorToNodeMap[i] == MCORE_INVALIDINDEX32)
			continue;

		if (mActorToNodeMap[i] >= mActorInstance->GetNumNodes())
			return false;
	}

	// validate all binding indices
	for (i=0; i<mNodeToActorMap.GetLength(); ++i)
	{
		if (mNodeToActorMap[i] == MCORE_INVALIDINDEX32)
			continue;

		if (mNodeToActorMap[i] >= mActors.GetLength())
			return false;
	}

	// validate all binding indices
	for (i=0; i<mJointToNodeMap.GetLength(); ++i)
	{
		if (mJointToNodeMap[i] == MCORE_INVALIDINDEX32)
			continue;

		if (mJointToNodeMap[i] >= mActorInstance->GetNumNodes())
			return false;
	}

	// validate all binding indices
	for (i=0; i<mNodeToJointMap.GetLength(); ++i)
	{
		if (mNodeToJointMap[i] == MCORE_INVALIDINDEX32)
			continue;

		if (mNodeToJointMap[i] >= mJoints.GetLength())
			return false;
	}

	return true;
}


// recursively find the first child node which has a physics body applied
uint32 PhysXController::RecursiveFindActor(const uint32 nodeIndex)
{
	uint32 result = MCORE_INVALIDINDEX32;

	if (GetActorForNode(nodeIndex))
		return nodeIndex;

	Actor* actor = mActorInstance->GetActor();
	Node* node = actor->GetNode(nodeIndex);
	const uint32 numChilds = node->GetNumChildNodes();
	for (uint32 i=0; i<numChilds; ++i)
		result = RecursiveFindActor( node->GetChildIndex(i) );

	return result;
}


// construct the ragdoll
bool PhysXController::CreateRagDoll()
{
	// we shouldn't already be bound
	if (IsValid())
		return false;

	//Timer initTimer;
	NXU::setErrorReport( this );
	NXU::instantiateCollection( mNXCollection, *mNXSDK, mNXScene, 0, this );
	//LOG("instantiateCollection: %f ms", initTimer.GetTime() * 1000.0f);

	assert( mNodeToActorMap.GetLength() == 0 && mActorToNodeMap.GetLength() == 0 );
	assert( mNodeToJointMap.GetLength() == 0 && mJointToNodeMap.GetLength() == 0 );

	// get the number of nodes, physics actors and joints
	Actor* actor			= mActorInstance->GetActor();
	const uint32 numNodes	= actor->GetNumNodes();
	const uint32 numActors	= mActors.GetLength();
	const uint32 numJoints	= mJoints.GetLength();

	// resize our arrays
	mNodeToActorMap.Resize( numNodes );
	mActorToNodeMap.Resize( numActors );
	mNodeToJointMap.Resize( numNodes );
	mJointToNodeMap.Resize( numJoints );

	// reset the array data
	uint32 i;
	for (i=0; i<numNodes; ++i)
		mNodeToActorMap[i] = MCORE_INVALIDINDEX32;

	// assign the PhysX actors to the nodes and the other way around
	for (i=0; i<numActors; ++i)
	{
		const char*	actorName	= mActors[i]->getName();
		Node*		node		= actor->FindNodeByName(actorName);

		if (node != NULL)
		{
			const uint32 nodeIndex		= node->GetNodeIndex();
			mNodeToActorMap[nodeIndex]	= i;
			mActorToNodeMap[i]			= nodeIndex;
		}
		else
		{
			LOG("ERROR: Could not assign node to physics actor '%s'.", actorName);
			mActorToNodeMap[i] = MCORE_INVALIDINDEX32;
		}
	}


	// reset the array data
	for (i=0; i<numNodes; ++i)
		mNodeToJointMap[i] = MCORE_INVALIDINDEX32;

	// assign the PhysX joints to the nodes and the other way around
	for (i=0; i<numJoints; ++i)
	{
		const char*	jointName	= mJoints[i]->getName();
		Node*		node		= actor->FindNodeByName(jointName);

		if (node != NULL)
		{
			const uint32 nodeIndex		= node->GetNodeIndex();
			mNodeToJointMap[nodeIndex]	= i;
			mJointToNodeMap[i]			= nodeIndex;
		}
		else
		{
			LOG("ERROR: Could not assign node to physics joint '%s'.", jointName);
			mJointToNodeMap[i] = MCORE_INVALIDINDEX32;
		}
	}

	// assign a PhysX actor to the actor instance and the other way around
	// TODO: TEMP: probably not the best way to do it
	const uint32 numRootNodes = actor->GetNumRootNodes();
	for (i=0; i<numRootNodes; ++i)
	{
		mRootActor = RecursiveFindActor( actor->GetRootNodeIndex(i) );

		if (mRootActor != MCORE_INVALIDINDEX32)
			break;
	}
	if (mRootActor == MCORE_INVALIDINDEX32)
		LOG("ERROR: Could not assign root physics actor.");

	return IsValid();
}


// release the rag-doll
void PhysXController::DestroyRagDoll()
{
	uint32 i;

	// release all PhysX joints
	const uint32 numJoints = mJoints.GetLength();
	for (i=0; i<numJoints; ++i)
	{
		NxScene& scene = mJoints[i]->getScene();
		scene.releaseJoint(*mJoints[i]);
	}

	// release all PhysX actors
	const uint32 numActors = mActors.GetLength();
	for (i=0; i<numActors; ++i)
	{
		NxScene& scene = mActors[i]->getScene();
		scene.releaseActor(*mActors[i]);
	}

	// remove all items from the arrays
	// do not release the memory yet in case the user reactivates the rag-doll
	mActors.Clear( false );
	mJoints.Clear( false );
	mNodeToActorMap.Clear( false );
	mActorToNodeMap.Clear( false );
	mNodeToJointMap.Clear( false );
	mJointToNodeMap.Clear( false );
	mRootActor = MCORE_INVALIDINDEX32;
}


// apply the current emfx character pose to the physics
void PhysXController::SetWorldPose()
{
	Actor* actor = mActorInstance->GetActor();
	const uint32 numNodes = actor->GetNumNodes();

	for (uint32 i=0; i<numNodes; ++i)
	{
		Node*			node		= actor->GetNode( i );
		const uint32	nodeIndex	= node->GetNodeIndex();

		SetRigidBodyWorldPoseByNode( nodeIndex );
	}
}


// get the physics pose and apply it on the emfx character
void PhysXController::GetWorldPose(WorldPose* outPose)
{
	uint32 i;
	Actor*			actor			= mActorInstance->GetActor();
	Matrix*			worldMatrices	= outPose->GetWorldMatrices();
	TransformData*	transformData	= mActorInstance->GetTransformData();
	const uint32	numActors		= mActors.GetLength();

	// get the transformation informations from the ragdoll's root node
	NxActor*	rootActor = NULL;
	Matrix		rootActorMatrix;
	Vector3		rootActorPos;
	Quaternion	rootActorRot;

	// only do it for characters, we don't want to do this for physics scenes
	if (mRootActor != MCORE_INVALIDINDEX32 && mRagDollType != PHYSICS_SCENE)
	{
		rootActor = GetActorForNode( mRootActor );
		if (rootActor)
		{
			NxMat34 globalPose = rootActor->getGlobalPose();
			globalPose.getColumnMajor44( rootActorMatrix.m16 );

			// decompose the position and rotation from the root node
			rootActorMatrix.Decompose( &rootActorPos, &rootActorRot);
		}
	}

	// step 1: extract node matrices for bones that have a direct correspondence in the physics model
	const uint32 numNodes = actor->GetNumNodes();
	NxMat34 globalPose;
	for (i=0; i<numNodes; ++i)
	{
		Node*			node		= actor->GetNode(i);
		const uint32	nodeIndex	= node->GetNodeIndex();
		NxActor*		nxActor		= GetActorForNode( nodeIndex );

		if (nxActor != NULL)
		{
			// if we are dealing with the ragdoll's root node, adjust the actor instance instead
			// of the corresponding emfx node
		//	if (nxActor == rootActor)
		//	{
		//		mActorInstance->SetLocalPos( rootPos );
		//		mActorInstance->SetLocalRot( rootRot );
		//	}
		//	else
			{
				globalPose = nxActor->getGlobalPose();
				globalPose.getColumnMajor44( worldMatrices[nodeIndex].m16 );
			}
		}
	}

	// step 2: using local pose we will in the world matrices for bones that don't have a representation in the physics model
	Matrix localMatrix;
	for (i=0; i<numNodes; ++i)
	{
		Node*			node		= actor->GetNode(i);
		const uint32	nodeIndex	= node->GetNodeIndex();
		NxActor*		nxActor		= GetActorForNode( nodeIndex );

		if (nxActor == NULL && node->GetParentNode() != NULL)
		{
			mActorInstance->CalcLocalTM(nodeIndex,
										Quaternion(),
										transformData->GetOrgRot(nodeIndex),
										transformData->GetOrgPos(nodeIndex),
										transformData->GetOrgScale(nodeIndex),
										&localMatrix);

			localMatrix.MultMatrix4x3( worldMatrices[node->GetParentIndex()] );
			worldMatrices[nodeIndex] = localMatrix;
		}
		else
		{
			if (node->GetParentNode() == NULL && rootActor != NULL)
				worldMatrices[nodeIndex] = rootActorMatrix;
		}
	}

	// update the actor instance's position
	// TODO: also do with rotation?
	//mActorInstance->SetLocalPos( rootActorMatrix.GetTranslation() );
}

} // namespace EMotionFX
