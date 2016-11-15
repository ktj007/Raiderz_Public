#pragma once

#include "XNetActionState.h"
#include "XModuleActorControl.h"
#include "XGrappledController.h"



class XNetActionGrappled : public XNetActionState
{
private:
	XGrappledController		m_Grappled;


public:
	XNetActionGrappled(XModuleNetControl* pOwner);
	virtual ~XNetActionGrappled();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);
	virtual void PostUpdate(float fDelta);
	virtual bool CheckEnterable( void* pParam);
	virtual bool CheckExitable( int nNextStateID);


	virtual int GetID()					{ return NET_ACTION_STATE_GRAPPLED;	}
	const wchar_t* GetDebugName()			{ return L"grappled";				}
};


