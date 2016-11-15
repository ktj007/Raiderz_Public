#pragma once

#include "RPhysXActorControllerBuilder.h"

// RagDoll XML Scheme
#define RAG_ELEMENT_INFO	"Info"
#define RAG_ELEMENT_CONTEXT	"Context"
#define RAG_ATTR_CONTEXT	"name"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// RagDoll Factor
//
//////////////////////////////////////////////////////////////////////////

class RPhysXRagDollFactor
{
public:
	RPhysXRagDollFactor()
	{
		fDensity = 1.f;
		fElbowLimitRatio = 0.7f;
		fElbowSpring = 1.f;
		fElbowDamping = 5.f;

		fKneeLimitRatio = 0.7f;
		fKneeSpring = 1.f;
		fKneeDamping = 5.f;

		fHipLimitRatio = 0.25f;
		fHipSpring = 1000.f;
		fHipDamping = 5000.f;
		fHipSwingSpring = 100.f;
		fHipSwingDamping = 500.f;
		fHipTwistSpring = 100.f;
		fHipTwistDamping = 500.f;

		fShoulderLimitRatio = 0.25f;
		fShoulderSpring = 1000.f;
		fShoulderDamping = 5000.f;
		fShoulderSwingSpring = 100.f;
		fShoulderSwingDamping = 500.f;
		fShoulderTwistSpring = 100.f;
		fShoulderTwistDamping = 500.f;

		fVertLimitRatio = 0.1f;
		fVertSpring = 1000.f;
		fVertDamping = 5000.f;
		fVertSwingSpring = 100.f;
		fVertSwingDamping = 500.f;
		fVertTwistSpring = 100.f;
		fVertTwistDamping = 500.f;
	}

	float fDensity;

	float fElbowLimitRatio;
	float fElbowSpring;
	float fElbowDamping;

	float fKneeLimitRatio;
	float fKneeSpring;
	float fKneeDamping;

	float fHipLimitRatio;
	float fHipSpring;
	float fHipDamping;
	float fHipTwistSpring;
	float fHipTwistDamping;
	float fHipSwingSpring;
	float fHipSwingDamping;

	float fShoulderLimitRatio;
	float fShoulderSpring;
	float fShoulderDamping;
	float fShoulderTwistSpring;
	float fShoulderTwistDamping;
	float fShoulderSwingSpring;
	float fShoulderSwingDamping;

	float fVertLimitRatio;
	float fVertSpring;
	float fVertDamping;
	float fVertTwistSpring;
	float fVertTwistDamping;
	float fVertSwingSpring;
	float fVertSwingDamping;

	bool LoadRagDollFactorFromContext(MXmlElement* pRagDollElement, const char* pSzContextName );
	void LoadRagDollFactorFromContext(MXmlElement* pContextElement);
	void ChangeRagDollControllerFromFactor(RPhysXActorController* pController);
};


//////////////////////////////////////////////////////////////////////////
//
// Controller Builder for RagDoll
//
//////////////////////////////////////////////////////////////////////////

class RPhysXRagDollBuilder : public RPhysXActorControllerBuilder
{
public:

	RPhysXRagDollBuilder();
	virtual ~RPhysXRagDollBuilder(){}


	//////////////////////////////////////////////////////////////////////////
	// Builder Interface
	//
	virtual bool	Build( RPhysXActorController* pPhysXController );


	//////////////////////////////////////////////////////////////////////////
	// Ragdoll Bone
	//
	enum E_BONE_PART
	{
		// SHPERE SHAPE
		E_SPINE2 = 0,		// 허리2
		E_SPINE1,			// 허리1
		E_PELVIS,			// 골반

		// CAPSULE SHAPE
		E_LEFT_UPPERARM,	// 상박
		E_RIGHT_UPPERARM,

		E_LEFT_FOREARM,		// 하박
		E_RIGHT_FOREARM,

		E_LEFT_THIGH,		// 허벅지
		E_RIGHT_THIGH,

		E_LEFT_CALF,		// 종아리
		E_RIGHT_CALF,

		E_BONE_NUM
	};


	//////////////////////////////////////////////////////////////////////////
	// Ragdoll Joint
	//
	enum E_JOINT_PART
	{
		E_VERT_2 = 0,
		E_VERT_1,

		E_LEFT_SHOULDER,
		E_RIGHT_SHOULDER,

		E_LEFT_HIP,
		E_RIGHT_HIP,

		E_LEFT_ELBOW,
		E_RIGHT_ELBOW,

		E_LEFT_KNEE,
		E_RIGHT_KNEE,

		E_JOINT_NUM
	};


	//////////////////////////////////////////////////////////////////////////
	// Building Information
	//
	void			SetNxActorDensity(float _fDensity){ m_fDensity = _fDensity; }

	void			SetPelvisSize(float fPelvisSize){ m_fSphereSize[E_PELVIS] = fPelvisSize; }
	void			SetCapsuleRatio( float fRatio ){ m_fCapsuleRatio = fRatio; }

	void			SetElbowJointsLimit(float _fLimitRatio){ m_fElbowJointLimit = _fLimitRatio; }
	void			SetKneeJointsLimit(float _fLimitRatio){ m_fKneeJointLimit = _fLimitRatio; }

	void			SetHipJointsLimit(float _fLimitRatio){ m_fHipJointLimit = _fLimitRatio; }
	void			SetShoulderJointsLimit(float _fLimitRatio){ m_fShoulderJointLimit = _fLimitRatio; }


private:

	RBIPID			m_aryRefBip[E_BONE_NUM];
	float			m_fSphereSize[E_LEFT_UPPERARM];

	float			m_fCapsuleRatio;
	float			m_fDensity;

	float			m_fKneeJointLimit;
	float			m_fElbowJointLimit;
	float			m_fHipJointLimit;
	float			m_fShoulderJointLimit;

	float			GetCapsuleLengthFromChildBip( RActor* pActor, RBIPID bipId);
};

}

