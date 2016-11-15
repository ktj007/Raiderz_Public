#pragma once

#include "XMyActionState.h"
#include "XTimer.h"

class XMyActionSit : public XMyActionState
{
private:
	void ActionSitDown();
	void ActionSitRise();
	bool m_bStandUpKeyDown;	///< 일어서는 키(또는 방향키)를 명시적으로 눌렀다.
	bool m_bSitDown;
	bool m_bPvPFreeView;

	bool CheckAnimationRise();
	bool CheckVirtualKeyForPvPFreeView(XVirtualKey nVirtualKey);

	void SetPvPFreeViewCamera();
	void ClearPvPFreeViewCamera();

public:
	XMyActionSit( XModuleMyControl* pOwner ) : XMyActionState(pOwner), m_bStandUpKeyDown(false), m_bPvPFreeView(false)	{}
	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_SIT; }
	virtual	bool			IsPostMovement()	{ return false; }
};

