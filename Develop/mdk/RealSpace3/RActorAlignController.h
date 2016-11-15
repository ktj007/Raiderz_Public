#pragma once

#include "RSceneNodeController.h"

namespace rs3 {

/// 카메라를 바라보는 align controller
class RActorAlignController : public RSceneNodeController
{
	RActor*		m_pActor;
public:
	DECLARE_NODE_ID_STRING("ActorAlignController");

	RActorAlignController(RActor* pActor);

	virtual void Update(RCameraSceneNode* pCamera) override;

protected:
	virtual void GetResultTransform( RMatrix& outResult, const RMatrix& matWorld, const RCameraSceneNode* pCamera );
};

/// Z축이 고정된 Align controller
class RActorZFixedAlignController : public RActorAlignController
{
public:
	DECLARE_NODE_ID_STRING("ActorZFixedAlignController");

	RActorZFixedAlignController(RActor* pActor);

protected:
	virtual void GetResultTransform( RMatrix& outResult, const RMatrix& matWorld, const RCameraSceneNode* pCamera );
};

}