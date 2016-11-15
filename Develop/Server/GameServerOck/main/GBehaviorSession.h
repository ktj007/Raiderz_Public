#pragma once

#include "GBehaviorState.h"

class GJob;
class GFieldNPCSession;

class GBehaviorSession : public GBehaviorState, public MMemPool<GBehaviorSession>
{
public:
	GBehaviorSession(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GBehaviorSession();

	virtual int				GetID()  override { return AI_BEHAVIOR_SESSION; }	
	virtual void			OnRegisterTrainstionRule() override;
	virtual void			Enter() override;
	virtual void			Exit(int nNextStateID) override;
	virtual void			Update(float fDelta) override;
	virtual GMessageResult Message( GMessage& msg ) override;

	GFieldNPCSession*		GetSession();

	virtual const wchar_t*		DebugString() { return L"session"; }

	GJob*					GetCurJob() const { return m_pCurJob; }
private:
	void SetJob(GJob* pJob);
	void					ClearCurJob();
	void					OnJobFinished(const GJob* pLastJob);
private:
	GJob*				m_pCurJob;
	MUID				m_uidSession;
};
