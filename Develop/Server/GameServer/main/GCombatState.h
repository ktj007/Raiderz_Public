#ifndef _GCOMBAT_STATE_H
#define _GCOMBAT_STATE_H

#include "GState.h"
#include "GModuleAI.h"
#include "GAIState.h"

class GCombatCombat;
class GEntityNPC;

//////////////////////////////////////////////////////////////////////////////////////////
// GCombatState //////////
class GCombatState : public GState<GModuleAI>
{
private:	
	GEntityNPC*			m_pOwnerNPC;

protected:
	bool					IsExistEnemy();
	bool					IsObtainVictory();
	bool					IsJobRunning();
	
	inline GModuleAI*		GetAI()						{ return m_pOwner; }
	inline GEntityNPC*		GetNPC()					{ return m_pOwnerNPC; }

	void					SendMsg_StartCombat(MUID uidEnemy=MUID::Invalid());
	void					SendMsg_EndCombat();
	void					SendMsg_StartVictory();
	void					SendMsg_EndPrologue();

	void					Logf(const wchar_t* szText, ...);
		
public:	
	GCombatState(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);	
	virtual void			Enter() {}
	virtual void			Update(float fDelta) {}
	virtual void			Exit(int nNextStateID) {}
	virtual GMessageResult	Message(GMessage& msg) { if (__super::Message(msg)==MR_TRUE) return MR_TRUE;	 return MR_FALSE; }
	int						GetID() { return AI_COMBAT_NONE; }
	

};


class GCombatStateMachine : public GStateMachine<GModuleAI>, public MTestMemPool<GCombatStateMachine>
{
protected:
	int	m_nPrevStateID;
	virtual void OnPreChangeState(GState<GModuleAI>* pNextState) override;
	virtual void OnChangedState(GState<GModuleAI>* pNextState) override;
public:
	GCombatStateMachine(GModuleAI* pOwner) : GStateMachine<GModuleAI>(pOwner), m_nPrevStateID(-1) {}
	virtual ~GCombatStateMachine() {}
	void Reset()
	{
		m_nPrevStateID = -1;
		SetCurrentState(NULL);
	}
	int GetNextStateID()		{ return m_nPrevStateID; }
	
};


#endif // _GCOMBAT_STATE_H