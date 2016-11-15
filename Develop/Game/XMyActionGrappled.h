#pragma once

#include "XMyActionState.h"
#include "XModuleActorControl.h"
#include "XGrappledController.h"



class XMyActionGrappled : public XMyActionState
{
private:
	XGrappledController		m_Grappled;


public:
	XMyActionGrappled(XModuleMyControl* pOwner);
	virtual ~XMyActionGrappled();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	virtual XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);
	virtual void PostUpdate(float fDelta);
	virtual bool CheckEnterable( void* pParam);
	virtual bool CheckExitable( int nNextStateID);
	virtual	bool IsPostMovement()	{ return true; }
	virtual int GetID()				{ return ACTION_STATE_GRAPPLED;		}
	void		FinishGrappled()	{ m_Grappled.OnFinish();}

protected:
	void _SetCamera();
	void _ReleaseCamera();
};


