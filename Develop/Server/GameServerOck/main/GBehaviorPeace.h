#ifndef _GBEHAVIOR_PEACE_H
#define _GBEHAVIOR_PEACE_H

#include "MTime.h"
#include "GBehaviorState.h"
#include "GEnemyFinder.h"

class GJobRunner;

// Idle Behavior State
class GBehaviorPeace : public GBehaviorState, public MMemPool<GBehaviorPeace>
{
private:
	GJobRunner*		m_pJobRunner;

	enum PeaceActType
	{
		PAT_STAND	= 0,	///< 가만히 서있기
		PAT_ROAM,			///< 돌아다니기
	};

	PeaceActType	m_nActType;
	PeaceActType	ChoiceActType();
public:
	GBehaviorPeace(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM);
	virtual ~GBehaviorPeace();

	virtual void			OnRegisterTrainstionRule();
	virtual void			Enter();	

	void RunAct();
	virtual void			Update(float fDelta);
	virtual void			Exit(int nNextStateID);
	virtual GMessageResult	Message(GMessage& msg);
	int						GetID() { return AI_BEHAVIOR_PEACE; }
	virtual const wchar_t*		DebugString() { return L"peace"; }	
};

#endif // _GBEHAVIOR_PEACE_H
