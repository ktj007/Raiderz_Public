#ifndef _XMYACTION_QUEST_INTERACTION_H
#define _XMYACTION_QUEST_INTERACTION_H

#include "XMyActionState.h"
#include "MTime.h"

class XMyActionQuestInteraction: public XMyActionState
{
private:
	MRegulator				m_Regulator;

	void					StopMove();

public:
	XMyActionQuestInteraction(XModuleMyControl* pOwner);

	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID)		{ return true;}
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update( float fDelta);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	
	int						GetID() { return ACTION_STATE_QUEST_INTERACTION; }
	virtual	bool			IsPostMovement()	{ return false; }
};



#endif // _XMYACTION_QUEST_INTERACTION_H