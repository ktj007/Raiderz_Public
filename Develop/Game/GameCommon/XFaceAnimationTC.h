#pragma once

#include "XRAnimationController.h"
#include "XTransformController.h"

struct stFaceAnimationList
{
	tstring						strAniName;

	float						fFaceEndElapsed;
	float						fFaceEndTime;

	XRAnimationController *		pFaceAnimationController;

	bool						bEye;

	stFaceAnimationList()
	{
		fFaceEndElapsed			= 0.0f;
		fFaceEndTime			= 0.0f;

		bEye					= false;
	}
};

typedef vector<stFaceAnimationList> FaceAnimationList;

class XFaceAnimationTC : public XTransformController
{
protected:
	XRAnimationController *				m_pBlinkAnimationController;	
	FaceAnimationList					m_vecFaceAnimationController;
	FaceAnimationList					m_vecEyeAnimationController;

	float								m_fBlinkElapsed;
	float								m_fBlinkStartTime;

	bool								m_bNPC;
	bool								m_bEnableBlink;

private:
	void								BlinkPlay(float fDelta);
	void								FaceAnimationEndPlay(float fDelta);

	bool								SetFaceAnimation(tstring& strAniName, string strNodeName, bool bEye, FaceAnimationList& animationList);

	void								UpdateAnimationController(FaceAnimationList& animationList);
	void								UpdateEndAnimationController(FaceAnimationList& animationList, float fDelta);

	void								DelAllAnimationController(FaceAnimationList& animationList);
	void								EndAnimationController(FaceAnimationList& animationList, tstring& strAniName, float fEndBledTime);

public:
	XFaceAnimationTC(XCharacter* pCharacter);
	virtual ~XFaceAnimationTC(void);

	virtual void						PreUpdate(float fDelta);
	virtual void						Update(float fDelta);
	virtual void						Stop(){}

	void								SetNPC(bool bNPC)			{ m_bNPC = bNPC; }
	void								SetEnableBlink(bool bEnable) { m_bEnableBlink = bEnable; }
	
	void								StartFaceAnimation(tstring& strAniName, bool bEye);
	void								EndFaceAnimation(tstring& strAniName, float fEndBledTime);
	void								EndAllFaceLoopAnimation();

	static TRANSFORM_CONTROLLER_LIST	GetID() { return TCL_FACE; }
};
