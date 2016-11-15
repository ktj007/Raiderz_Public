#pragma once

#include "XUIDGenerator.h"
#include "XTimer.h"


class XSystem
{
protected:
	bool				m_bResourceLoading;

	XUIDGenerator		m_UIDGenerator;
	XTimer				m_Timer;
public:
	XSystem() : m_bResourceLoading(true) {}
	virtual ~XSystem() {}
	virtual bool Init();
	virtual MUID GenerateLocalUID();
	void SetResourceLoading(bool b) { m_bResourceLoading = b; }
	bool IsResourceLoading() { return m_bResourceLoading; }
	virtual unsigned int GetNowTime()		{ return m_Timer.GetNowTime(); }

	XTimer& GetTimer() { return m_Timer; }

	// cursor
	virtual void ShowCursor(bool bShow);
	virtual void SetCursorPos(int x, int y);
};

inline unsigned int XGetNowTime()
{
	if (!global.system) return 0;
	return global.system->GetNowTime();
}
