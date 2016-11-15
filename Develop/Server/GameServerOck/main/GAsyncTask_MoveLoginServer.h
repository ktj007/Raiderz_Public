#pragma once

#include "GAsyncTask.h"

class GPlayerObject;
class GPlayerLogoutDBExecuter;

class GAsyncTask_MoveLoginServer : public GAsyncTask, public MTestMemPool<GAsyncTask_MoveLoginServer>
{
public:
	enum MoveTaskState
	{
		MTS_INITIAL = 0,
		MTS_LOGOUT_DB_EXECUTE,
		MTS_ROUTE_REQUEST_MOVE_SERVER,
		MTS_WAIT_RESPONSE_COMMAND
	};

public:
	GAsyncTask_MoveLoginServer(MUID uid, uint32 nNowTime);
	virtual ~GAsyncTask_MoveLoginServer();

	void						Start(MUID uidPlayer);

	virtual void				OnUpdate() override;
	virtual GAsyncTaskID		GetID() { return ASYNC_TASK_MOVE_TO_LOGINSERVER; }

	MoveTaskState				GetState() { return m_stateMoveTask; }
	void						SetState(MoveTaskState stateMoveTask) { m_stateMoveTask = stateMoveTask; }

private:
	MoveTaskState				m_stateMoveTask;
	MUID						m_uidPlayer;
	uint32						m_nTimeoutStartTime;

	GPlayerLogoutDBExecuter*	m_pPlayerLogoutDBExecuter;

	void						OnTaskState_LOGOUT_DB_EXECUTE();
	void						OnTaskState_ROUTE_REQUEST_MOVE_SERVER(GPlayerObject* pPlayerObject);
	void						OnTaskState_WAIT_RESPONSE_COMMAND(GPlayerObject* pPlayerObject);

	bool						CheckTimeout();

};
