#ifndef _MTIME_H
#define _MTIME_H

#include <crtdbg.h>
#include <mmsystem.h>
#include "MDebug.h"

/// timeGetTime등으로 나온 시간값을 처리하기 위한 클래스
class MTimeValue
{
private:
	unsigned int m_Value;
public:
	MTimeValue()	{ m_Value = 0; }
	MTimeValue(unsigned int nValue) { m_Value = nValue; }

	MTimeValue operator+() const { return MTimeValue(m_Value);	}
	MTimeValue operator+(const MTimeValue& other) const 
	{ 
		_ASSERT(m_Value + other.m_Value >= m_Value);
		_ASSERT(m_Value + other.m_Value >= other.m_Value);
		return MTimeValue(m_Value + other.m_Value);	
	}
	MTimeValue& operator+=(const MTimeValue& other)	
	{ 
		_ASSERT(m_Value + other.m_Value >= m_Value);
		_ASSERT(m_Value + other.m_Value >= other.m_Value);
		m_Value += other.m_Value;
		return *this; 
	}
	MTimeValue operator-() const { return MTimeValue(m_Value);  }
	MTimeValue operator-(const MTimeValue& other) const 
	{ 
		_ASSERT(m_Value >= other.m_Value);
		return MTimeValue(m_Value - other.m_Value);	
	}
	MTimeValue& operator-=(const MTimeValue& other)	
	{ 
		_ASSERT(m_Value >= other.m_Value);
		m_Value -= other.m_Value;
		return *this; 
	}
	MTimeValue& operator=(const MTimeValue& other)
	{ 
		m_Value = other.m_Value;
		return *this; 
	}
	MTimeValue& operator=(const unsigned int& other)
	{ 
		m_Value = other;
		return *this; 
	}
	MTimeValue& operator++()		{ (this->m_Value)+=1; return (*this); }
	const MTimeValue operator++(int)
	{ 
		const MTimeValue oldValue = *this; 
		++(*this);
		return oldValue; 
	}
	bool operator<=(const MTimeValue& other) const { return m_Value <= other.m_Value; }
	bool operator< (const MTimeValue& other) const { return m_Value < other.m_Value; }
	bool operator>=(const MTimeValue& other) const { return m_Value >= other.m_Value; }
	bool operator> (const MTimeValue& other) const { return m_Value > other.m_Value; }
	bool operator==(const MTimeValue& other) const { return m_Value == other.m_Value; }
	bool operator!=(const MTimeValue& other) const { return m_Value != other.m_Value; }

	MTimeValue Distance(const MTimeValue& laterTime)
	{
		if(laterTime.m_Value >= m_Value) return laterTime.m_Value - m_Value;
		return laterTime.m_Value + (UINT_MAX - m_Value + 1);
	}

	unsigned int GetValue() { return m_Value; }
};

class MRegulator
{
private:
	bool	m_bActive;
	float	m_fElapsed;
	float	m_fUpdateTick;
public:
	MRegulator() : m_fElapsed(0.0f), m_fUpdateTick(0.0f), m_bActive(true) {}
	MRegulator(float fUpdateTick) : m_fElapsed(0.0f), m_fUpdateTick(fUpdateTick), m_bActive(true) {}
	MRegulator& operator=(const MRegulator& other)
	{ 
		m_bActive = other.m_bActive;
		m_fElapsed = other.m_fElapsed;
		m_fUpdateTick = other.m_fUpdateTick;
		return *this; 
	}

	void	SetTickTime(float fTime) { m_fUpdateTick = fTime; }
	float	GetTickTime() { return m_fUpdateTick; }
	bool	IsReady(float fDelta)
	{
		if (!m_bActive) return false;

		m_fElapsed += fDelta;
		if (m_fElapsed >= m_fUpdateTick)
		{
			m_fElapsed = 0.0f;
			return true;
		}
		return false;
	}
	void Start()
	{
		m_bActive = true;
		m_fElapsed = 0.0f;
	}
	void Reset(bool bImmediately=false)
	{
		if (bImmediately) m_fElapsed = m_fUpdateTick;
		else m_fElapsed = 0.0f;
	}
	void Stop()
	{
		m_bActive = false;
	}
	float GetElapsedTime() const { return m_fElapsed; }
	void SetElapsedTime(float fElapsedTime) { m_fElapsed = fElapsedTime; }
	bool IsActive() const { return m_bActive; }
};

class MTimeChecker
{
private:
	unsigned int	m_nLastTimeMS;
public:
	MTimeChecker()	{}
	~MTimeChecker()	{}
	void Start(unsigned int nNowTimeMS)	{ m_nLastTimeMS = nNowTimeMS; }
	void Start()	{ m_nLastTimeMS = timeGetTime(); }
	unsigned int Check(unsigned int nNowTimeMS)	{ return (nNowTimeMS - m_nLastTimeMS); }
	unsigned int Check()	{ return (timeGetTime() - m_nLastTimeMS); }
};

class MTimeChecker2
{
public:
	MTimeChecker2() 
	{
		m_nStartTime.QuadPart = 0;
	}
	
	void Start()
	{
		QueryPerformanceFrequency(&m_nFrequency);
		QueryPerformanceCounter(&m_nStartTime);
	}

	double Check()
	{
		LARGE_INTEGER nEndTime;
		QueryPerformanceCounter(&nEndTime);
		return (((double)(nEndTime.QuadPart - m_nStartTime.QuadPart) / (double)m_nFrequency.QuadPart * 1000.0));
	}

	bool IsRunning()
	{
		return m_nStartTime.QuadPart!=0;
	}

private:
	LARGE_INTEGER m_nStartTime;
	LARGE_INTEGER m_nFrequency;
};

class MTimeCheckReporter
{
public:
	MTimeCheckReporter(const char* pszMsg)
		: m_pszMsg(pszMsg)
	{
		m_TimeChecker.Start();
	}

	~MTimeCheckReporter()
	{
		mlog("TimeMeasure: %s (elapsed: %.2f)\n", m_pszMsg, m_TimeChecker.Check());
	}

private:
	const char* m_pszMsg;
	MTimeChecker2 m_TimeChecker;
};

#endif // _MTIME_H