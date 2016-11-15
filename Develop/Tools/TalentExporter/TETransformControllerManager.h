#pragma once

//#include <string>
//#include "RSceneNode.h"
//#include "RAnimation.h"
//#include "RAnimationDef.h"
//using namespace rs3;

#include "XTransformController.h"
#include "XTransformControllerManager.h"
#include "MPhysX4R3/MPxAniLookUpTC.h"

using namespace physx;
namespace physx{ class MPxAniLookUpTC; }

class TECharacter;

/// Actor의 애니메이션 관리자
class TETransformControllerManager : public XTransformControllerManager
{
private:
	void					CreateAnimationTC(TECharacter* pCharacter);

	void					UpdateActorNode();

public:
	TETransformControllerManager(TECharacter* pCharacter);

	virtual ~TETransformControllerManager();

	//////////////////////////////////////////////////////////////////////////
	// Base Animation
	virtual void			Update(float fDelta);

	virtual bool 			SetAnimation(tstring& strAniName, bool bForce=false, bool bSameFrame = false, bool bTestPlay = false);

	virtual void			CloseAnimationEvent();

	void					OnLoadingComplete();

	virtual float			GetAnimationSpeed();
	virtual void			SetAnimationSpeed(float fSpeed);
	virtual void			SetAnimationFrame(unsigned int nFrame);
	virtual float			GetMaxFrameToSecond();
	virtual float			GetCurFrameToSecond();

	virtual void			PauseAnimation();
	virtual void			StopAnimation();
	virtual void			PlayAnimation();

	virtual tstring			GetAnimationName();
	virtual int				GetAniFrame();
	virtual int				GetAniMaxFrame();
	RAnimation*				GetAnimation();

	virtual bool			IsAnimationLoop();
	virtual bool			IsPlayDone();
	virtual bool			IsOncePlayDone();

	bool					ReSetAnimation();
};

