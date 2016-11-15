#include "stdafx.h"
#include "XTimer.h"
#include "XNetwork.h"


XTimer::XTimer()
{
	// Initialize query performance timer
	{
		LARGE_INTEGER _curr;
		QueryPerformanceCounter( &_curr);
		m_QueryTimer.tStart = _curr;
		m_QueryTimer.tEnd = _curr;

		LARGE_INTEGER _freq;
		QueryPerformanceFrequency( &_freq);
		double _frequency = 1000.0 / (double)_freq.QuadPart;
		for ( int i = 0;  i < MAX_FREQUENCY;  i++)		m_QueryTimer.fFrequency[ i] = _frequency;
	}


	// Initialize tick timer
	{
		DWORD _curr = timeGetTime();
		m_TickTimer.dwStart = _curr;
		m_TickTimer.dwEnd = _curr;
	}


	// Reset
	Reset();
}


XTimer::~XTimer()
{
	Reset();
}


unsigned int XTimer::GetNowTime()
{
	return XTimeUtil::GetTimeDistance( m_TickTimer.dwStart, timeGetTime());
}


void XTimer::Reset()
{
	m_fCurrTime = 0.0;
	m_fDeltaTime = 0.0;
	m_fSpeed = 1.0;
	m_bPaused = false;
}


double XTimer::Update()
{
	// Get current frequency
	LARGE_INTEGER _freq;
	QueryPerformanceFrequency( &_freq);
	double _frequency = 1000.0 / (double)_freq.QuadPart;


	// Get average frequency
	memcpy( &m_QueryTimer.fFrequency[ 0], &m_QueryTimer.fFrequency[ 1], sizeof( double) * ( MAX_FREQUENCY - 1));
	m_QueryTimer.fFrequency[ MAX_FREQUENCY - 1] = _frequency;
	double _avg_freq = 0.0;
	for ( int i = 0;  i < MAX_FREQUENCY;  i++)		_avg_freq += m_QueryTimer.fFrequency[ i];
	_avg_freq /= (double)MAX_FREQUENCY;


	// Get delta time
	LARGE_INTEGER _curr;
	QueryPerformanceCounter( &_curr);
	double _delta = max( 0.01, ( _curr.QuadPart - m_QueryTimer.tEnd.QuadPart) * _avg_freq);
	m_QueryTimer.tEnd = _curr;


	// Get delta time
	if ( m_bPaused == true)		m_fDeltaTime = 0.0;
	else						m_fDeltaTime = _delta * m_fSpeed * 0.001;


	// Get current time
	m_fCurrTime += m_fDeltaTime;

	return m_fDeltaTime;
}


uint32 XTimer::GetNowGlobalTime()
{
	return m_ServerTimer.GetNowGlobalTime();
}









XServerTimer::XServerTimer()
: m_nGlobalTime(0)
, m_nRecevedTime(0)
{
}

void XServerTimer::Synch( uint32 nServerGlobalTime )
{
	m_nRecevedTime = timeGetTime();

	m_nGlobalTime = nServerGlobalTime;
	if (global.net)
	{
		m_nGlobalTime += global.net->GetPing();
	}
}

uint32 XServerTimer::GetNowGlobalTime()
{
	uint32 nNowTime = timeGetTime();
	uint32 local_time = XTimeUtil::GetTimeDistance(m_nRecevedTime, nNowTime);
	return (m_nGlobalTime+local_time);
}

