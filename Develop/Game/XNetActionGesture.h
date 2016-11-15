#pragma once

#include "XNetActionState.h"

class XNetActionGesture : public XNetActionState
{
protected:

public:
	XNetActionGesture(XModuleNetControl* pOwner);
	virtual ~XNetActionGesture();

	virtual void Enter( void* pParam =NULL);
	virtual void Exit();
	XEventResult Event( XEvent& msg);
	virtual void Update( float fDelta);
	virtual bool CheckEnterable( void* pParam);
	virtual bool CheckExitable( int nNextStateID);


	virtual int GetID()					{ return NET_ACTION_STATE_GESTURE;	}
	const wchar_t* GetDebugName()		{ return L"sit";					}

};