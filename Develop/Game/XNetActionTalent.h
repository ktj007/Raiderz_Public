#pragma once

#include "XNetActionState.h"
#include "XModuleActorControl.h"



class XNetActionTalent : public XNetActionState
{
private:
	float					m_fCastingTime;
	int						m_nCurrentTalentID;						// 현재 진행중인 탤런트 ID

	bool					m_bCastingEndEffectDone;

private:
	void					TalentStart(XTalentInfo* pTalentInfo);
	void					CheckCastingTime(float fDelta);

	void					CastingEndEffect();

	bool					CheckCancelableSocialTalent();

public:
	XNetActionTalent(XModuleNetControl* pOwner);
	virtual ~XNetActionTalent();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);


	virtual int GetID()				{ return NET_ACTION_STATE_TALENT;		}
};


