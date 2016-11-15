#ifndef _XMYACTION_HOLD_H
#define _XMYACTION_HOLD_H

#include "XMyActionState.h"
#include "XTimer.h"

class XMyActionHold : public XMyActionState
{
public:
	XMyActionHold( XModuleMyControl* pOwner ) : XMyActionState(pOwner)	{}
	virtual bool			CheckEnterable(void* pParam)	{ return true; }
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual void			Exit();;
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_HOLD; }
	virtual	bool			IsPostMovement()	{ return false; }
};

#endif // _XMYACTION_HOLD_H