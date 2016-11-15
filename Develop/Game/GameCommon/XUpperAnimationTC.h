#pragma once

#include "XRAnimationController.h"
#include "XTransformController.h"

class XUpperAnimationTC : public XTransformController
{
private:
	XRAnimationController *				m_pRAnimationController;

	bool								m_bUseUpperAnimation;
	bool								m_bEndUpperAnimation;

	tstring								m_strLastUpperAni;
protected:

private:
	float								m_fCurrentElapsed;

public:
	XUpperAnimationTC(XCharacter* pCharacter);
	virtual ~XUpperAnimationTC(void);

	void								Init();
	virtual void						PreUpdate(float fDelta);
	virtual void						Update(float fDelta);
	virtual void						Stop();

	virtual bool 						SetAnimation(tstring& strAniName);
	virtual void						SetAnimationFrame(unsigned int nFrame);
	virtual void						SetAnimationSpeed(float fSpeed);

	virtual int							GetAniFrame();
	virtual int							GetAniMaxFrame();
	virtual float						GetAnimationSpeed();
	virtual float						GetMaxFrameToSecond();
	virtual float						GetAniFrameToSecond();

	virtual bool						IsOncePlayDone();
	virtual bool						IsPlayDone();
	virtual bool						IsLoop();

	static TRANSFORM_CONTROLLER_LIST	GetID() { return TCL_UPPER; }
};
