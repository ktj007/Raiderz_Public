#pragma once

class XMapSignAreaChecker
{
protected:
	XRegulator				m_CheckRegulator;

public:
	XMapSignAreaChecker(void);
	~XMapSignAreaChecker(void);
	
	void UpdateTouchAreaTrigger(XObject* pOwner, float fDelta );
};
