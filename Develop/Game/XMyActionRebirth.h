#ifndef _XMYACTION_REBIRTH_H
#define _XMYACTION_REBIRTH_H

#include "XMyActionState.h"

class XMyActionRebirth :
	public XMyActionState
{
private:
	bool					m_bPlayDone;

public:
	XMyActionRebirth(XModuleMyControl* pOwner);
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	int						GetID() { return ACTION_STATE_REBIRTH; }
	virtual	bool			IsPostMovement()	{ return false; }

	void					RebirthEffect();
};

# endif //_XMYACTION_REBIRTH_H