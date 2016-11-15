#include "stdafx.h"
#include "RSoundTimer.h"



RSoundTimer::RSoundTimer()
{
	LARGE_INTEGER _freq;
	QueryPerformanceFrequency( &_freq);
	double _frequency = 1000.0 / (double)_freq.QuadPart;
	for ( int i = 0;  i < MAX_FREQUENCY;  i++)
		m_fFrequency[ i] = _frequency;

	QueryPerformanceCounter( &m_QueryTime);

	m_fCurTime = 0.0;
	m_fElapsed = 0.0;
	m_bPaused = false;
	m_fSpeed = 1.0;
}


double RSoundTimer::Update()
{
	// Get current frequency
	LARGE_INTEGER _freq;
	QueryPerformanceFrequency( &_freq);
	double _frequency = 1000.0 / (double)_freq.QuadPart;


	// Get average frequency
	memcpy( &m_fFrequency[ 0], &m_fFrequency[ 1], sizeof( double) * ( MAX_FREQUENCY - 1));
	m_fFrequency[ MAX_FREQUENCY - 1] = _frequency;

	double _avg_freq = 0.0;
	for ( int i = 0;  i < MAX_FREQUENCY;  i++)		_avg_freq += m_fFrequency[ i];
	_avg_freq /= (double)MAX_FREQUENCY;


	// Get current time
	LARGE_INTEGER _curr;
	QueryPerformanceCounter( &_curr);


	// Set current and elapsed time
	if ( m_bPaused == true)
	{
		m_fElapsed = 0.0;
	}
	else
	{
		double _delta = ( _curr.QuadPart - m_QueryTime.QuadPart) * _avg_freq * m_fSpeed;
		m_fCurTime += _delta;
		m_fElapsed = _delta;
	}


	m_QueryTime = _curr;

	return m_fElapsed;
}


void RSoundTimer::Reset()
{
	m_fCurTime = 0.0;
	m_fElapsed = 0.0;
	m_bPaused = false;
	m_fSpeed = 1.0;
}


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


void RSoundTimer::SetPause( bool bPause)
{
	m_bPaused = bPause;
}
 

bool RSoundTimer::GetPause() const
{
	return m_bPaused;
}


void RSoundTimer::SetSpeed( double fSpeed)
{
	m_fSpeed = fSpeed;
}


double RSoundTimer::GetSpeed() const
{
	return m_fSpeed;
}
