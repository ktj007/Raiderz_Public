#include "stdafx.h"
#include "GWeatherMgr.h"
#include "GFieldInfo.h"
#include "GGlobal.h"
//gmgr.pEnvManager->GetWeatherTickSec()
static const int MAX_WEATHER_TRANS = 2;

// 날씨 전이표는 특별히 바뀔 일이 없기 때문에 하드코딩한다. - birdkr
// 현재날씨, 다음날씨, 최대 전이 개수
int g_weather_trans_table[MAX_NORMAL_WEATHER_TYPE][MAX_NORMAL_WEATHER_TYPE][MAX_WEATHER_TRANS] = {
	
	{	// 현재 : W_SUN
		{W_SUN, -1}, 
		{W_CLD, -1}, 
		{W_CLD, W_RNY}, 
		{W_CLD, W_HRN}, 
		{W_CLD, W_SNW}, 
		{W_CLD, W_HSW} 
	},

	{	// 현재 : W_CLD
		{W_SUN, -1}, 
		{W_CLD, -1}, 
		{W_RNY, -1}, 
		{W_RNY, W_HRN}, 
		{W_SNW, -1}, 
		{W_SNW, W_HSW}
	},

	{	// 현재 : W_RNY
		{W_SUN, -1}, 
		{W_CLD, -1}, 
		{W_RNY, -1}, 
		{W_HRN, -1}, 
		{W_CLD, -1}, 
		{W_CLD, -1}
	},

	{	// 현재 : W_HRN
		{W_RNY, W_SUN}, 
		{W_RNY, W_CLD}, 
		{W_RNY, -1}, 
		{W_HRN, -1}, 
		{W_RNY, W_CLD}, 
		{W_RNY, W_CLD}  
	},

	{	// 현재 : W_SNW
		{W_SUN, -1}, 
		{W_CLD, -1}, 
		{W_CLD, -1}, 
		{W_CLD, -1}, 
		{W_SNW, -1}, 
		{W_HSW, -1}  
	},

	{	// 현재 : W_HSW
		{W_SNW, W_HSW}, 
		{W_SNW, W_CLD}, 
		{W_SNW, W_CLD}, 
		{W_SNW, W_CLD}, 
		{W_SNW, W_CLD}, 
		{W_HSW, -1}
	}
};

/// 전이
void GWeatherMgr::TransWeather(GWeatherQueue& _queue, GAME_WEATHER_TYPE nCurrentWeather, GAME_WEATHER_TYPE nNextWeather)
{
	if (m_nCurrentWeather <= WEATHER_INVALID || MAX_NORMAL_WEATHER_TYPE <= m_nCurrentWeather)
	{
		mlog("Failed TransWeather(Field=%d, CurrentWeater=%d, NextWeather=%d).\n", m_pFieldInfo->m_nFieldID, nCurrentWeather, nNextWeather);
		return;
	}

	if (nNextWeather <= WEATHER_INVALID || MAX_NORMAL_WEATHER_TYPE <= nNextWeather)	
	{
		mlog("Failed TransWeather(Field=%d, CurrentWeater=%d, NextWeather=%d).\n", m_pFieldInfo->m_nFieldID, nCurrentWeather, nNextWeather);			
		return;
	}

	for (int i = 0; i < MAX_WEATHER_TRANS; i++)
	{
		if (g_weather_trans_table[nCurrentWeather][nNextWeather][i] != -1)
		{
			GAME_WEATHER_TYPE nWeather = GAME_WEATHER_TYPE(g_weather_trans_table[nCurrentWeather][nNextWeather][i]);
			_queue.push(nWeather);
		}
	}
}

////////////////////////////////////////////
GWeatherMgr::GWeatherMgr() 
: m_fElapsedTime(0.0f)
, m_pFieldInfo(NULL)
, m_nOldWeather(WEATHER_INVALID)
, m_nCurrentWeather(DEFAULT_GAME_WEATHER)
{

}

GWeatherMgr::~GWeatherMgr()
{


}

void GWeatherMgr::Init(const GFieldInfo* pFieldInfo, int nTick)
{
	m_fTick = (float)nTick;
	m_pFieldInfo = pFieldInfo;

	if (m_pFieldInfo->IsWeatherNotChanged())
	{
		m_nCurrentWeather = m_pFieldInfo->GetRandomWeather();
	}
	else
	{
		UpdateQueue();
	}
}

bool GWeatherMgr::Update(float fDelta)
{
	if (m_pFieldInfo->IsWeatherNotChanged()) return false;

	m_fElapsedTime += fDelta;

	// 아직 틱 시간이 안됨
	if (gsys.pServer == NULL || m_fElapsedTime < m_fTick) return false;
	m_fElapsedTime = 0;

	return UpdateQueue();
}

bool GWeatherMgr::UpdateQueue()
{
	if (m_pFieldInfo == NULL) return false;

	GAME_WEATHER_TYPE nNextWeather = m_pFieldInfo->GetRandomWeather();

	if (true == m_Queue.empty())
	{
		TransWeather(m_Queue, m_nCurrentWeather, nNextWeather);
	}	

	// Current Weather 세팅
	GAME_WEATHER_TYPE nPopWeather = DEFAULT_GAME_WEATHER;
	if (!m_Queue.empty())
	{
		nPopWeather = m_Queue.front();
		m_Queue.pop();
	}

	if (m_nCurrentWeather == nPopWeather) return false;

	m_nCurrentWeather = nPopWeather;
	return true;
}

void GWeatherMgr::ChangeWeather(GAME_WEATHER_TYPE nWeatherType)
{
	m_nOldWeather = m_nCurrentWeather;

	if (m_nCurrentWeather == nWeatherType) return;
	
	m_nCurrentWeather = nWeatherType;
	m_fElapsedTime = 0;

	while (!m_Queue.empty())
	{
		m_Queue.pop();
	}
}

