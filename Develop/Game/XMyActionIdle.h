#ifndef _XMYACTION_IDLE_H
#define _XMYACTION_IDLE_H

#include "XMyActionState.h"
#include "XTimer.h"
#include "XKeyTransition.h"
#include "XCursorSmartReleaser.h"
#include "XMyActionDieTransChecker.h"

class XInteraction;
class XMouseInteractor;

struct XMyActionIdleParam
{
	bool bSetIdleAnimation;
	XMyActionIdleParam() : bSetIdleAnimation(true) {}
};

///@brief Idle Action State
class XMyActionIdle : public XMyActionState
{
private:
	XRegulator				m_TargetNPCRegulator;
	XKeyConditionChecker	m_KeyConditionChecker;
	XCursorSmartReleaser	m_CursorSmartReleaser;

	XInteraction*			m_pInteraction;
	XMyActionDieTransChecker m_ActionDieTransChecker;

	bool					m_bGuardKeyReleased;

	void					UpdateUI( float fDelta );

	bool					CheckGuardKey();
	bool					CheckKeyCondition(XVirtualKey nVirtualKey, unsigned int nKeyState);
	bool					CheckAutoStanceChanging();
	bool					CheckEnabledToBattleStance();

public:
	XMyActionIdle(XModuleMyControl* pOwner);
	virtual ~XMyActionIdle();

	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);


	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);

	virtual bool			OnSpecialAction(SPECIAL_ACTION_COMMAND nSpecialActionCommand);
	virtual int				GetID() { return ACTION_STATE_IDLE; }
};


#endif // _XMYACTION_IDLE_H