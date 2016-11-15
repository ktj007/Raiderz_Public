#ifndef _XMYACTION_SLEEP_H
#define _XMYACTION_SLEEP_H

#include "XMyActionState.h"

enum MAP_OBJECT
{
	MAP_OBJECT_NONE = 0,
	MAP_OBJECT_BED,
	MAP_OBJECT_END
};

class XMyActionMapObj : public XMyActionState
{
private:
	MUID					m_MapObjUID;
	MAP_OBJECT				m_nMapObject;
	bool					m_bMotionPlayOnce;

	void					MoveDummyPos(MUID MapObjUID);
	void					ActionSleep();

public:
	XMyActionMapObj(XModuleMyControl* pOwner);
	virtual ~XMyActionMapObj(void);

	virtual void			Enter(void* pParam = NULL);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual void			Update( float fDelta);
	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual	bool			IsPostMovement()	{ return false; }

	virtual int				GetID() { return ACTION_STATE_MAPOBJ_INTERACTION; }

	void					Init();
	void					BeginSleep();
	void					EndSleep();
};

#endif // _XMYACTION_SLEEP_H