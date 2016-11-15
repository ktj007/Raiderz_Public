#ifndef _XMYACTION_DIE_H
#define _XMYACTION_DIE_H

#include "XMyActionState.h"
#include "MTime.h"

//#define _MADDUCK3
///@brief Die Action State
class XMyActionDie : public XMyActionState
{
protected:
	float					m_fRebirthTime;							//< 자동부활시켜주는 시간
	MRegulator				m_RebirthRegulator;						//< 부활창 뜨워져 있는 시간타이머
	bool					m_bChangeState;							//< 다이애니메이션이 종료되어야 부활이 되도록

	void					RebirthWindowShow(bool bForceDieAniEnd = false);
	void					RebirthRequireWindowShow();
	void					Rebirth();
	void					CheckRebirthWindow();

public:
	XMyActionDie(XModuleMyControl* pOwner);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Exit();
	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual XEventResult	Event(XEvent& msg);
	virtual void			Update(float fDelta);
	virtual int				GetID() { return ACTION_STATE_DIE; }
	virtual	bool			IsPostMovement()	{ return false; }

	float					GetRebirthRemainTime();

#ifdef _MADDUCK3
	// test
	bool					m_bTestRebirth;
	void					Debug_AutoRebirth();	//< 테스트용 자동 부활
#endif
};


#endif // _XMYACTION_DIE_H