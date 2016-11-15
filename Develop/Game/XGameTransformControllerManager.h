#pragma once

#include <string>
#include "RSceneNode.h"
#include "RAnimation.h"
#include "RAnimationDef.h"
using namespace rs3;

#include "XTransformController.h"
#include "XTransformControllerManager.h"
#include "MPhysX4R3/MPxAniLookUpTC.h"

using namespace physx;
namespace physx{ class MPxAniLookUpTC; }

/// Actor의 애니메이션 관리자
class XGameTransformControllerManager : public XTransformControllerManager
{
protected:
	MPxAniLookUpTC* m_pAniLookUpTransformController;

	float			m_fElapsedTime;

	bool			m_bFreezingFrame;
	float			m_fFreezingFrame;
	float			m_fFreezingFrameDelta;

	void CheckFreezingFrame( float fDelta );

private:
	void					CreateAnimationTC(XCharacter* pCharacter);
	bool					CheckCameraViewFrustum();

public:
	XGameTransformControllerManager(XCharacter* pCharacter, MPxAniLookUpTC* pAniLookUpTransformController);

	virtual ~XGameTransformControllerManager();

	//////////////////////////////////////////////////////////////////////////
	// Base Animation
	virtual void			Update(float fDelta);

	virtual bool 			SetAnimation(wstring& strAniName, bool bForce=false, bool bSameFrame = false);

	virtual float			GetElapsedTime()		{ return m_fElapsedTime; }

	virtual void			CloseAnimationEvent();

	void					FreezeFrame(float fFreezingTime);		// fFreezingTime동안 애니메이션 멈추기

	void					OnLoadingComplete();

	virtual float			GetAnimationSpeed();
	virtual void			SetAnimationSpeed(float fSpeed);
	virtual void			SetAnimationFrame(unsigned int nFrame);
	virtual float			GetMaxFrameToSecond();
	virtual float			GetCurFrameToSecond();

	virtual void			ResetAniamtionSpeed();
	virtual float			GetOrgAniamtionSpeed();


	virtual void			PauseAnimation();
	virtual void			StopAnimation();
	virtual void			PlayAnimation();

	virtual wstring			GetAnimationName();
	virtual int				GetAniFrame();
	virtual int				GetAniFrameWithUpdateTime();
	virtual int				GetAniMaxFrame();
	RAnimation*				GetAnimation();

	wstring					GetPreAnimationName();
	int						GetPreAniFrame();
	float					GetPreAniFrameToSec(unsigned int nFrame);

	bool					GetActorNodePosByFrame(wstring& strNodeName, unsigned int nFrame, vec3& vOut);

	vector<ANIMATION_EVENT>* GetAnimationEvent();

	virtual bool			IsAnimationLoop();
	virtual bool			IsPlayDone();
	virtual bool			IsOncePlayDone();
	bool					IsAnimationUsable();

	void					UseDummyGrip();
	void					UseAttachActorNode(wstring strActorNodeName);

	//////////////////////////////////////////////////////////////////////////
	// Face Animation
	void					SetFaceAnimationActive(bool bActive);
	void					SetFaceBlinkAnimationActive(bool bActive);
	void					SetFaceAnimationNPC(bool bNPC);

	void					StartFaceAnimation(wstring& strAniName, bool bEye);
	void					EndFaceAnimation(wstring& strAniName, float fEndBledTime);

	//////////////////////////////////////////////////////////////////////////
	// Upper Animation
	void					InitUpperAnimation();

	virtual bool			SetUpperAnimation(wstring strUpperAniName);
	virtual void			SetUpperFrame(int nFrame);
	virtual void			SetUpperAnimationSpeed(float fSpeed);

	virtual void			StopUpperAnimation();

	virtual int				GetUpperFrame();
	virtual int				GetUpperAniMaxFrame();
	virtual float			GetUpperAnimationSpeed();
	virtual float			GetUpperMaxFrameToSecond();

	virtual bool			IsUpperAnimationOncePlayDone();
	virtual bool			IsUpperAnimationPlayDone();
	virtual bool			IsUpperAnimationLoop();

	//////////////////////////////////////////////////////////////////////////
	void					InitFakeBeatenAnimation(tstring& strAniName, float fWeight, float fSpeed);
	bool					StartFakeBeaten(const TCHAR* szCurAni, const float fWeightRate, const float fSpeedRate, const TCHAR* szAniPrefix=NULL);
};

