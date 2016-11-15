#ifndef _XMYACTION_INTERACTION_H
#define _XMYACTION_INTERACTION_H

#include "XMyActionState.h"

class XMyActionNPCInteraction : public XMyActionState
{
private:
	void					InteractionEnd();
	void					ForceInteractionEnd();

	bool					m_bEndPost;
	bool					m_bEndStart;

public:
	XMyActionNPCInteraction( XModuleMyControl* pOwner ) : XMyActionState(pOwner), m_bEndStart(false), m_bEndPost(false)	{}
	virtual ~XMyActionNPCInteraction();

	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual bool			OnSpecialAction(SPECIAL_ACTION_COMMAND nSpecialActionCommand);

	bool					IsMovable();

	int						GetID() { return ACTION_STATE_INTERACTION; }
	virtual	bool			IsPostMovement()	{ return false; }
};

#endif // _XMYACTION_HOLD_H