#ifndef _XMYACTION_SWITCHING_WEAPON_H
#define _XMYACTION_SWITCHING_WEAPON_H

#include "XMyActionState.h"

typedef CHAR_STANCE SWITCHING_WEAPON_STATE;

enum WEAPON_CHANGE_STATE
{
	WCH_READY				= 0,
	WCH_WEAPON_CHANGE_READY,
	WCH_WEAPON_UNEQUIP,
	WCH_WEAPON_CHANGE,
	WCH_WEAPON_EQUIP,
	WCH_DONE
};
//////////////////////////////////////////////////////////////////////////
class XMyActionSwitchingWeapon : public XMyActionState
{
private:
	WEAPON_CHANGE_STATE		m_nWeaponChangeState;

	int						m_nCurrentWeaponSet;
	int8					m_nWeaponSet;
	uint32					m_nLastCheckEnterableTime;

private:
	void					Init();
	void					End();

	void					WeaponUnEquip();
	void					SwitchingWeaponReady();
	bool					SwitchingWeapon();
	void					WeaponEquip();

	void					ChangeStance(SWITCHING_WEAPON_STATE nSwitch);
	void					SwitchingWeaponModel();

	int						GetNextSwitcingWeapon(int nWeaponSet);
	
	bool					CheckEnterable_CheckCoolTime();
public:
	XMyActionSwitchingWeapon(XModuleMyControl* pOwner);

	virtual bool			CheckEnterable(void* pParam);
	virtual bool			CheckExitable(int nNextStateID);
	virtual void			Enter(void* pParam=NULL);
	virtual void			Update(float fDelta) {}
	virtual void			Exit();
	virtual XEventResult	Event(XEvent& msg);
	int						GetID() { return ACTION_STATE_WEAPON_CHANGE; }
	virtual	bool			IsPostMovement()	{ return true; }
};



#endif // _XMYACTION_WEAPON_CHANGE_H