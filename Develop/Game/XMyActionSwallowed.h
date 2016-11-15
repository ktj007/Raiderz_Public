#pragma once

#include "XMyActionState.h"
#include "XModuleActorControl.h"
#include "XSwallowedController.h"



class XMyActionSwallowed : public XMyActionState
{
private:
	XSwallowedController		m_Eaten;


public:
	XMyActionSwallowed(XModuleMyControl* pOwner);
	virtual ~XMyActionSwallowed();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);
	virtual void PostUpdate(float fDelta);
	virtual bool CheckEnterable( void* pParam);
	virtual bool CheckExitable( int nNextStateID);
	virtual	bool IsPostMovement()	{ return true; }
	virtual int GetID()				{ return ACTION_STATE_SWALLOWED;		}
	void		FinishEaten()		{ m_Eaten.OnFinish();}

protected:
	void _SetCamera();
	void _ReleaseCamera();
};


