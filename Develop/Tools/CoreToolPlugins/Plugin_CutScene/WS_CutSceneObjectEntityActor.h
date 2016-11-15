#pragma once
#include "WS_CutSceneObjectEntity.h"

class CCutSceneObjectEntityActor : public CCutSceneObjectEntity
{
public:
	CCutSceneObjectEntityActor(CControlMediator* pControlMediator, const std::string& _rStrType);
	~CCutSceneObjectEntityActor();

	virtual void ResetUpdate();
	virtual void SetTransform(const MMatrix& mat, bool bUsingCollision);
	virtual void SetAnimation(const std::string& _rAnimationName);
	virtual rs3::RCameraSceneNode* GetRelatedCamera();
	virtual rs3::RSceneNode* GetSceneNodeToRender();
	void ResetEntityActorSceneNode(const char* _pSzActorType);

	rs3::RActor* GetXRActor(){ return dynamic_cast<rs3::RActor*>(m_pCutSceneActor_); };


protected:
	rs3::RActor* m_pCutSceneActor_;
};