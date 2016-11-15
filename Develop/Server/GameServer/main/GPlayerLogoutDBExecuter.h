#pragma once

class GEntityPlayer;
class GPlayerObject;

class GPlayerLogoutDBExecuter : public MTestMemPool<GPlayerLogoutDBExecuter>
{
public:
	enum State
	{
		FlushDBChachState = 0,
		LogoutDBTaskState,
		WaitingLogoutDBTaskState,
		FinalState
	};
public:
	GPlayerLogoutDBExecuter(GPlayerObject* pPlayerObject);
	~GPlayerLogoutDBExecuter();

	void			Update();
	bool			IsDone()	{ return (FinalState == m_state); }

	State			GetState()				{ return m_state; }
	void			SetState(State state)	{ m_state = state; }

	virtual bool	OnFlushDBChachState();
	virtual bool	OnLogoutDBTaskState();
	virtual bool	OnWaitingLogoutDBTaskState();

private:
	State			m_state;
	GPlayerObject*	m_pPlayerObject;
	GEntityPlayer*	m_pEntityPlayer;

	bool			FlushCache();

};
