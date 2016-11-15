#ifndef _XMYACTION_JUMP_H
#define _XMYACTION_JUMP_H

#include "XMyActionState.h"


///@brief Jump Action State
class XMyActionJump : public XMyActionState
{
private:
	void					OnEvent_UseTalent( XEvent &msg );

public:
	XMyActionJump(XModuleMyControl* pOwner) : XMyActionState(pOwner)	{}
	virtual void			Enter(void* pParam=NULL);
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual int				GetID() { return ACTION_STATE_JUMP; }
	virtual	bool			IsPostMovement()	{ return true; }
	virtual bool			OnSpecialAction(SPECIAL_ACTION_COMMAND nSpecialActionCommand);


private:
	bool					m_bAutoRunning;
};


#endif // _XMYACTION_HITTED_H