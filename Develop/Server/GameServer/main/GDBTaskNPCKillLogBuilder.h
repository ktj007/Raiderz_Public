#ifndef _GDBTASK_NPC_KILL_LOG_BUILDER_H
#define _GDBTASK_NPC_KILL_LOG_BUILDER_H


#include "GNpcKillerData.h"

class GLogDBTaskQuery;

class GDBTaskNPCKillLogBuilder
{
public :
	GDBTaskNPCKillLogBuilder();
	virtual ~GDBTaskNPCKillLogBuilder();

	void SetPlayer(const size_t nIdx, const GNPC_KILLER& killer);
	void Reset();
	void PostNpcKillLog(const int nWorldID, const wstring& strRegDate, const int nNpcID, const vector<GNPC_KILLER>& vKiller);


protected :
	void Post(int nWorldID, const wstring& strRegDate, int nNpcID);

	virtual void PostTask(GLogDBTaskQuery* pTask);


protected :
	GNPC_KILLER	m_Player[GNPC_KILLER::MAX_COUNT];	
};


#endif