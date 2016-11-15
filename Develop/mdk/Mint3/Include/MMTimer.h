#pragma once

#include "MWidget.h"

namespace mint3 {

class MMTimer : public MWidget
{
protected:
	unsigned long m_nInterval;
	unsigned long m_nTime;
	unsigned long m_nLastTime;
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual void OnEnabled(bool bEnabled);
	virtual void OnShow();
	virtual void OnRun(void);
	virtual void OnDraw(MDrawContext* pDC);
	virtual void OnTick();
	void InitTimer();
public:
	MMTimer(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MMTimer(void);
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	void SetInterval(unsigned long nInterval)	{ m_nInterval = nInterval; }
	unsigned long GetInterval()					{ return m_nInterval; }

	// glue 함수
	void glueSetInterval(unsigned long nInterval);
	unsigned long glueGetInterval()					{ return GetInterval(); }

#define MINT_TIMER		"Timer"
	virtual const char* GetClassName(void){ return MINT_TIMER; }
};


#define MINT_DEFAULT_TIMER_INTERVAL		100			// 기본값은 0.1초

} // namespace mint3