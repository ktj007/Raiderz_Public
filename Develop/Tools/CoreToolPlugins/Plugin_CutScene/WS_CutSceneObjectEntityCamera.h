#pragma once
#include "WS_CutSceneObjectEntity.h"

class CCutSceneObjectEntityCamera : public CCutSceneObjectEntity
{
public:
	CCutSceneObjectEntityCamera(CControlMediator* pControlMediator);
	virtual ~CCutSceneObjectEntityCamera();

	virtual void SetTransform(const MMatrix& mat, bool bUsingCollision);
	virtual rs3::RCameraSceneNode* GetRelatedCamera();
	virtual void SetAnimation(const std::string& _rAnimationName);
};