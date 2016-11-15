#ifndef _XMYACTION_STATE_H
#define _XMYACTION_STATE_H

#include "XState.h"
#include "XVirtualKeyDef.h"

enum ACTION_STATE
{
	ACTION_STATE_NONE				= -1,
	ACTION_STATE_IDLE				= 0,
//	ACTION_STATE_RUN,
	ACTION_STATE_JUMP,
	ACTION_STATE_FALLING,
	ACTION_STATE_CHANGE_STANCE,
	ACTION_STATE_DAMAGED,
	ACTION_STATE_TACKLE,
	ACTION_STATE_GUARD,
	ACTION_STATE_NORMAL_ATTACK,
	ACTION_STATE_SPECIAL_ATTACK,
	ACTION_STATE_USE_TALENT,
	ACTION_STATE_USE_ARCHERY,
	ACTION_STATE_USE_GUN,

	MAX_ACTION_STATE
};

struct XActionTransitionCondition
{
	ACTION_STATE	nCurrentState;		///< 현재 상태
	XVirtualKey		nKey;				///< 키
	unsigned int	nState;				///< 키 조합 상태(방향키)

	ACTION_STATE	nNextState;
	unsigned int	nTime;
	int				nCombo;

	XActionTransitionCondition*	next;
};


class XModuleAction;

class XMyActionState : public XState<XModuleAction>
{
protected:

public:
	XMyActionState(XModuleAction* pOwner) : XState<XModuleAction>(pOwner) {}
	virtual void			Enter();
	virtual void			Update(float fDelta) {}
	virtual void			Exit();
	virtual XMessageResult	Message(XMessage& msg) { return MR_FALSE; }
	int						GetID() { return ACTION_STATE_NONE; }
};

// Idle Action State
class XMyActionIdle : public XMyActionState
{
public:
	XMyActionIdle(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter();
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_IDLE; }
};

// Run Action State
//class XMyActionRun : public XMyActionState
//{
//public:
//	XMyActionRun(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
//	virtual void			Enter()	{}
//	virtual void			Update(float fDelta)	{}
//	virtual void			Exit()	{}
//	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
//	int						GetID() { return ACTION_STATE_RUN; }
//};

// Jump Action State
class XMyActionJump : public XMyActionState
{
public:
	XMyActionJump(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter();
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg);
	int						GetID() { return ACTION_STATE_JUMP; }
};

// Falling Action State
class XMyActionFalling : public XMyActionState
{
public:
	XMyActionFalling(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_FALLING; }
};

// ChangeState Action State
class XMyActionChangeState : public XMyActionState
{
public:
	XMyActionChangeState(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter();
	virtual void			Update(float fDelta)	{}
	virtual void			Exit();
	virtual XMessageResult	Message(XMessage& msg);
	int						GetID() { return ACTION_STATE_CHANGE_STANCE; }
};

// Damaged Action State
class XMyActionDamaged : public XMyActionState
{
public:
	XMyActionDamaged(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_DAMAGED; }
};

// Tackle Action State
class XMyActionTackle : public XMyActionState
{
public:
	XMyActionTackle(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_TACKLE; }
};

// Guard Action State
class XMyActionGuard : public XMyActionState
{
public:
	XMyActionGuard(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_GUARD; }
};

// NormalAttack Action State
class XMyActionNormalAttack : public XMyActionState
{
public:
	XMyActionNormalAttack(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_NORMAL_ATTACK; }
};

// SpecialAttack Action State
class XMyActionSpecialAttack : public XMyActionState
{
public:
	XMyActionSpecialAttack(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_SPECIAL_ATTACK; }
};

// UseTalent Action State
class XMyActionUseTalent : public XMyActionState
{
public:
	XMyActionUseTalent(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter();
	virtual void			Update(float fDelta)	{}
	virtual void			Exit();
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_USE_TALENT; }
};

// UseArchery Action State
class XMyActionUseArchery : public XMyActionState
{
public:
	XMyActionUseArchery(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_USE_ARCHERY; }
};

// UseGun Action State
class XMyActionUseGun : public XMyActionState
{
public:
	XMyActionUseGun(XModuleAction* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter()	{}
	virtual void			Update(float fDelta)	{}
	virtual void			Exit()	{}
	virtual XMessageResult	Message(XMessage& msg)	{	return MR_FALSE;	}
	int						GetID() { return ACTION_STATE_USE_GUN; }
};

#endif // _XMYACTION_STATE_H