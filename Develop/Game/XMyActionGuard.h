#ifndef _XMYACTION_GUARD_H
#define _XMYACTION_GUARD_H

#include "XMyActionState.h"

// Guard Action State - 일단 ShotArrow에서 고대로 복사. Guard에 맞게 수정해주세요. - birdkr
class XMyActionGuard : public XMyActionState
{
protected:
	bool					m_bReleased;
	bool					m_bGuardKnockback;
	bool					m_bIdle;
	bool					m_bShowErrorMsg;
	unsigned int			m_nStartTime;

	class XGuardReserver
	{
	private:
		TALENT_NORMAL_ATTACK_TYPE	m_nReservedNormalAttackType;
	public:
		XGuardReserver() : m_nReservedNormalAttackType(NA_NA) {}
		bool IsReserved() { return (m_nReservedNormalAttackType != NA_NA); }
		bool IsComboEnabled()
		{
			switch (m_nReservedNormalAttackType)
			{
			case NA_USE_TALENT__CLIENT_ONLY:
				{
					return true;
				}
			}
			return false;
		}
		void Reserve(TALENT_NORMAL_ATTACK_TYPE nAttackType)
		{
			if (nAttackType == NA_NA || nAttackType == NA_USE_TALENT__CLIENT_ONLY)
			{
				Release();
				return;
			}

			m_nReservedNormalAttackType = nAttackType;
		}
		void Release() { m_nReservedNormalAttackType = NA_NA; }
		TALENT_NORMAL_ATTACK_TYPE GetReservedAttackType() { return m_nReservedNormalAttackType; }
	};

	XGuardReserver		m_AttackReserver;
	bool				m_bEnableAtkReserve;


	void					doRelease();
	void					guarded();
	void					ActionRelease();
	bool					CheckAvailableTalentOnGuard(int nTalentID);
public:
	XMyActionGuard(XModuleMyControl* pOwner) : XMyActionState(pOwner), m_bEnableAtkReserve(true), m_bGuardKnockback(false), m_bIdle(false), m_bShowErrorMsg(false)	{}
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual bool			CheckExitable(int nNextStateID);
	virtual void			Update(float fDelta);
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);



	virtual	bool			IsPostMovement() { return true; }
	bool					IsGuardKnockback()	{ return m_bGuardKnockback; }

	virtual bool			OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand );
	int						GetID() { return ACTION_STATE_GUARD; }
};


#endif // _XMYACTION_GUARD_H