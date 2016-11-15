#ifndef _XMYACTION_CHANGE_CHANNEL_H
#define _XMYACTION_CHANGE_CHANNEL_H

#include "XMyActionState.h"
#include "MTime.h"

class XMyActionChangeChannel: public XMyActionState
{
private:
	MRegulator		m_Timer;

public:
	XMyActionChangeChannel( XModuleMyControl* pOwner ) : XMyActionState(pOwner) {}
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_CHANGE_CHANNEL; }
	virtual	bool			IsPostMovement()	{ return false; }
};

#endif // _XMYACTION_CHANGE_CHANNEL_H