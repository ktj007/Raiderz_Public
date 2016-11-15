#pragma once

#include "XRAnimationController.h"
#include "XTransformController.h"

class XOverlayAnimationTC : public XTransformController
{
private:
	struct OVERLAY_ANIMATION_CONTROLLER
	{
		XRAnimationController *			pOverlayAnimationController;
		tstring							strAniName;
		tstring							strBoneName;
	};

private:
	vector<OVERLAY_ANIMATION_CONTROLLER>		m_vecOverlayAnimationController;
	vector<tstring>						m_vecReservedOverlayAnimationName;

	tstring								m_strCurrentPlayAnimation;

protected:
	void								SetOverlayAnimation(tstring& strAniName, tstring& strBoneName);
	void								OverlayAnimationDel();
	void								GetNewOverlayAnimationList(OVERLAY_ANIMATION_LIST * pList, vector<OVERLAY_ANIMATION_INFO>& outList);

public:
	XOverlayAnimationTC(XCharacter* pCharacter);
	virtual ~XOverlayAnimationTC(void);

	virtual void						SetAnimation(tstring& strAniName);

	void								OverlayAnimationPlay();
	void								OverlayAnimationPause();
	void								OverlayAnimationStop();

	virtual void						PreUpdate(float fDelta);
	virtual void						Update(float fDelta);
	virtual void						Stop(){}

	void								CheckReservedAnimation();

	static TRANSFORM_CONTROLLER_LIST	GetID() { return TCL_OVERLAY; }
};
