#ifndef _XMYACTION_NORMAL_ATTACK_H
#define _XMYACTION_NORMAL_ATTACK_H

#include "XMyActionState.h"


class XCombo;

///@brief 기본 공격 Action State
///@see
/// - 기본 공격의 콤보도 여기서 관리한다.
/// - doAttack에서 서버에 메세지를 전송한다.
class XMyActionNormalAttack : public XMyActionState
{
protected:

	TALENT_NORMAL_ATTACK_TYPE	m_AttackType;

	XCombo*						m_pCombo;

	wstring						m_strMotionName;
	wstring						m_strReturnMotionName;
	int							m_nTalentID;

	bool doAttack(TALENT_NORMAL_ATTACK_TYPE nAttackType);

	void UpdateCheckCombo();
	void SetupMotionNames();
	int GetTalentID( TALENT_NORMAL_ATTACK_TYPE nAttackType);
	void PostAttack(TALENT_NORMAL_ATTACK_TYPE nAttackType);
	bool CheckCanReserveCombo(SPECIAL_ACTION_COMMAND nSpecialActionCommand = SPECIAL_KEY_NONE);
	bool CheckSTA(int nTalentID);

	void doAutoNormalAttack();

	virtual int GetMyStatusSTA();

	bool CheckCanCommandWhenReturnMotion(SPECIAL_ACTION_COMMAND nSpecialActionCommand = SPECIAL_KEY_NONE, TALENT_NORMAL_ATTACK_TYPE nAttackType = NA_NA);
	bool CheckCurrentReturnMotion();

	TALENT_NORMAL_ATTACK_TYPE	ConvertFromParam(void* pParam);

	void OnEvent_UseTalent( XEvent &msg );
	void OnEvent_VirtualKeyPressed( XEvent &msg );
	void OnEvent_MotionPlayOnce();
	void OnEvent_OnCanCancelAction();
public:
	XMyActionNormalAttack(XModuleMyControl* pOwner);
	~XMyActionNormalAttack();
	virtual bool			CheckEnterable(void* pParam);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta);

	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);

	virtual bool			OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand );

	bool ImmediatelyAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand );
	bool ReserveCombo( SPECIAL_ACTION_COMMAND nSpecialActionCommand );
	int						GetID() { return ACTION_STATE_NORMAL_ATTACK; }
	virtual	bool			IsPostMovement()	{ return true; }

};





#endif // _XMYACTION_NORMAL_ATTACK_H