#ifndef _XNETACTION_HITTED_H
#define _XNETACTION_HITTED_H

#include "XNetActionState.h"

///@brief Hitted Action State
class XNetActionHit : public XNetActionState
{
private:
	bool					m_bDone;

	MF_STATE				m_nMFState;
	int						m_nMFWeight;

private:
	void					EndMFLockTime();

public:
	XNetActionHit(XModuleNetControl* pOwner) : XNetActionState(pOwner), m_bDone(false), m_nMFState(MF_NONE) {}
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual XEventResult	Event(XEvent& msg);
	virtual bool			CheckExitable(int nNextStateID);
	int						GetID() { return NET_ACTION_STATE_HIT; }
};




#endif // _XMYACTION_HITTED_H