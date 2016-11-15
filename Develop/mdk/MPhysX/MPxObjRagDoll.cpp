#include "stdafx.h"

#include "MPxObjRagDoll.h"
#include "MPhysXResourceFactory.h"

#pragma warning( disable : 4305 )

namespace physx {

class SampleRagdoll : public MPxObjRagDoll::RagDollEntity
{
public:
	SampleRagdoll(const NxVec3& pos, NxScene* pNxScene)
	{
		NxQuat qRotLeft, qRotRight, qRotAround;
		qRotLeft.fromAngleAxis(90, NxVec3(0,0,1));
		qRotRight.fromAngleAxis(-90, NxVec3(0,0,1));
		qRotAround.fromAngleAxis(180, NxVec3(0,0,1));

		NxMat33 mRotLeft, mRotRight, mRotAround;
		mRotLeft.fromQuat(qRotLeft);
		mRotRight.fromQuat(qRotRight);
		mRotAround.fromQuat(qRotAround);

		// Create body parts
		head = MPhysXResourceFactory::CreateSphere(pNxScene, NxVec3(0,8.8,0), 0.5, 10);
		torso = MPhysXResourceFactory::CreateSphere(pNxScene, NxVec3(0,7,0), 0.95, 10);
		pelvis = MPhysXResourceFactory::CreateSphere(pNxScene, NxVec3(0,5.8,0), 0.7, 10);

		leftUpperArm = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(0.5,8.5,0), 1, 0.4, 10);
		leftUpperArm->setGlobalOrientationQuat(qRotRight);
		//leftForeArm = MPhysXNxObjectFactory::CreateCapsule(pNxScene, NxVec3(2,8.5,0), 1, 0.3, 10);
		leftForeArm = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(2.2,8.5,0), 1, 0.3, 10);
		leftForeArm->setGlobalOrientationQuat(qRotRight);
		leftHand = MPhysXResourceFactory::CreateBox(pNxScene, NxVec3(3.5,8.5,0), NxVec3(0.3,0.3,0.1), 10);
		leftHand->setGlobalOrientationQuat(qRotRight);

		rightUpperArm = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(-0.5,8.5,0), 1, 0.4, 10);
		rightUpperArm->setGlobalOrientationQuat(qRotLeft);
		//rightForeArm = MPhysXNxObjectFactory::CreateCapsule(pNxScene, NxVec3(-2,8.5,0), 1, 0.3, 10);
		rightForeArm = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(-2.2,8.5,0), 1, 0.3, 10);
		rightForeArm->setGlobalOrientationQuat(qRotLeft);
		rightHand = MPhysXResourceFactory::CreateBox(pNxScene, NxVec3(-3.5,8.5,0), NxVec3(0.3,0.3,0.1), 10);
		rightHand->setGlobalOrientationQuat(qRotLeft);

		leftThigh = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(0.6,6,0), 1.5, 0.5, 10);
		leftThigh->setGlobalOrientationQuat(qRotAround);
		leftCalf = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(0.6,3.5,0), 1.5, 0.35, 10);
		leftCalf->setGlobalOrientationQuat(qRotAround);
		leftFoot = MPhysXResourceFactory::CreateBox(pNxScene, NxVec3(0.6,1.5,0.2), NxVec3(0.4,0.2,0.75), 10);
		leftFoot->setGlobalOrientationQuat(qRotAround);

		rightThigh = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(-0.6,6,0), 1.5, 0.5, 10);
		rightThigh->setGlobalOrientationQuat(qRotAround);
		rightCalf = MPhysXResourceFactory::CreateCapsule(pNxScene, NxVec3(-0.6,3.5,0), 1.5, 0.35, 10);
		rightCalf->setGlobalOrientationQuat(qRotAround);
		rightFoot = MPhysXResourceFactory::CreateBox(pNxScene, NxVec3(-0.6,1.5,0.2), NxVec3(0.4,0.2,0.75), 10);
		rightFoot->setGlobalOrientationQuat(qRotAround);

		// Joint body parts together
		neck = MPhysXResourceFactory::CreateBodySphericalJoint(pNxScene, head,torso,NxVec3(0,8.8,0),NxVec3(0,1,0));
		leftShoulder = MPhysXResourceFactory::CreateBodySphericalJoint(pNxScene, leftUpperArm,torso,NxVec3(0.5,8.5,0),NxVec3(1,0,0));
		rightShoulder = MPhysXResourceFactory::CreateBodySphericalJoint(pNxScene, rightUpperArm,torso,NxVec3(-0.5,8.5,0),NxVec3(-1,0,0));
		spine = MPhysXResourceFactory::CreateBodySphericalJoint(pNxScene, torso,pelvis,NxVec3(0,7,0),NxVec3(0,-1,0));
		leftHip = MPhysXResourceFactory::CreateBodySphericalJoint(pNxScene, leftThigh,pelvis,NxVec3(0.6,6,0),NxVec3(0,-1,0));
		rightHip = MPhysXResourceFactory::CreateBodySphericalJoint(pNxScene, rightThigh,pelvis,NxVec3(-0.6,6,0),NxVec3(0,-1,0));

		leftElbow = MPhysXResourceFactory::CreateRevoluteJoint(pNxScene, leftForeArm,leftUpperArm,NxVec3(2,8.5,0),NxVec3(0,0,-1));
		rightElbow = MPhysXResourceFactory::CreateRevoluteJoint(pNxScene, rightForeArm,rightUpperArm,NxVec3(-2,8.5,0),NxVec3(0,0,-1));

		leftWrist = MPhysXResourceFactory::CreateRevoluteJoint2(pNxScene, leftHand,leftForeArm,NxVec3(0,-0.15,0),NxVec3(0,1.3,0),NxVec3(0,1,0),NxVec3(0,1,0));
		rightWrist = MPhysXResourceFactory::CreateRevoluteJoint2(pNxScene, rightHand,rightForeArm,NxVec3(0,-0.15,0),NxVec3(0,1.3,0),NxVec3(0,1,0),NxVec3(0,1,0));

		leftKnee = MPhysXResourceFactory::CreateRevoluteJoint(pNxScene, leftCalf,leftThigh,NxVec3(0.6,3.5,0),NxVec3(1,0,0));
		rightKnee = MPhysXResourceFactory::CreateRevoluteJoint(pNxScene, rightCalf,rightThigh,NxVec3(-0.6,3.5,0),NxVec3(-1,0,0));

		leftAnkle = MPhysXResourceFactory::CreateRevoluteJoint(pNxScene, leftFoot,leftCalf,NxVec3(0.6,1.3,0),NxVec3(1,0,0));
		rightAnkle = MPhysXResourceFactory::CreateRevoluteJoint(pNxScene, rightFoot,rightCalf,NxVec3(-0.6,1.3,0),NxVec3(-1,0,0));

		TranslationPosition(pos);

#define _F( actor ) { actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY); actor->raiseBodyFlag(NX_BF_FROZEN); }

		_F(head);
		_F(torso);
		_F(pelvis);

		_F(leftUpperArm);
		_F(leftForeArm);
		_F(leftHand);

		_F(rightUpperArm);
		_F(rightForeArm);
		_F(rightHand);

		_F(leftThigh);
		_F(leftCalf);
		_F(leftFoot);

		_F(rightThigh);
		_F(rightCalf);
		_F(rightFoot);
	}

	virtual void Destroy( NxScene* pScene )
	{
		pScene->releaseActor(*head);
		pScene->releaseActor(*torso);
		pScene->releaseActor(*pelvis);
		pScene->releaseActor(*leftUpperArm);
		pScene->releaseActor(*rightUpperArm);
		pScene->releaseActor(*leftForeArm);
		pScene->releaseActor(*rightForeArm);
		pScene->releaseActor(*leftHand);
		pScene->releaseActor(*rightHand);
		pScene->releaseActor(*leftThigh);
		pScene->releaseActor(*rightThigh);
		pScene->releaseActor(*leftCalf);
		pScene->releaseActor(*rightCalf);
		pScene->releaseActor(*leftFoot);
		pScene->releaseActor(*rightFoot);

		pScene->releaseJoint(*neck);
		pScene->releaseJoint(*leftShoulder);
		pScene->releaseJoint(*rightShoulder);
		pScene->releaseJoint(*spine);
		pScene->releaseJoint(*leftHip);
		pScene->releaseJoint(*rightHip);

		pScene->releaseJoint(*leftElbow);
		pScene->releaseJoint(*rightElbow);
		pScene->releaseJoint(*leftWrist);
		pScene->releaseJoint(*rightWrist);
		pScene->releaseJoint(*leftKnee);
		pScene->releaseJoint(*rightKnee);
		pScene->releaseJoint(*leftAnkle);
		pScene->releaseJoint(*rightAnkle);
	}

	virtual const MBox& GetAreaBox()
	{
		NxVec3 vNxPos(pelvis->getGlobalPosition());
		MVector3 vPos(vNxPos.x, vNxPos.y, vNxPos.z);
		m_AABB.vmax = vPos + MVector3(20, 20, 20);
		m_AABB.vmin = vPos - MVector3(20, 20, 20);

		return m_AABB;
	}

	virtual void OnSimulated(){}

	void TranslationPosition(const NxVec3& trans)
	{
#define _TRANS( _obj, _trans) { _obj->setGlobalPosition( _obj->getGlobalPosition() + _trans ); }

		_TRANS(head ,trans );
		_TRANS(torso ,trans );
		_TRANS(pelvis ,trans );
		_TRANS(leftUpperArm, trans );
		_TRANS(rightUpperArm ,trans );
		_TRANS(leftForeArm ,trans );
		_TRANS(rightForeArm ,trans );
		_TRANS(leftHand ,trans );
		_TRANS(rightHand ,trans );
		_TRANS(leftThigh ,trans );
		_TRANS(rightThigh ,trans );
		_TRANS(leftCalf ,trans );
		_TRANS(rightCalf ,trans );
		_TRANS(leftFoot ,trans );
		_TRANS(rightFoot ,trans );
	}

	NxActor* head;
	NxActor* torso;
	NxActor* pelvis;
	NxActor* leftUpperArm;
	NxActor* rightUpperArm;
	NxActor* leftForeArm;
	NxActor* rightForeArm;
	NxActor* leftHand;
	NxActor* rightHand;
	NxActor* leftThigh;
	NxActor* rightThigh;
	NxActor* leftCalf;
	NxActor* rightCalf;
	NxActor* leftFoot;
	NxActor* rightFoot;

	NxSphericalJoint* neck;
	NxSphericalJoint* leftShoulder;
	NxSphericalJoint* rightShoulder;
	NxSphericalJoint* spine;
	NxSphericalJoint* leftHip;
	NxSphericalJoint* rightHip;

	NxRevoluteJoint* leftElbow;
	NxRevoluteJoint* rightElbow;
	NxRevoluteJoint* leftWrist;
	NxRevoluteJoint* rightWrist;
	NxRevoluteJoint* leftKnee;
	NxRevoluteJoint* rightKnee;
	NxRevoluteJoint* leftAnkle;
	NxRevoluteJoint* rightAnkle;

	MBox m_AABB;
};

MPxObjRagDoll::MPxObjRagDoll()
: m_pRagDoll(NULL)
{

}

MPxObjRagDoll::~MPxObjRagDoll()
{
	if (m_pRagDoll)
	{
		m_pRagDoll->Destroy(GetNxScene());
		delete m_pRagDoll;
	}
}

void MPxObjRagDoll::CreateSampleRagDoll( const MVector3& _rPos )
{
	m_pRagDoll = new SampleRagdoll( NxVec3(_rPos.x, _rPos.y, _rPos.z), GetNxScene() );
}

const MBox* MPxObjRagDoll::GetAreaBox()
{
	MBox* pAABB = NULL;
	if (m_pRagDoll && m_pRagDoll->IsActive())
	{
		m_AABB = m_pRagDoll->GetAreaBox();
		pAABB = &m_AABB;
	}

	return pAABB;
}

int MPxObjRagDoll::OnRefreshCollisionGroup()
{
	int nGroup = MPhysXObject::OnRefreshCollisionGroup();
	if(nGroup > 0 && m_pRagDoll)
		m_pRagDoll->OnRefreshCollisionGroup(nGroup);

	return nGroup;
}

} // namespace physx
