#pragma once

#include "RLibTypes.h"

// DXUTTimer를 참조, QueryPerformance 대신 TimeGetTime을 사용하여 만들었음.
namespace rs3 {

class RS_API RTimer
{
protected:
	bool m_bTimerStopped;		///< 타이머가 정지했는지 여부

	DWORD m_dwLastUpdateTime;	///< 마지막 업데이트 한 시간
	DWORD m_dwElapsedTime;		///< 마지막 업데이트때의 흐른 시간 간격
	DWORD m_dwStopTime;			///< 정지한 시간
	DWORD m_dwBaseTime;			///< Start ( or reset) 한 시간

public:
	RTimer();

	void	Reset();		///< resets the timer
	void	Start();		///< starts the timer
	void	Stop();			///< stop (or pause) the timer
	void	Update();		///< ElapsedTime을 구하기 위한 업데이트
	void	UpdateCustom(DWORD dwElapsedTime);	///< ElapsedTime 지정하여 업데이트

	// get the absolute system time
	DWORD	GetAbsoluteTime() const;
	
	// get the current time
	DWORD	GetTime() const;

	// get the time that elapsed between GetElapsedTime() calls
	DWORD	GetElapsedTime() const;

	// returns true if timer stopped
	bool	IsStopped() const;

	// set/get application elapsed time
// 	void	SyncToApplicationElapsedTime(float fDelta);
// 	void	SyncToApplicationElapsedTime(DWORD dwDelta);
};

class RS_API RTimeChecker
{
private:
	unsigned int m_nLastTime;
public:
	RTimeChecker() : m_nLastTime(0) {}
	void Start();
	unsigned int Check();
};

class RS_API RTimeChecker100us
{
private:
	__int64 m_i64LastTime;
public:
	RTimeChecker100us(){}
	void Start();
	unsigned int Check();
};

}
