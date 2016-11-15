#include "StdAfx.h"
#include "RTimer.h"
#include <windows.h>
#include <mmsystem.h>

#pragma comment ( lib, "winmm.lib")

/**
 D3DXUTTimer와 거의 같다. 
 만일, QueryPerformance-Timer를 사용하고 싶다면, D3DXUTTimer의 코드로 고치면 된다.
 ( timeGetTime을 사용한 이유는 예전에 Hyper-Theading 기반의 CPU에서 QueryPerformance Timer가 비정상적 작동을 했던 경험이 있어서,
  그때 timeGetTime을 사용하여 그 문제를 해결하였다. )
*/

namespace rs3 {

RTimer::RTimer()
{
	m_bTimerStopped = true;
	m_dwStopTime = m_dwLastUpdateTime = m_dwBaseTime = 0;
	m_dwElapsedTime = 0;
	m_dwLastUpdateTime = timeGetTime();
}

void RTimer::Reset()
{
	DWORD time = timeGetTime();

	m_dwBaseTime = time;
	m_dwLastUpdateTime = time;
	m_dwStopTime = 0;
	m_dwElapsedTime = 0;
	m_bTimerStopped = false;
}

void RTimer::Start()
{
	DWORD time = timeGetTime();
	if ( m_bTimerStopped )
		m_dwBaseTime += ( time - m_dwStopTime );

	m_dwStopTime = 0;
	m_dwLastUpdateTime = time;
	m_bTimerStopped = false;
}

void RTimer::Stop()
{
	if( m_bTimerStopped == false )
	{
		DWORD time = timeGetTime();
		
		m_dwStopTime = time;
		m_bTimerStopped = true;
	}
}

void RTimer::Update()
{
	DWORD time;
	if ( m_dwStopTime != 0 )
		time = m_dwStopTime;
	else
		time = timeGetTime();

	m_dwElapsedTime = ( time - m_dwLastUpdateTime );
	m_dwLastUpdateTime = time;
}

// 시간을 강제로 흘러가게 하는것
void RTimer::UpdateCustom(DWORD dwElapsed)
{
	m_dwElapsedTime = dwElapsed;
	m_dwLastUpdateTime = m_dwLastUpdateTime + dwElapsed;
}

DWORD RTimer::GetAbsoluteTime() const
{
	return timeGetTime();
}

DWORD RTimer::GetTime() const
{
// 	DWORD time;
// 	if ( m_dwStopTime != 0 )
// 		time = m_dwStopTime;
// 	else
// 		time = timeGetTime();
// 
// 	return (time - m_dwBaseTime);
	return (m_dwLastUpdateTime - m_dwBaseTime);
}

DWORD RTimer::GetElapsedTime() const
{
	return m_dwElapsedTime;
}

bool RTimer::IsStopped() const
{
	return m_bTimerStopped;
}

// void RTimer::SyncToApplicationElapsedTime( float fDelta )
// {
// 	if ( m_dwStopTime == 0 )
// 		m_dwElapsedTime = fDelta * 1000;
// }
// 
// void RTimer::SyncToApplicationElapsedTime( DWORD dwDelta )
// {
// 	if ( m_dwStopTime == 0 )
// 		m_dwElapsedTime = dwDelta;
// }

//////////////////////////////////////////////////////////////////////////
// TimeChecker
void RTimeChecker::Start()
{
	m_nLastTime = timeGetTime();
}

unsigned int RTimeChecker::Check()
{
	return (timeGetTime() - m_nLastTime);
}

void RTimeChecker100us::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_i64LastTime);
}

unsigned int RTimeChecker100us::Check()
{
	LARGE_INTEGER ilFreq; 
	QueryPerformanceFrequency(&ilFreq);

	LARGE_INTEGER ilCurrentTime;
	QueryPerformanceCounter(&ilCurrentTime);
	return (unsigned int)((ilCurrentTime.QuadPart-m_i64LastTime)/(ilFreq.QuadPart/10000));
}

}
