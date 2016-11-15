#ifndef _XMYACTION_USETALENT_H
#define _XMYACTION_USETALENT_H

#pragma once

#include "XMyActionState.h"
#include "XTalentAvailable.h"

enum USE_TALENT_STATE
{
	UTS_NONE = 0,
	UTS_CHECK,
	UTS_ENTER,
	UTS_START,
	UTS_ACT,
	UTS_CANCEL,
	UTS_END
};

class XTarget;

///@brief UseTalent Action State
class XMyActionUseTalent : public XMyActionState
{
private:
	bool					m_bIsImmediateTalent;
	int						m_nTalentID;
	int						m_nCurrentTalentID;						// 현재 진행중인 탤런트 ID
	int						m_nTalentStep;							// 탤런트 단계
	float					m_fCastingTime;
	bool					m_bTargetingTalent;
	bool					m_bCastingEndEffectDone;

	XTarget *				m_pTarget;
	XTarget *				m_pUseTarget;

	USE_TALENT_STATE		m_nUseTalentState;						// 탤런트 사용 단계

	bool					ActSpell();
	bool					ValidateUse(XTalentInfo* pTalentInfo);
	void					CancleTalent();
	void					CancleSocialTalent();
	void					TalentStart(XTalentInfo* pTalentInfo);

	bool					CheckTalentCost(XTalentInfo* pTalentInfo);
	void					CheckCastingTime(float fDelta);
	bool					CheckActSpell();
	bool					CheckBuff( int nTalentID );
	bool					CheckEnterTalentMovableType(XTalentInfo* pTalentInfo);

	bool					IsUseTalent(XTalentInfo* pTalentInfo);
	bool					HasCastingTime(XTalentInfo* pTalentInfo);

	void					ShowCastingTimeUI( XTalentInfo* pTalentInfo );

	void					PostUseTalent();

	void					CreateTarget(XTalentInfo* pTalentInfo);
	void					CastingEndEffect();

public:
	XMyActionUseTalent(XModuleMyControl* pOwner);
	virtual ~XMyActionUseTalent();
	virtual bool			CheckEnterable(void* pParam);


	virtual void			Enter(void* pParam=NULL);


	virtual void			Update(float fDelta);
	virtual void			Exit();
	virtual bool			CheckExitable(int nNextStateID);
	virtual XEventResult	Event(XEvent& msg);
	virtual int				GetID() { return ACTION_STATE_USE_TALENT; }
	virtual	bool			IsPostMovement()	{ return true; }
	virtual bool			OnSpecialAction(SPECIAL_ACTION_COMMAND nSpecialActionCommand);

};


#endif // _XMYACTION_USETALENT_H