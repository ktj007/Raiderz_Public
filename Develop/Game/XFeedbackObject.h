#pragma once

#include "XNonPlayer.h"
#include "MTime.h"

class XFeedbackObject : public XNonPlayer
{
	MDeclareRTTI;
private:
	bool	m_bBalloonVisible;
	wstring m_strName;
	wstring	m_strComment;
protected:
	virtual void					OnUpdate(float fDelta);

	MRegulator						m_BalloonResetRegulator;
	MRegulator						m_rgtDistanceCheck;
	void UpdateBalloon(float fDelta);
	void ShowBalloon();
public:
	XFeedbackObject(MUID uid);
	virtual ~XFeedbackObject();
	void Init(vec3 vPos, vec3 vDir, wstring& strName, wstring& strComment);

	virtual vec3		GetNamePos();
	virtual XObjectID				GetType() { return XOBJ_NPC; }
};