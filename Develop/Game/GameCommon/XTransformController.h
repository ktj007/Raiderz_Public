#pragma once

#include "XCharacter.h"
#include "RActorController.h"
#include "XRAnimationController.h"
#include "RAnimationResource.h"

// 컨트롤 업데이트 순서이다. 잘 지키자.
enum TRANSFORM_CONTROLLER_LIST
{
	TCL_NONE,
	TCL_BASE,
	TCL_PHYSX,
	TCL_UPPER,
	TCL_FACE,
	TCL_FAKE_BEATEN,

	TCL_OVERLAY,


	TCL_MAX
};

enum ANIMATION_CHECK_STATE
{
	AS_NONE,			// 없다.
	AS_MODEL_LOADING,	// 모델 로딩중
	AS_LOADING,			// 애니메이션 로딩중
	AS_LOADED			// 애니메이션 로딩완료
};

class XTransformController
{
protected:
	XCharacter*							m_pActor;
	bool								m_bEnable;
	
public:
	XTransformController(XCharacter* pCharacter);
	virtual ~XTransformController();

	void								Destroy();

	virtual void						PreUpdate(float fDelta) = 0;
	virtual void						Update(float fDelta);
	virtual void						PostUpdate(float fDelta) {}

	virtual void						GetActiveRActorNodeList(vector<tstring>& vecActorNodeList) {}
	XCharacter*							GetActor()				{ return m_pActor; }

	ANIMATION_CHECK_STATE				CheckAnimationLoadingComplete(tstring& strAniName);
	ANIMATION_CHECK_STATE				CheckAnimationUsable(tstring& strAniName);

	void								SetEnable(bool bEnable)	{ m_bEnable = bEnable; }	
	bool								IsEnabled()				{ return m_bEnable; }

	static TRANSFORM_CONTROLLER_LIST	GetID() { return TCL_NONE; }
};