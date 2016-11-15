#pragma once

#include "XNetActionState.h"
#include "XModuleActorControl.h"



class XNetActionGuard : public XNetActionState
{
private:
	bool		m_bGuardDefense;

	int			m_nGuardCnt;

public:
	XNetActionGuard(XModuleNetControl* pOwner);
	virtual ~XNetActionGuard();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);
	virtual bool CheckEnterable( void* pParam);


	virtual int GetID()				{ return NET_ACTION_STATE_GUARD;		}
};


