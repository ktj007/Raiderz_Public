#pragma once

class LTimeoutHandler;

class LTimeoutWatcher
{
private:
	struct TimeoutInfo
	{
		uint32 m_nTime;
		LTimeoutHandler* m_pHandler;
	};

	typedef map<MUID, TimeoutInfo> WatchMap;

public:
	LTimeoutWatcher();
	virtual ~LTimeoutWatcher();

	void AddPlayer(MUID uidPlayer, LTimeoutHandler* pHandler);
	void RemovePlayer(MUID uidPlayer);
	void UpdateTimeout(void);
	bool IsTimeout(uint32 nTime);

	void SetTimeoutPeriod(uint32 nMilliSeconds);

private:
	WatchMap m_mapPlayerTimeout;
	WatchMap::iterator m_itPlayerTimeout;
	uint32 m_nTimeoutPeriod;
	uint32 m_nTimeOverflowBalance;					// 타임아웃이 자료형 오버플로우를 일으키는 경계값
};
