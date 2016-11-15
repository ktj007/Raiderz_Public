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

class TCharacter;

/// Actor의 애니메이션 관리자
class TTransformControllerManager : public XTransformControllerManager
{
private:
	void					CreateAnimationTC(TCharacter* pCharacter);

	void					UpdateActorNode();

public:
	TTransformControllerManager(TCharacter* pCharacter);

	virtual ~TTransformControllerManager();

	//////////////////////////////////////////////////////////////////////////
	// Base Animation
	virtual void			Update(float fDelta);

	virtual bool 			SetAnimation(string& strAniName, bool bForce=false, bool bSameFrame = false, bool bTestPlay = false);

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

	virtual string			GetAnimationName();
	virtual int				GetAniFrame();
	virtual int				GetAniMaxFrame();
	RAnimation*				GetAnimation();

	virtual bool			IsAnimationLoop();
	virtual bool			IsPlayDone();
	virtual bool			IsOncePlayDone();

	bool					ReSetAnimation();

	//////////////////////////////////////////////////////////////////////////
	// Face Animation
	void					SetFaceAnimationActive(bool bActive);
	void					SetFaceAnimationNPC(bool bNPC, TCharacter * pActor);

	void					StartFaceAnimation(string& strAniName, bool bEye);
	void					EndFaceAnimation(string& strAniName, float fEndBledTime);

	void					StopFaceAnimation();

	//////////////////////////////////////////////////////////////////////////
	// Upper Animation
	void					InitUpperAnimation();

	virtual bool			SetUpperAnimation(string strUpperAniName);

	virtual bool			IsUpperAnimationOncePlayDone();
	virtual bool			IsUpperAnimationPlayDone();
	virtual bool			IsUpperAnimationLoop();


	//////////////////////////////////////////////////////////////////////////
	void					InitFakeBeatenAnimation(tstring& strAniName, float fWeight, float fSpeed);
	void					StartFakeBeaten(const TCHAR* szCurAni, const float fWeightRate, const float fSpeedRate, const TCHAR* szAniPrefix=NULL);

};

