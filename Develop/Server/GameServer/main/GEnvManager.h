#ifndef _GENV_MANAGER_H
#define _GENV_MANAGER_H

#include "CSWorldInfo.h"

struct GEnvParam
{
	int nStartHour[MAX_TIME_TYPE];

	int nLocalTimePerGameTimeOneHour;
	int nUpdateTickSec;
	int nWeatherTickSec;
	GEnvParam()
	{
		nStartHour[TIME_DAWN]		= 4;			// 4:00am ~ 7:00am
		nStartHour[TIME_DAYTIME]	= 7;			// 7:00am ~ 6:00pm
		nStartHour[TIME_SUNSET]		= 18;			// 6:00pm ~ 9:00pm
		nStartHour[TIME_NIGHT]		= 21;			// 9:00pm ~ 4:00am
		nStartHour[TIME_CUSTOM1]	= 0;
		nStartHour[TIME_CUSTOM2]	= 0;

		nLocalTimePerGameTimeOneHour = 60;
		nUpdateTickSec = 720;
		nWeatherTickSec = 120;
	}
};

#define MAX_ENV_HOUR	24

/// 환경(시간, 날씨) 관리자
class GEnvManager : public MTestMemPool<GEnvManager>
{
protected:
	// 설정값
	GAME_TIME_TYPE		m_nGameTimes[MAX_ENV_HOUR];				///< 시간별 게임 시간

	int					m_nLocalSecPerGametimeHour;		///< 게임상에서 1시간은 실시간으로 몇초? (기본값=120초)
	int					m_nGameTimeUpdateTickSec;		///< 시간체크 틱값(초) - 60초

	float				m_fElapsed;
	float				m_fNextCheckTime;
	GAME_TIME_TYPE		m_nOldTime;
	GAME_TIME_TYPE		m_nCurrentTime;
	bool				m_bChagnedTime;
	int					m_nBaseHour;

	int					m_nOldTimeHour;
	int					m_nCurrentTimeHour;
	bool				m_bChagnedTimeHour;

protected:
	virtual tm*			GetLocalTime();

	GAME_TIME_TYPE		CheckGameTime(int nTime);
	int					CheckGameTimeHour();
	void				RouteGameTime(GAME_TIME_TYPE nGameTime);
	void				SetTimeHour(int nHour);		// 임의로 현재의 시간을 설정한다.
public:
	GEnvManager();
	virtual ~GEnvManager();
	bool				Create(GEnvParam& envParam);
	void				Update(float fDelta);		// 시간대가 변하면 true 반환
	void				SetTime(GAME_TIME_TYPE nTime);		// 임의로 현재의 시간대를 설정한다.
	void				ChangeTimeForced(GAME_TIME_TYPE nGameTime);
	bool				IsChangedTime();
	bool				IsChangedTimeHour();
	GAME_TIME_TYPE		GetOldTime()		{ return m_nOldTime; }
	GAME_TIME_TYPE		GetCurrentTime()	{ return m_nCurrentTime; }	///< 게임시간대를 반환
	
	int					GetCurrentTimeHour()	{ return m_nCurrentTimeHour; } ///< 정확한 게임시간을 반환 (0~24)
};




#endif // _GENV_MANAGER_H