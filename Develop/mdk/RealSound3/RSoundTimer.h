#ifndef _RSOUNDTIMER
#define _RSOUNDTIMER


// Value of time
#define TIMEVAL		double


// Time unit
enum
{
	TIMEUNIT_MICROSEC,					///< Convert to micro-second
	TIMEUNIT_MILLISEC,					///< Convert to milli-second
	TIMEUNIT_SEC,						///< Convert to second
	TIMEUNIT_MIN						///< Convert to minute
};





/// class : RSoundTimer
class RSoundTimer
{
/// Member variables
protected:
	bool				m_bCreate;
	double				m_fFrequency;
	LARGE_INTEGER		m_tCounter;
	TIMEVAL				m_fCurTime;
	TIMEVAL				m_fElapsed;


/// Member functions
public:
	RSoundTimer();
	virtual ~RSoundTimer();

	// Get singleton instance
	static RSoundTimer* GetInstance();

	// Update
	void Update();

	// Get time
	TIMEVAL GetCurTime( int nTimeUnit =TIMEUNIT_MILLISEC) const;
	TIMEVAL GetElapsedTime( int nTimeUnit =TIMEUNIT_MILLISEC) const;
};





// inline : Update timer
inline void UpdateTimer()
{
	RSoundTimer::GetInstance()->Update();
}


// inline : Get current time
inline TIMEVAL GetCurTime( int nTimeUnit =TIMEUNIT_MILLISEC)
{
	return RSoundTimer::GetInstance()->GetCurTime( nTimeUnit);
}


// inline : Get elapsed time
inline TIMEVAL GetElapsedTime( int nTimeUnit =TIMEUNIT_MILLISEC)
{
	return RSoundTimer::GetInstance()->GetElapsedTime( nTimeUnit);
}

#endif
