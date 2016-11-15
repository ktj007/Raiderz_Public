#pragma once

#include "MTime.h"
#include "GAsyncTask.h"

class GEntityPlayer;
class GPlayerObject;
class GPlayerLogoutDBExecuter;

class GAsyncTask_MoveGameServer: public GAsyncTask, public MTestMemPool<GAsyncTask_MoveGameServer>
{
public:
	enum MoveTaskState
	{
		MTS_INITIAL = 0,
		MTS_LOGOUT_DB_EXECUTE,
		MTS_PACK_AND_ROUTE_PLAYDATA,
		MTS_WAIT_RESPONSE_COMMAND
	};

public:
	GAsyncTask_MoveGameServer(MUID uid, uint32 nNowTime);
	virtual ~GAsyncTask_MoveGameServer();
	
	virtual void				OnUpdate() override;
	virtual GAsyncTaskID		GetID() { return ASYNC_TASK_GATE_TO_SHAREDFIELD; }

	void						Start(MUID uidPlayer, int nTargetGameServerID);

	MoveTaskState				GetState() { return m_stateMoveTask; }
	void						SetState(MoveTaskState stateMoveTask) { m_stateMoveTask = stateMoveTask; }

private:
	MoveTaskState				m_stateMoveTask;
	MUID						m_uidPlayer;
	int							m_nTargetGameServerID;
	uint32						m_nTimeoutStartTime;

	GPlayerLogoutDBExecuter*	m_pPlayerLogoutDBExecuter;

	void						OnTaskState_LOGOUT_DB_EXECUTE();
	void						OnTaskState_PACK_AND_ROUTE_PLAYDATA(GEntityPlayer* pEntityPlayer);
	void						OnTaskState_WAIT_RESPONSE_COMMAND(GPlayerObject* pPlayerObject);

	bool						CheckTimeout();

};
