#pragma once

#include "XNetActionState.h"
#include "XModuleActorControl.h"
#include "XSwallowedController.h"



class XNetActionSwallowed : public XNetActionState
{
private:
	XSwallowedController		m_Eaten;


public:
	XNetActionSwallowed(XModuleNetControl* pOwner);
	virtual ~XNetActionSwallowed();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);
	virtual void PostUpdate(float fDelta);
	virtual bool CheckEnterable( void* pParam);
	virtual bool CheckExitable( int nNextStateID);


	virtual int GetID()				{ return NET_ACTION_STATE_SWALLOWED;		}
	void		FinishEaten()		{ m_Eaten.OnFinish();}
};


