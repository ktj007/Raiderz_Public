#ifndef _XMYACTION_CHANGE_STATE_H
#define _XMYACTION_CHANGE_STATE_H

#include "XMyActionState.h"



///@brief ChangeState Action State
class XMyActionChangeState : public XMyActionState
{
private:
	CHAR_STANCE				m_nStanceTo;
	bool					m_bQuick;

	bool					m_bStartIdle;
	bool					m_bStartRun;

public:
	XMyActionChangeState(XModuleMyControl* pOwner) : XMyActionState(pOwner), 
													m_bQuick(false), 
													m_nStanceTo(CS_NORMAL), 
													m_bStartIdle(false),
													m_bStartRun(false)	{}
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta) {}
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_CHANGE_STANCE; }
	virtual	bool			IsPostMovement()	{ return true; }


protected:
	bool					CheckExistWeapon();
	bool					CheckChangeStateAnimation(wstring strMotionName);
};



#endif // _XMYACTION_CHANGE_STATE_H