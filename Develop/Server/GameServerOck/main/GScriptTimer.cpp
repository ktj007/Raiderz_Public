#include "stdafx.h"
#include "GScriptTimer.h"
#include "GConst.h"

GScriptTimer::GScriptTimer()
{
	m_rgrTick.SetTickTime(1.0f);
	m_rgrTick.Start();
}

bool GScriptTimer::SetTimer( int nTimerID, float fIntervalTime, bool bRepeat/*=false*/ )
{
	if (IsSet(nTimerID))	return false;

	m_mapRegulators.insert(MAP_REGULATORS::value_type(nTimerID, MRegulator(fIntervalTime)));

	if (bRepeat)
		m_setRepeatedIDs.insert(nTimerID);

	return true;
}

void GScriptTimer::ResetTimer( int nTimerID )
{
	if (!IsSet(nTimerID))	return;

	m_mapRegulators[nTimerID].Reset();
}

bool GScriptTimer::KillTimer( int nTimerID )
{
	if (!IsSet(nTimerID))	return false;

	m_mapRegulators.erase(nTimerID);
	m_setRepeatedIDs.erase(nTimerID);

	return true;
}

bool GScriptTimer::Update( float fDelta)
{
	if (!HasTimer()) 
		return false;

	float fElapsedTime = m_rgrTick.GetElapsedTime();
	if (!m_rgrTick.IsReady(fDelta))
		return false; // 주기가 되지 않음

	m_vecUpdatedList.clear(); 

	for (MAP_REGULATORS::iterator iter = m_mapRegulators.begin(); iter != m_mapRegulators.end(); ++iter)
	{
		const int nTimerID			= iter->first;
		MRegulator& Regulator		= iter->second;

		if (GConst::LOG_SCRIPT_TIMER)
		{
			mlog("%s (TimerID:%d) - %.2f / %.2f\n", __FUNCTION__, nTimerID, Regulator.GetElapsedTime(), Regulator.GetTickTime());
		}
				
		if (Regulator.IsReady(fElapsedTime+fDelta))
		{
			m_vecUpdatedList.push_back(nTimerID);
		}
	}

	for each (int nTimerID in m_vecUpdatedList)
	{
		if (!IsRepeatable(nTimerID))
		{
			KillTimer(nTimerID);
		}
	}

	return !m_vecUpdatedList.empty();
}

void GScriptTimer::Clear()
{
	m_mapRegulators.clear();
	m_setRepeatedIDs.clear();
}

bool GScriptTimer::IsSet( int nTimerID )
{
	return (m_mapRegulators.find(nTimerID) != m_mapRegulators.end());
}

bool GScriptTimer::IsRepeatable( int nTimerID )
{
	return (m_setRepeatedIDs.find(nTimerID) != m_setRepeatedIDs.end());
}

bool GScriptTimer::HasTimer() const
{
	return !m_mapRegulators.empty();
}

const vector<int>& GScriptTimer::GetUpdatedTimers() const
{
	return m_vecUpdatedList;
}