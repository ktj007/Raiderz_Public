#include "stdafx.h"
#include "MMTimer.h"

namespace mint3 {

bool MMTimer::OnEvent(MEvent* pEvent, MListener* pListener)
{

	return false;
}

MMTimer::MMTimer(const char* szName, MWidget* pParent, MListener* pListener) : MWidget(szName, pParent, pListener), 
					m_nInterval(MINT_DEFAULT_TIMER_INTERVAL), m_nTime(0), m_nLastTime(0)
{
	InitTimer();
}

MMTimer::~MMTimer(void)
{

}

bool MMTimer::OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	return false;
}

void MMTimer::OnRun(void)
{
	if ((!IsVisible()) || (!IsEnable())) return;

	unsigned long int nNowTime = timeGetTime();
	unsigned long int delta = nNowTime - m_nLastTime;
	m_nLastTime = nNowTime;

	m_nTime += delta;

	if (m_nTime >= m_nInterval) 
	{
		m_nTime = 0;
		OnTick();
	}
	
}
void MMTimer::OnDraw(MDrawContext* pDC)
{

}

void MMTimer::OnEnabled(bool bEnabled)
{
	if (bEnabled) InitTimer();
}

void MMTimer::OnShow()
{
	InitTimer();
}

void MMTimer::InitTimer()
{
	m_nLastTime = timeGetTime();
}

void MMTimer::OnTick()
{
	m_EventHandler.OnCommand(this, MEVENT_TIMER_TICK);
}


void MMTimer::glueSetInterval(unsigned long nInterval)
{
	if (nInterval == 0) return;
	SetInterval(nInterval);
}

} // namespace mint3