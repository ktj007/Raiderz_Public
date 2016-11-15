#include "stdafx.h"
#include "GEnvManager.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GCommand.h"

GEnvManager::GEnvManager() 
: m_fElapsed(99999.0f)
, m_nLocalSecPerGametimeHour(120)
, m_fNextCheckTime(10.0f)
, m_nGameTimeUpdateTickSec(60)
, m_bChagnedTime(false)
, m_bChagnedTimeHour(false)
, m_nOldTime(TIME_INVALID)
, m_nCurrentTime(TIME_INVALID)
, m_nOldTimeHour(0)
, m_nCurrentTimeHour(0)
, m_nBaseHour(0)
{
	for (int i = 0; i < MAX_ENV_HOUR; i++)
	{
		m_nGameTimes[i] = TIME_DAYTIME;
	}
}

GEnvManager::~GEnvManager()
{

}

bool GEnvManager::Create(GEnvParam& envParam)
{
	int nGameTimeStartHours[MAX_TIME_TYPE] = {4, 7, 18, 21, 0, 0};

	for (int t = TIME_DAWN; t <= TIME_NIGHT; t++)
	{
		nGameTimeStartHours[t] = envParam.nStartHour[t];
	}

	for (int time = 0; time < MAX_ENV_HOUR; time++)
	{
		if (time < nGameTimeStartHours[TIME_DAWN]) m_nGameTimes[time] = TIME_NIGHT;
		else if (time < nGameTimeStartHours[TIME_DAYTIME]) m_nGameTimes[time] = TIME_DAWN;
		else if (time < nGameTimeStartHours[TIME_SUNSET]) m_nGameTimes[time] = TIME_DAYTIME;
		else if (time < nGameTimeStartHours[TIME_NIGHT]) m_nGameTimes[time] = TIME_SUNSET;
		else m_nGameTimes[time] = TIME_NIGHT;
	}

	m_nLocalSecPerGametimeHour			= envParam.nLocalTimePerGameTimeOneHour;
	m_nGameTimeUpdateTickSec			= envParam.nUpdateTickSec;

	if (m_nGameTimeUpdateTickSec <= 0) m_nGameTimeUpdateTickSec = 60;
	if (m_nLocalSecPerGametimeHour <= 0) m_nLocalSecPerGametimeHour = 120;

	int nCurrentTimeHour = CheckGameTimeHour();
	SetTime( CheckGameTime(nCurrentTimeHour) );
	SetTimeHour(nCurrentTimeHour);

	return true;
}

void GEnvManager::Update(float fDelta)
{
	m_fElapsed += fDelta;

	if (m_fElapsed < m_fNextCheckTime) return;		// 1분마다 체크

	m_fElapsed = 0.0f;

	int nCurrentTimeHour = CheckGameTimeHour();
	GAME_TIME_TYPE nCheckTime = CheckGameTime(nCurrentTimeHour);

	if (nCheckTime != m_nCurrentTime)
	{
		SetTime(nCheckTime);
	}

	if (nCurrentTimeHour != m_nCurrentTimeHour)
	{
		SetTimeHour(nCurrentTimeHour);
	}
}

bool GEnvManager::IsChangedTime()
{
	if (m_bChagnedTime)
	{
		m_bChagnedTime = false;
		return true;
	}

	return false;
}

bool GEnvManager::IsChangedTimeHour()
{
	if (m_bChagnedTimeHour)
	{
		m_bChagnedTimeHour = false;
		return true;
	}

	return false;
}

int GEnvManager::CheckGameTimeHour()
{
	// 0:00 부터 현재까지의 초
	struct tm* pTM = GetLocalTime();
	int nTotalSec = (pTM->tm_hour * 60 * 60) + (pTM->tm_min * 60) + pTM->tm_sec;

	int nGameTimeSecPerOneDay = m_nLocalSecPerGametimeHour * MAX_ENV_HOUR;

	int nGameTimeHour = m_nBaseHour + ((nTotalSec % nGameTimeSecPerOneDay) / m_nLocalSecPerGametimeHour);

	if (nGameTimeHour >= MAX_ENV_HOUR)
	{
		nGameTimeHour = nGameTimeHour % MAX_ENV_HOUR;
	}

	return nGameTimeHour;
}

GAME_TIME_TYPE GEnvManager::CheckGameTime(int nHour)
{
	m_fNextCheckTime = (float)m_nGameTimeUpdateTickSec;

	// 게임 내 시간
	if (nHour >= 0 && nHour < MAX_ENV_HOUR)
	{
		return m_nGameTimes[nHour];
	}

	return TIME_DAYTIME;
}

void GEnvManager::RouteGameTime(GAME_TIME_TYPE nGameTime)
{
	int8 nCharGameTime = (int8)nGameTime;

	// 메세지 전송
	MCommand* pNewCommand = MakeNewCommand(MC_FIELD_SET_TIME, 0, 1, NEW_CHAR(nCharGameTime));
	gsys.pCommandCenter->RouteToAll(pNewCommand);
}

void GEnvManager::SetTime( GAME_TIME_TYPE nGameTime )
{
	m_nOldTime = m_nCurrentTime;
	m_nCurrentTime = nGameTime;
	m_bChagnedTime = true;

	// 유저들에게 메세지를 보내준다.
	RouteGameTime(m_nCurrentTime);
}

void GEnvManager::SetTimeHour( int nHour )
{
	m_nOldTimeHour = m_nCurrentTimeHour;
	m_nCurrentTimeHour = nHour;
	m_bChagnedTimeHour = true;
}

tm* GEnvManager::GetLocalTime()
{
	return gsys.pSystem->GetLocalTime();
}

void GEnvManager::ChangeTimeForced( GAME_TIME_TYPE nGameTime )
{
	int nHour = 0;
	for (int i = 0; i < MAX_ENV_HOUR; i++)
	{
		if (m_nGameTimes[i] == nGameTime)
		{
			nHour = i;
			break;
		}
	}
	m_nBaseHour = 0;
	int nCurrentTime = CheckGameTimeHour();
	
	if (nHour >= nCurrentTime)
	{
		m_nBaseHour = nHour - nCurrentTime;
	}
	else
	{
		m_nBaseHour = nHour + MAX_ENV_HOUR - nCurrentTime;
	}

	SetTime(nGameTime);
}