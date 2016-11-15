#ifndef _XMYACTION_DYE_H
#define _XMYACTION_DYE_H

#include "XMyActionState.h"
#include "XTimer.h"
#include "XCommonData.h"

class XMyActionDye : public XMyActionState
{
private:
	MRegulator				m_Regulator;

	DYE_DATA				m_DyeData;

	bool					m_bActionDone;		///< 이 플래그가 true이어야 이 ActionState를 벗어날 수 있다.
	bool					m_bPostDone;

	void					Init();
	void					DyeCancel();
	void					DyeEnd();

public:
	XMyActionDye( XModuleMyControl* pOwner );
	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual void			Exit();;
	virtual XEventResult	Event(XEvent& msg);

	int						GetID() { return ACTION_STATE_DYE; }
	virtual	bool			IsPostMovement()	{ return false; }
};

#endif // _XMYACTION_HOLD_H