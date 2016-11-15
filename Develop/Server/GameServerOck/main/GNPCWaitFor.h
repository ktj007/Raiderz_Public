#pragma once

class GEntityNPC;

class GNPCWaitFor : public MTestMemPool<GNPCWaitFor>
{
public:
	GNPCWaitFor(GEntityNPC* pOwner);
	~GNPCWaitFor(void);

	// 대기 받는 경우 --------------------
	void AddWaiter(MUID uidWaiter);
	void RunNextAction();

	// 대기 하는 경우 --------------------
	void AddWaiting(MUID uidTarget);
	bool IsWaiting();
	void OnNextAction(MUID uidActor);
private:
	GEntityNPC*		m_pOwner;
	// 대기 받는 경우 --------------------
	vector<MUID>	m_vecWaiters;	// 대기자 목록
	// 대기 하는 경우 --------------------
	set<MUID>		m_setWaitings;	// 대기하고 있는 액터 목록
};
