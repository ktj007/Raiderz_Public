#include "stdafx.h"
#include "RSoundTimer.h"



// RSoundTimer
RSoundTimer::RSoundTimer()
{
	m_bCreate = false;
	m_fFrequency = 0.0;
	m_fCurTime = 0.0;
	m_fElapsed = 0.0;
}


// ~RSoundTimer
RSoundTimer::~RSoundTimer()
{
	m_bCreate = false;
	m_fFrequency = 0.0;
	m_fCurTime = 0.0;
	m_fElapsed = 0.0;
}


// GetInstance
RSoundTimer* RSoundTimer::GetInstance()
{
	static RSoundTimer instance;
	return &instance;
}


// Update
void RSoundTimer::Update()
{
	if ( m_bCreate == false)
	{
		LARGE_INTEGER _frequency;
		QueryPerformanceFrequency( &_frequency);
		m_fFrequency = 1000.0 / (double)_frequency.QuadPart;

		QueryPerformanceCounter( &m_tCounter);

		m_fCurTime = m_tCounter.QuadPart * m_fFrequency;
		m_fElapsed = 0.0;

		m_bCreate = true;
	}

	else
	{
		LARGE_INTEGER _current;
		QueryPerformanceCounter( &_current);

		m_fCurTime = _current.QuadPart * m_fFrequency;
		m_fElapsed = ( _current.QuadPart - m_tCounter.QuadPart) * m_fFrequency;
		m_tCounter = _current;
	}
}


// GetCurTime
TIMEVAL RSoundTimer::GetCurTime( int nTimeUnit) const
{
	TIMEVAL _time;

	switch ( nTimeUnit)
	{
	case TIMEUNIT_MICROSEC :
		_time = m_fCurTime * 1000.0;
		break;

	case TIMEUNIT_MILLISEC :
		_time = m_fCurTime;
		break;

	case TIMEUNIT_SEC :
		_time = m_fCurTime * 0.001;
		break;

	case TIMEUNIT_MIN :
		_time = m_fCurTime * 0.00001666;
		break;
	}

	return _time;
}


// GetElapsedTime
TIMEVAL RSoundTimer::GetElapsedTime( int nTimeUnit) const
{
	TIMEVAL _time;

	switch ( nTimeUnit)
	{
	case TIMEUNIT_MICROSEC :
		_time = m_fElapsed * 1000.0;
		break;

	case TIMEUNIT_MILLISEC :
		_time = m_fElapsed;
		break;

	case TIMEUNIT_SEC :
		_time = m_fElapsed * 0.001;
		break;

	case TIMEUNIT_MIN :
		_time = m_fElapsed * 0.00001666;
		break;
	}

	return _time;
}
