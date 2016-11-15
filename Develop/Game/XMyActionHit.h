#ifndef _XMYACTION_HITTED_H
#define _XMYACTION_HITTED_H

#include "XMyActionState.h"
#include "XDeathController.h"
#include "XMyActionDieTransChecker.h"

///@brief Hitted Action State
class XMyActionHit : public XMyActionState
{
private:
	bool					m_bDone;
	bool					m_bDoneLockTime;
	bool					m_bReservedEnd;

	unsigned int			m_nEnterTime;
	MF_STATE				m_nMFState;
	int						m_nMFWeight;

	vec3					m_vPrePos;

	XDeathController		m_DeathController;
	XMyActionDieTransChecker m_ActionDieTransChecker;
private:
	void					CheckMFLockTime(MF_STATE mfState);
	void					EndMFLockTime();
	void					UpdateCheckMFReleaseTime();
	float					CalcHeightFromGroundZ();

	bool					CheckColTerrain();
public:
	XMyActionHit(XModuleMyControl* pOwner) : XMyActionState(pOwner), m_bDone(false), m_bDoneLockTime(true), m_bReservedEnd(false)	{}
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	virtual bool			OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand );
	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual	bool			IsPostMovement();		// 이것을 false로 하면 해당 액션 중일 때에는 XPostMove를 보내지 않는다.
	int						GetID() { return ACTION_STATE_HIT; }
};




#endif // _XMYACTION_HITTED_H