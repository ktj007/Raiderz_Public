
#pragma once

#define TIMEVAL				double
#define MAX_FREQUENCY		10


enum
{
	TIMEUNIT_MICROSEC,					///< Convert to micro-second
	TIMEUNIT_MILLISEC,					///< Convert to milli-second
	TIMEUNIT_SEC,						///< Convert to second
	TIMEUNIT_MIN						///< Convert to minute
};




class RSoundTimer
{
protected:
	double				m_fFrequency[ MAX_FREQUENCY];
	LARGE_INTEGER		m_QueryTime;
	TIMEVAL				m_fCurTime;
	TIMEVAL				m_fElapsed;
	bool				m_bPaused;
	double				m_fSpeed;


public:
	RSoundTimer();

	double Update();

	TIMEVAL GetCurTime( int nTimeUnit =TIMEUNIT_MILLISEC) const;
	TIMEVAL GetElapsedTime( int nTimeUnit =TIMEUNIT_MILLISEC) const;

	void Reset();

	void SetPause( bool bPause);
	bool GetPause() const;

	void SetSpeed( double fSpeed);
	double GetSpeed() const;
};
