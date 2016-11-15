#pragma once

#include "MTime.h"

class GNPCDebugger;

class GScriptTimer : public MTestMemPool<GScriptTimer>
{
private:
	friend GNPCDebugger;
	typedef map<int,MRegulator>		MAP_REGULATORS;
private:
	MAP_REGULATORS	m_mapRegulators;
	set<int>		m_setRepeatedIDs;
	MRegulator		m_rgrTick;
	vector<int>		m_vecUpdatedList;
public:
	GScriptTimer();

	bool HasTimer() const;

	bool SetTimer(int nTimerID, float fIntervalTime, bool bRepeat=false);
	bool KillTimer(int nTimerID);
	void ResetTimer(int nTimerID);
	
	bool IsSet(int nTimerID);
	bool IsRepeatable(int nTimerID);

	const vector<int>& GetUpdatedTimers() const;

	bool Update( float fDelta);
	void Clear();
	
};
