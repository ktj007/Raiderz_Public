#pragma once

#include "RActorController.h"
#include "RNewton.h"

namespace rs3 {

enum RAGDOLLBONETYPE {
	RAGDOLLBONETYPE_BOX,
	RAGDOLLBONETYPE_CYLINDER,
	RAGDOLLBONETYPE_SPHERE,
};

struct ragDollBones;

class RRagDollSkeletonController : public RActorController
{
	const NewtonWorld* nWorld;
	NewtonApplyForceAndTorque m_callBackNewtonApplyForceAndTorque;

	ragDollBones *bones;
	NewtonRagDoll* m_pRagDoll;

	bool	m_bShowBones;

	float GetParentBoneLength( RBIPID bipId);

	void SetCurrent(RBIPID bipId,const RMatrix& matTransform);

	RMatrix	m_matRoot;

public:
	RRagDollSkeletonController(const NewtonWorld* pNWorld, NewtonApplyForceAndTorque callBackNewtonApplyForceAndTorque);
	virtual ~RRagDollSkeletonController(void);

	void DestroyRagDoll();
	void BuildRagDoll();

	void ShowBones(bool bShow);

	virtual void Update();
	virtual void OnAdd();
	virtual void OnRemove();
};

}