#include "StdAfx.h"

#pragma push_macro("min")
#pragma push_macro("max")
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "NxPhysics.h"

#include "NxController.h"
#include "NxControllerManager.h"

#pragma pop_macro("min")
#pragma pop_macro("max")

#include "RActor.h"
#include "RPhysXRagDollBuilder.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RDebugPassRenderer.h"
#include "RMeshNode.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// RagDoll Factor
//
//////////////////////////////////////////////////////////////////////////

bool RPhysXRagDollFactor::LoadRagDollFactorFromContext( MXmlElement* pRagDollElement, const char* pSzContextName )
{
	_ASSERT(pRagDollElement != NULL);

	bool bRet = false;
	std::string strWantedName(pSzContextName);
	MXmlElement* pContextElement = pRagDollElement->FirstChildElement(RAG_ELEMENT_CONTEXT);
	while ( pContextElement )
	{
		if ( strWantedName == pContextElement->Attribute(RAG_ATTR_CONTEXT) )
		{
			LoadRagDollFactorFromContext(pContextElement);
			bRet = true;
			break;
		}
		pContextElement = pContextElement->NextSiblingElement(RAG_ELEMENT_CONTEXT);
	}

	return bRet;
}

void RPhysXRagDollFactor::LoadRagDollFactorFromContext( MXmlElement* pContextElement )
{
	_Contents(&fDensity, pContextElement->FirstChildElement("Density"));
	_Contents(&fElbowLimitRatio, pContextElement->FirstChildElement("ElbowLimitRatio"));
	_Contents(&fElbowSpring, pContextElement->FirstChildElement("ElbowSpring"));
	_Contents(&fElbowDamping, pContextElement->FirstChildElement("ElbowDamping"));
	_Contents(&fKneeLimitRatio, pContextElement->FirstChildElement("KneeLimitRatio"));
	_Contents(&fKneeSpring, pContextElement->FirstChildElement("KneeSpring"));
	_Contents(&fKneeDamping, pContextElement->FirstChildElement("KneeDamping"));
	_Contents(&fHipLimitRatio, pContextElement->FirstChildElement("HipLimitRatio"));
	_Contents(&fHipSpring, pContextElement->FirstChildElement("HipSpring"));
	_Contents(&fHipDamping, pContextElement->FirstChildElement("HipDamping"));
	_Contents(&fHipTwistSpring, pContextElement->FirstChildElement("HipTwistSpring"));
	_Contents(&fHipTwistDamping, pContextElement->FirstChildElement("HipTwistDamping"));
	_Contents(&fHipSwingSpring, pContextElement->FirstChildElement("HipSwingSpring"));
	_Contents(&fHipSwingDamping, pContextElement->FirstChildElement("HipSwingDamping"));
	_Contents(&fShoulderLimitRatio, pContextElement->FirstChildElement("ShoulderLimitRatio"));
	_Contents(&fShoulderSpring, pContextElement->FirstChildElement("ShoulderSpring"));
	_Contents(&fShoulderDamping, pContextElement->FirstChildElement("ShoulderDamping"));
	_Contents(&fShoulderTwistSpring, pContextElement->FirstChildElement("ShoulderTwistSpring"));
	_Contents(&fShoulderTwistDamping, pContextElement->FirstChildElement("ShoulderTwistDamping"));
	_Contents(&fShoulderSwingSpring, pContextElement->FirstChildElement("ShoulderSwingSpring"));
	_Contents(&fShoulderSwingDamping, pContextElement->FirstChildElement("ShoulderSwingDamping"));
	_Contents(&fVertLimitRatio, pContextElement->FirstChildElement("VertLimitRatio"));
	_Contents(&fVertSpring, pContextElement->FirstChildElement("VertSpring"));
	_Contents(&fVertDamping, pContextElement->FirstChildElement("VertDamping"));
	_Contents(&fVertTwistSpring, pContextElement->FirstChildElement("VertTwistSpring"));
	_Contents(&fVertTwistDamping, pContextElement->FirstChildElement("VertTwistDamping"));
	_Contents(&fVertSwingSpring, pContextElement->FirstChildElement("VertSwingSpring"));
	_Contents(&fVertSwingDamping, pContextElement->FirstChildElement("VertSwingDamping"));
}

void RPhysXRagDollFactor::ChangeRagDollControllerFromFactor( RPhysXActorController* pController )
{
	if (NULL == pController)
		return;

	NxSphericalJointDesc spDesc;
	NxSphericalJoint* pSpJoint = NULL;

	#define _SET_SP_JOINT( _id, _preFix, _limitRatio )	{			\
		pSpJoint = dynamic_cast<NxSphericalJoint*>(pController->GetNxJointFromResourceMap(RPhysXRagDollBuilder::##_id##));	\
		if (pSpJoint) {												\
			pSpJoint->saveToDesc(spDesc);							\
			spDesc.jointSpring.spring = f##_preFix##Spring;			\
			spDesc.jointSpring.damper = f##_preFix##Damping;		\
			spDesc.swingSpring.spring = f##_preFix##SwingSpring;	\
			spDesc.swingSpring.damper = f##_preFix##SwingDamping;	\
			spDesc.twistSpring.spring = f##_preFix##TwistSpring;	\
			spDesc.twistSpring.damper = f##_preFix##TwistDamping;	\
			spDesc.swingLimit.value = NxPi * _limitRatio;			\
			pSpJoint->loadFromDesc(spDesc);							\
		}\
	}

	// 척추계열
	_SET_SP_JOINT(E_VERT_1, Vert, fVertLimitRatio);
	_SET_SP_JOINT(E_VERT_2, Vert, fVertLimitRatio);

	// 어깨 / 엉덩이
	_SET_SP_JOINT(E_LEFT_SHOULDER, Shoulder, fShoulderLimitRatio);
	_SET_SP_JOINT(E_RIGHT_SHOULDER, Shoulder, fShoulderLimitRatio);
	_SET_SP_JOINT(E_LEFT_HIP, Hip, fHipLimitRatio );
	_SET_SP_JOINT(E_RIGHT_HIP, Hip, fHipLimitRatio );


	NxRevoluteJointDesc rvDesc;
	NxRevoluteJoint* pRvJoint = NULL;

	#define _SET_RV_JOINT( _id, _preFix )	{						\
		pRvJoint = dynamic_cast<NxRevoluteJoint*>(pController->GetNxJointFromResourceMap(RPhysXRagDollBuilder::##_id##));	\
		if (pRvJoint) {												\
			pRvJoint->saveToDesc(rvDesc);							\
			rvDesc.spring.spring = f##_preFix##Spring;				\
			rvDesc.spring.damper = f##_preFix##Damping;				\
			if(rvDesc.limit.low.value < 0 ){rvDesc.limit.low.value = -NxPi * f##_preFix##LimitRatio;}	\
			else{ rvDesc.limit.high.value = NxPi * f##_preFix##LimitRatio; }							\
			pRvJoint->loadFromDesc(rvDesc);							\
		}\
	}

	// 팔꿈치 / 무릎
	_SET_RV_JOINT(E_LEFT_ELBOW, Elbow);
	_SET_RV_JOINT(E_RIGHT_ELBOW, Elbow);
	_SET_RV_JOINT(E_LEFT_KNEE, Knee);
	_SET_RV_JOINT(E_RIGHT_KNEE, Knee);

	// Density 변경
	pController->SetResourceNxActorDensity(fDensity);
}

//////////////////////////////////////////////////////////////////////////
//
// Controller Builder for RagDoll
//
//////////////////////////////////////////////////////////////////////////


NxSphericalJoint* CreateBodySphericalJoint( NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis, float fSwingLimit)
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
	//sphericalDesc.swingLimit.value = (NxReal)0.25*NxPi;
	sphericalDesc.swingLimit.value = (NxReal)fSwingLimit*NxPi;
	sphericalDesc.swingLimit.hardness = 0.5;
	sphericalDesc.swingLimit.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	//sphericalDesc.twistSpring.spring = 0.5;
	//sphericalDesc.twistSpring.damper = 1;

	sphericalDesc.twistSpring.spring = 100;
	sphericalDesc.twistSpring.damper = 500;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	//sphericalDesc.swingSpring.spring = 0.5;
	//sphericalDesc.swingSpring.damper = 1;

	sphericalDesc.swingSpring.spring = 100;
	sphericalDesc.swingSpring.damper = 500;

	sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
	sphericalDesc.jointSpring.spring = 1000;
	sphericalDesc.jointSpring.damper = 5000;

	//// 샘플 기본 값
	//sphericalDesc.projectionDistance = (NxReal)0.15;
	//sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;

	//sphericalDesc.projectionDistance = 1.f;
	//sphericalDesc.projectionDistance = 0.5f;
	sphericalDesc.projectionDistance = 0.7f;
	sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;

	return (NxSphericalJoint*)scene->createJoint(sphericalDesc);
}

NxRevoluteJoint* CreateRevoluteJoint( NxScene* scene, NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis, bool bBottomLimit, float fJointLimit )
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	revDesc.flags |= NX_RJF_LIMIT_ENABLED;
	if (bBottomLimit)
	{
		revDesc.limit.high.value = 0;
		revDesc.limit.low.value = -NxPi * fJointLimit;
	}
	else
	{
		revDesc.limit.high.value = NxPi * fJointLimit;
		revDesc.limit.low.value = 0;
	}

	revDesc.flags |= NX_RJF_SPRING_ENABLED;
	revDesc.spring.spring = 1;
	revDesc.spring.damper = 5;

	//revDesc.projectionDistance = 1.f;
	//revDesc.projectionDistance = 0.5f;
	revDesc.projectionDistance = 0.7f;
	revDesc.projectionMode = NX_JPM_POINT_MINDIST;

	return (NxRevoluteJoint*)scene->createJoint(revDesc);
}

NxActor* CreateCapsuleBone( NxScene* scene, const NxVec3& actorpos, const NxReal height, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	bodyDesc.solverIterationCount = 12;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;

	//capsuleDesc.localPose.t.set(0, 0, -height/2);
	// 눕혀있는 캡슐 세워주기
	NxMat33 matRot;
	matRot.rotZ(NxPi*0.5f);
	capsuleDesc.localPose.M = matRot;

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

NxActor* CreateSphereBone( NxScene* scene, const NxVec3& pos, const NxMat33& rot, const NxReal radius, const NxReal density )
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	bodyDesc.solverIterationCount = 12;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	//sphereDesc.localPose.M = rot;
	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;

	actorDesc.globalPose.M = rot;

	return scene->createActor(actorDesc);
}

RPhysXRagDollBuilder::RPhysXRagDollBuilder()
{
	m_fDensity = 1.f;
	m_fCapsuleRatio = 4.f;

	m_fKneeJointLimit = 0.7f;
	m_fElbowJointLimit = 0.3f;
	m_fHipJointLimit = 0.2f;
	m_fShoulderJointLimit = 0.25f;

	m_fSphereSize[E_SPINE2]	= 11.f;
	m_fSphereSize[E_SPINE1]	= 10.f;
	m_fSphereSize[E_PELVIS]	= 12.f;


	//////////////////////////////////////////////////////////////////////////
	// Sphere
	//
	// 머리 / 가슴 / 골반
	m_aryRefBip[E_SPINE2]			= RBIPID_SPINE2;
	m_aryRefBip[E_SPINE1]			= RBIPID_SPINE1;
	m_aryRefBip[E_PELVIS]			= RBIPID_PELVIS;


	//////////////////////////////////////////////////////////////////////////
	// Capsule, 캡슐의 끝부분, 즉, 자식의 bip로 설정한후에 재설정 해준다.
	//
	// 상박 / 하박
	m_aryRefBip[E_LEFT_UPPERARM]	= RBIPID_LFOREARM;
	m_aryRefBip[E_RIGHT_UPPERARM]	= RBIPID_RFOREARM;

	m_aryRefBip[E_LEFT_FOREARM]		= RBIPID_LHAND;
	m_aryRefBip[E_RIGHT_FOREARM]	= RBIPID_RHAND;

	// 허벅지 / 종아리
	m_aryRefBip[E_LEFT_THIGH]		= RBIPID_LCALF;
	m_aryRefBip[E_RIGHT_THIGH]		= RBIPID_RCALF;

	m_aryRefBip[E_LEFT_CALF]		= RBIPID_LFOOT;
	m_aryRefBip[E_RIGHT_CALF]		= RBIPID_RFOOT;
}

bool RPhysXRagDollBuilder::Build( RPhysXActorController* pPhysXController )
{
	bool bResult = true;

	// 리소스 빌드에 필요한 객체들
	RActor* pActor = GetActor(pPhysXController);
	NxScene* pNxScene = GetNxScene(pPhysXController);
	_ASSERT(pActor);
	_ASSERT(pNxScene);

	// 리소스를 빌드하기전에 설정할 것들
	//pActor->Pause();
	pActor->Stop();


	//////////////////////////////////////////////////////////////////////////
	//
	// Build NxActor
	//
	//////////////////////////////////////////////////////////////////////////

	RVector vPos, vScale;
	RQuaternion qOrient;
	NxQuat qNxOrient;
	float fScale = pActor->GetScale().x;

	NxActor* aryNxActor[E_BONE_NUM] = {0, };
	RBIPID aryBipd[E_BONE_NUM];

	for(int i=0;i<E_BONE_NUM;i++)
	{
		NxActor* pNxActor = NULL;
		RBIPID eBipd;
		DWORD dwAttr = 0;

		// Sphere bone --
		if (i >=0 && i <= E_PELVIS )
		{
			eBipd = m_aryRefBip[i];
			const RMatrix& mat = pActor->GetActorNode( RSkeleton::GetBipName(eBipd) )->GetWorldTransform();
			vPos.Set(mat._41, mat._42, mat._43);

			NxMat33 rot;
			rot(0, 0) = mat._11;
			rot(1, 0) = mat._12;
			rot(2, 0) = mat._13;

			rot(0, 1) = mat._21;
			rot(1, 1) = mat._22;
			rot(2, 1) = mat._23;

			rot(0, 2) = mat._31;
			rot(1, 2) = mat._32;
			rot(2, 2) = mat._33;

			pNxActor = CreateSphereBone(pNxScene, NxVec3(vPos.x, vPos.y, vPos.z), rot, m_fSphereSize[i] * fScale, m_fDensity);
		}

		// Capsule bone --
		else
		{
			//  NxActor 생성 정보
			float fRadius = m_fCapsuleRatio * fScale;
			float fHeight = (GetCapsuleLengthFromChildBip( pActor, m_aryRefBip[i] ) - (fRadius * 2) ) * fScale;

			eBipd = RSkeleton::GetBipParent(m_aryRefBip[i]);	// 실린더 타입의 경우 길이를 재기위해 자식으로 적혀있다.
			const RMatrix& mat = pActor->GetActorNode( RSkeleton::GetBipName(eBipd) )->GetWorldTransform();
			mat.Decompose(vScale, vPos, qOrient);
			qNxOrient.setXYZW( qOrient.x, qOrient.y, qOrient.z, qOrient.w);

			RVector vOffSetForCapsule(mat._11, mat._12, mat._13);
			vOffSetForCapsule.Normalize();
			vOffSetForCapsule *= (fHeight/2 + fRadius);
			vPos += vOffSetForCapsule;

			// NxActor 설정
			pNxActor = CreateCapsuleBone(pNxScene, NxVec3(vPos.x, vPos.y, vPos.z), fHeight, fRadius, m_fDensity);
			if ( NULL != pNxActor )
			{
				pNxActor->setGlobalOrientationQuat( qNxOrient );
			}
		}

		aryNxActor[i] = pNxActor;
		aryBipd[i] = eBipd;

		AddNxActorResource(pPhysXController, i, pNxActor);
		if (NULL == pNxActor)
		{
			bResult = false;
		}
		else
		{
			//pNxActor->setSleepLinearVelocity(1000);
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// Build NxJoint
	//
	//////////////////////////////////////////////////////////////////////////

	RVector vActorNodePos;
	RVector vActorNodeUp;

	NxVec3 vNxPos;
	NxVec3 vNxUpAxis;

	NxJoint* aryJoint[E_JOINT_NUM] = {0, };
	for(int i=0;i<E_JOINT_NUM;i++)
	{
		NxJoint* pJoint = NULL;
		RMatrix matRot;

		int nUpActor = i;
		int nBottomActor = i + 1;

		float fSwingLimit = 0.25f; // 척추 swing limit ratio
		float fRevolueLimit = 1;

		bool bBottomLimit = true;
		if (i == E_LEFT_SHOULDER || i == E_RIGHT_SHOULDER)
		{
			nUpActor = E_SPINE2;
			fSwingLimit = m_fShoulderJointLimit;
		}
		else if ( i == E_LEFT_HIP || i == E_RIGHT_HIP )
		{
			nUpActor = E_PELVIS;
			nBottomActor = i + (E_LEFT_THIGH - E_LEFT_HIP);
			fSwingLimit = m_fHipJointLimit;
		}
		else if (i == E_LEFT_ELBOW || i == E_RIGHT_ELBOW)
		{
			nUpActor = i + ( E_LEFT_UPPERARM - E_LEFT_ELBOW );
			nBottomActor = nUpActor + 2;
			matRot.MakeIdentity();

			fRevolueLimit = m_fElbowJointLimit;

			if (E_RIGHT_ELBOW == i)
			{
				bBottomLimit = false;
			}
		}
		else if ( i == E_LEFT_KNEE || i == E_RIGHT_KNEE )
		{
			nUpActor = i + ( E_LEFT_THIGH - E_LEFT_KNEE );
			nBottomActor = nUpActor + 2;
			matRot.SetRotationZ(MMath::HALFPI);

			fRevolueLimit = m_fKneeJointLimit;
		}

		RBIPID eBip = aryBipd[nBottomActor];
		const RMatrix& mat = pActor->GetActorNode( RSkeleton::GetBipName(eBip) )->GetWorldTransform();
		vActorNodePos.Set(mat._41, mat._42, mat._43);

		// Spherical Joint
		if (i >=0 && i <= E_RIGHT_HIP )
		{
			vActorNodeUp.Set(mat._11, mat._12, mat._13);
			vActorNodeUp.Normalize();
			vActorNodePos += ( m_fSphereSize[nBottomActor] * vActorNodeUp );

			vNxUpAxis.set(-vActorNodeUp.x, -vActorNodeUp.y, -vActorNodeUp.z);
			vNxPos.set(vActorNodePos.x, vActorNodePos.y, vActorNodePos.z);

			pJoint = CreateBodySphericalJoint(pNxScene, aryNxActor[nUpActor], aryNxActor[nBottomActor], vNxPos, vNxUpAxis, fSwingLimit);
		}

		// revolute Joint, 팔꿈치 / 무릎
		else
		{
			vActorNodeUp.Set(mat._21, mat._22, mat._23);
			matRot.TransformVect(vActorNodeUp);
			vActorNodeUp.Normalize();

			vNxUpAxis.set(vActorNodeUp.x, vActorNodeUp.y, vActorNodeUp.z);
			vNxPos.set(vActorNodePos.x, vActorNodePos.y, vActorNodePos.z);

			pJoint = CreateRevoluteJoint(pNxScene, aryNxActor[nUpActor],aryNxActor[nBottomActor],vNxPos,vNxUpAxis, bBottomLimit, fRevolueLimit);
		}

		AddNxJointResource(pPhysXController, i, pJoint);
		aryJoint[i] = pJoint;
		if (NULL == pJoint)
		{
			bResult = false;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// Add Update Info
	//
	//////////////////////////////////////////////////////////////////////////

	// root, pelvis로 부터 Update된다.
	DWORD dwAttr = RPhysXActorController::APPLY_ACTOR_POS | RPhysXActorController::APPLY_ACTOR_AXIS | RPhysXActorController::ADD_HIERARCHY_ATTR;
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(RBIPID_ROOT), aryNxActor[E_PELVIS], NULL, dwAttr | RPhysXActorController::ADD_BIP_ROOT_ATTR );

	// pelvis
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(RBIPID_PELVIS), aryNxActor[E_PELVIS], NULL, dwAttr | RPhysXActorController::ADD_PELVIS_ATTR );

	// 하체, pelvis - spine 으로 계층되어 있음
	dwAttr = RPhysXActorController::APPLY_JOINT_POS | RPhysXActorController::APPLY_ACTOR_AXIS | RPhysXActorController::ADD_HIERARCHY_ATTR;
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_LEFT_THIGH]), aryNxActor[E_LEFT_THIGH], aryJoint[E_LEFT_HIP], dwAttr);
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_LEFT_CALF]), aryNxActor[E_LEFT_CALF], aryJoint[E_LEFT_KNEE], dwAttr);

	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_RIGHT_THIGH]), aryNxActor[E_RIGHT_THIGH], aryJoint[E_RIGHT_HIP], dwAttr);
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_RIGHT_CALF]), aryNxActor[E_RIGHT_CALF], aryJoint[E_RIGHT_KNEE], dwAttr);

	// 상체 - spine1 - spine2 로 계층되어 있음
	dwAttr = RPhysXActorController::APPLY_ACTOR_POS | RPhysXActorController::APPLY_ACTOR_AXIS | RPhysXActorController::ADD_HIERARCHY_ATTR;
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_SPINE1]), aryNxActor[E_SPINE1], NULL, dwAttr);
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_SPINE2]), aryNxActor[E_SPINE2], NULL, dwAttr);

	// 팔
	dwAttr = RPhysXActorController::APPLY_JOINT_POS | RPhysXActorController::APPLY_ACTOR_AXIS | RPhysXActorController::ADD_HIERARCHY_ATTR;
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_LEFT_UPPERARM]), aryNxActor[E_LEFT_UPPERARM], aryJoint[E_LEFT_SHOULDER], dwAttr | RPhysXActorController::ADD_LEFT_ATTR);
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_LEFT_FOREARM]), aryNxActor[E_LEFT_FOREARM], aryJoint[E_LEFT_ELBOW], dwAttr);

	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_RIGHT_UPPERARM]), aryNxActor[E_RIGHT_UPPERARM], aryJoint[E_RIGHT_SHOULDER], dwAttr | RPhysXActorController::ADD_RIGHT_ATTR);
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(aryBipd[E_RIGHT_FOREARM]), aryNxActor[E_RIGHT_FOREARM], aryJoint[E_RIGHT_ELBOW], dwAttr);

	// 손
	dwAttr = RPhysXActorController::ADD_HIERARCHY_ATTR | RPhysXActorController::ADD_ANKLE_ATTR;
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(RBIPID_LHAND), aryNxActor[E_LEFT_FOREARM], NULL,  dwAttr | RPhysXActorController::ADD_LEFT_ATTR);
	PushBackNxUpdateInfo(pPhysXController, RSkeleton::GetBipName(RBIPID_RHAND), aryNxActor[E_RIGHT_FOREARM], NULL, dwAttr | RPhysXActorController::ADD_RIGHT_ATTR);

	// twist
	dwAttr = RPhysXActorController::ADD_HIERARCHY_ATTR | RPhysXActorController::ADD_LINKEDNODE_DATA;
	PushBackNxUpdateInfo(pPhysXController, "Bip01 LThighTwist", NULL, NULL, dwAttr, RSkeleton::GetBipName(aryBipd[E_LEFT_THIGH]) );
	PushBackNxUpdateInfo(pPhysXController, "Bip01 RThighTwist", NULL, NULL, dwAttr, RSkeleton::GetBipName(aryBipd[E_RIGHT_THIGH]) );
	PushBackNxUpdateInfo(pPhysXController, "Bip01 LUpArmTwist", NULL, NULL, dwAttr, RSkeleton::GetBipName(aryBipd[E_LEFT_UPPERARM]) );
	PushBackNxUpdateInfo(pPhysXController, "Bip01 RUpArmTwist", NULL, NULL, dwAttr, RSkeleton::GetBipName(aryBipd[E_RIGHT_UPPERARM]) );

	// Set Pelivs Inverse Transform to 'Hierarchy Root Inverse' and Update Map Refresh
	RActorNode* pPelvis = pActor->GetActorNode( RSkeleton::GetBipName(RBIPID_PELVIS) );
	if ( pPelvis )
	{
		RMatrix matInv;
		pPelvis->GetResultTransform().GetInverse(&matInv);
		SetHierarchyRootInvTransform(pPhysXController, matInv);
	}
	RefreshNxUpdateInfoMap(pPhysXController);
	return bResult;
}

float RPhysXRagDollBuilder::GetCapsuleLengthFromChildBip( RActor* pActor, RBIPID bipId )
{
	_ASSERT( pActor != NULL);
	_ASSERT( bipId != RBIPID_UNKNOWN);

	RActorNode* pActorNode = pActor->GetActorNode( RSkeleton::GetBipName(bipId) );
	RMatrix matLocal = pActorNode->m_pMeshNode->m_matLocal;
	return (RVector::ZERO * matLocal).Length();
}

}
