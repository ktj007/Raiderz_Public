#pragma once
#include "XMyActionState.h"

class XTargetSticky;

class XMyActionBow :
	public XMyActionState
{
public:
	XMyActionBow(XModuleMyControl* pOwner);
	virtual ~XMyActionBow(void);

	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_SHOOT_ARROW; }
	virtual	bool			IsPostMovement()	{ return false; }

private:
	bool					m_bShooted;
	unsigned int			m_nAimingTime;
	unsigned int			m_nPreparingTime;
	XTargetSticky*			m_pTarget;

	void					doReady();
	void					doShoot();
	void					doCancel();
	bool					IsDonePreparing();
};
