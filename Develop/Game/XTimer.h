#ifndef _XTIMER_H
#define _XTIMER_H

#include "MTime.h"

class XServerTimer
{
private:
	uint32	m_nGlobalTime;
	uint32	m_nRecevedTime;
public:
	XServerTimer();

	void	Synch(uint32 nServerGlobalTime);
	uint32	GetNowGlobalTime();
};


// 게임틱 관리 타이머
#define MAX_FREQUENCY		50
class XTimer
{
private:
	XServerTimer	m_ServerTimer;

	struct TIMER_QUERY
	{
		double			fFrequency[ MAX_FREQUENCY];
		LARGE_INTEGER	tStart;
		LARGE_INTEGER	tEnd;
	}	m_QueryTimer;

	struct TIMER_TICK
	{
		DWORD			dwStart;
		DWORD			dwEnd;
	}	m_TickTimer;

	double				m_fCurrTime;
	double				m_fDeltaTime;
	double				m_fSpeed;
	bool				m_bPaused;


public:
	XTimer();
	virtual ~XTimer();

	double GetTime() const						{ return m_fCurrTime;					}
	double GetDelta() const						{ return m_fDeltaTime;					}
	unsigned int GetNowTime();

	void SetSpeed( double fSpeed)				{ m_fSpeed = fSpeed;					}
	double GetSpeed() const						{ return m_fSpeed;						}
	void ResetSpeed()							{ m_fSpeed = 1.0;						}

	void SetPause( bool bPause)					{ m_bPaused = bPause;					}
	bool IsPaused() const						{ return m_bPaused;						}

	void Reset();

	double Update();

	void SynchGlobalTime( uint32 nGlobalTime)	{ m_ServerTimer.Synch( nGlobalTime);	}
	uint32 GetNowGlobalTime();
};




class XTimeChecker
{
private:
	unsigned int	m_nLastTime;
public:
	XTimeChecker()	{}
	~XTimeChecker()	{}
	void Start(unsigned int nNowTime)	{ m_nLastTime = nNowTime; }
	unsigned int Check(unsigned int nNowTime)	{ return (nNowTime - m_nLastTime); }
};

class XRegulator
{
private:
	float	m_fElapsed;
	float	m_fUpdateTick;
public:
	XRegulator() : m_fElapsed(0.0f), m_fUpdateTick(0.0f) {}
	XRegulator(float fUpdateTick) : m_fElapsed(0.0f), m_fUpdateTick(fUpdateTick) {}
	void SetTickTime(float fTime)
	{
		m_fElapsed = 0.0f;
		m_fUpdateTick = fTime;
	}
	bool IsReady(float fDelta)
	{
		m_fElapsed += fDelta;
		if (m_fElapsed >= m_fUpdateTick)
		{
			m_fElapsed = 0.0f;
			return true;
		}
		return false;
	}
};

// ulong 타임을 시/분/초로 변환해주는 클래스
class XRemainTime
{
public:
	int nHour;
	int nMin;
	int nSec;
	bool bTimeover;

	XRemainTime():nHour(0), nMin(0), nSec(0), bTimeover(false) {}

	void Set(uint32 nTime)
	{
		int32 nRemainTime = (int32)(nTime)/1000;

		bTimeover = (nRemainTime < 0);
		nRemainTime = abs(nRemainTime);

		nHour = nRemainTime/(60*60);
		nRemainTime %= (60*60);
		nMin = nRemainTime/60;
		nRemainTime %= 60;
		nSec = nRemainTime;
	}
	
};

class XTimeUtil
{
public:
	/// TimeA로부터 TimeB까지의 시간 차이 (TimeA ~ TimeB)
	static unsigned int	GetTimeDistance(unsigned long int nTimeA, unsigned long int nTimeB)
	{
		if(nTimeB >= nTimeA) return nTimeB - nTimeA;

		return nTimeB + (UINT_MAX - nTimeA + 1);
	}
};

#endif // _XTIMER_H