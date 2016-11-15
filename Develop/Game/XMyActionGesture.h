#pragma once

#include "XMyActionState.h"
#include <set>

class XMyActionGesture : public XMyActionState
{
private:
	bool					m_bCancelableGesture;

public:
	XMyActionGesture( XModuleMyControl* pOwner );
	virtual void			Enter(void* pParam=NULL);
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_GESTURE; }
	virtual	bool			IsPostMovement()	{ return false; }
	virtual bool			CheckExitable(int nNextStateID);
};

