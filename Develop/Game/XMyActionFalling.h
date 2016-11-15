#ifndef _XMYACTION_FALLING_H
#define _XMYACTION_FALLING_H

#include "XMyActionState.h"

///@brief Falling Action State
class XMyActionFalling : public XMyActionState
{
	bool	bAutoRun;
public:
	XMyActionFalling(XModuleMyControl* pOwner) : XMyActionState(pOwner)	{}
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual int				GetID() { return ACTION_STATE_FALLING; }
};



#endif // _XMYACTION_FALLING_H