#include "stdafx.h"
#include "LTimeoutWatcher.h"
#include "LTimeoutHandler.h"
#include "LGlobal.h"
#include "LSystem.h"

LTimeoutWatcher::LTimeoutWatcher()
: m_nTimeoutPeriod(0)
, m_nTimeOverflowBalance(UINT_MAX)
{
	m_itPlayerTimeout = m_mapPlayerTimeout.end();
}

LTimeoutWatcher::~LTimeoutWatcher()
{
	// do nothing
}

void LTimeoutWatcher::AddPlayer(MUID uidPlayer, LTimeoutHandler* pHandler)
{	
	uint32 nTime = gsys.pSystem->GetNowTime();

	TimeoutInfo timeoutInfo;
	timeoutInfo.m_nTime = nTime + m_nTimeoutPeriod;
	timeoutInfo.m_pHandler = pHandler;

	m_mapPlayerTimeout.insert(make_pair(uidPlayer, timeoutInfo));
}

void LTimeoutWatcher::RemovePlayer(MUID uidPlayer)
{
	WatchMap::iterator it = m_mapPlayerTimeout.find(uidPlayer);

	if (it == m_mapPlayerTimeout.end())
	{
		return;
	}

	if (it == m_itPlayerTimeout)
	{
		m_itPlayerTimeout = m_mapPlayerTimeout.erase(it);
	}
	else
	{
		m_mapPlayerTimeout.erase(it);
	}
}

void LTimeoutWatcher::UpdateTimeout(void)
{
	// 없으면 검사하지 않는다.
	if (m_mapPlayerTimeout.empty())
	{
		return;
	}


	// 검사 다했으면, 처음부터 다시한다.
	if (m_itPlayerTimeout == m_mapPlayerTimeout.end())
	{
		m_itPlayerTimeout = m_mapPlayerTimeout.begin();
	}


	// 한번에 한 플레이어씩 검사한다.
	TimeoutInfo timeoutInfo = m_itPlayerTimeout->second;	
	if (IsTimeout(timeoutInfo.m_nTime))
	{	
		MUID uidPlayer = m_itPlayerTimeout->first;

		// 삭제 후, 접속종료. LServer에서 RemovePlayer()가 호출되어 m_itPlayerTimeOut가 변경될 부작용 방지.
		m_itPlayerTimeout = m_mapPlayerTimeout.erase(m_itPlayerTimeout);
		
		if (timeoutInfo.m_pHandler != NULL)
		{
			timeoutInfo.m_pHandler->OnTimeout(uidPlayer);			
		}		
	}
	else
	{
		m_itPlayerTimeout++;
	}
}

bool LTimeoutWatcher::IsTimeout(uint32 nTime)
{
	uint32 nCurrentTime = gsys.pSystem->GetNowTime();			

	// timeGetTime 오버플로우 영역에선, 오버플로우를 고려하여 계산한다.	
	if (m_nTimeOverflowBalance < nCurrentTime)
	{
		uint32 nOverflowGuardTime = nCurrentTime + m_nTimeoutPeriod;
		return nOverflowGuardTime < nTime && nTime <= nCurrentTime;
	}
	else
	{
		return nTime <= nCurrentTime;
	}
}

void LTimeoutWatcher::SetTimeoutPeriod(uint32 nMilliSeconds)
{
	m_nTimeoutPeriod = nMilliSeconds;
	m_nTimeOverflowBalance = UINT_MAX - nMilliSeconds;
}
