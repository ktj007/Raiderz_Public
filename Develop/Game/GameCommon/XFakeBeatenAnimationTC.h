#pragma once

#include "XRAnimationController.h"
#include "XTransformController.h"
#include "XReActionAnimationInfo.h"

class XFakeBeatenAnimationTC : public XTransformController
{
private:
	XRAnimationController *				m_pRAnimationController;

	ReAction_DATA						m_ReActionDefalutInfo;
	ReAction_DATA *						m_pCurrentReActionInfo;

	float								m_fWeaponVisibility;

protected:
	float								m_fStartTime;
	bool								m_bActive;

protected:
	void								StopBeaten();

	void								CheckWeaponVisibility();

public:
	XFakeBeatenAnimationTC(XCharacter* pCharacter);
	virtual ~XFakeBeatenAnimationTC(void);

	void								Init(tstring& strAniName, float fWeight, float fSpeed);

	virtual void						PreUpdate(float fDelta);
	virtual void						Update(float fDelta);

	virtual void						PostUpdate(float fDelta);
	virtual void						Stop();

	bool								StartBeaten(const TCHAR* szCurAni, const float fWeightRate, const float fSpeedRate, const TCHAR* szAniPrefix=NULL);

	static TRANSFORM_CONTROLLER_LIST	GetID() { return TCL_FAKE_BEATEN; }
};
