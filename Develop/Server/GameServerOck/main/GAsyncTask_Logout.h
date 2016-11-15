#pragma once

#include "GAsyncTask.h"

class GPlayerObject;
class GEntityPlayer;
class GPlayerLogoutDBExecuter;

class GAsyncTask_Logout: public GAsyncTask, public MTestMemPool<GAsyncTask_Logout>
{
public:
	enum State
	{
		StartLogoutState,
		LogoutDBExecuteState,
		CheckMoveServerAsyncTaskState,
		PublisherLogoutTaskState,
		DestroyPlayerState,
		FinalState
	};
public:
	GAsyncTask_Logout(MUID uid, uint32 nNowTime, MUID uidPlayer);
	virtual ~GAsyncTask_Logout();
	
	virtual GAsyncTaskID GetID()	{ return ASYNC_TASK_LOGOUT;	}
	
	State GetState();
	void SetState(State state);
	bool IsLeaveGame();

	virtual bool OnStartLogout();

	virtual bool OnLogoutDBExecuteState();	
	virtual bool OnCheckMoveServerAsyncTaskState();
	virtual bool OnPublisherLogoutTask();
	virtual bool OnDestroyPlayer();

protected:
	virtual void OnUpdate() override;

	bool CheckMoveServerAsyncTask_GameServer();
	bool CheckMoveServerAsyncTask_LoginServer();

private:
	MUID m_uidPlayer;
	State m_state;
	bool m_isLeaveGame;
	bool m_isBackToLogin;
	
	GPlayerObject*				m_pPlayerObject;
	GEntityPlayer*				m_pEntityPlayer;
	GPlayerLogoutDBExecuter*	m_pPlayerLogoutDBExecuter;
};
