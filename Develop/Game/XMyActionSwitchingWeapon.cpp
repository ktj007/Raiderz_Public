#include "stdafx.h"
#include "XMyActionSwitchingWeapon.h"
#include "XModuleMotion.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XModuleBuff.h"
#include "XMyPlayer.h"
#include "XEventID.h"
#include "XMyActionIdle.h"
#include "XPost_Action.h"

// Change State //////////////////////////////////////////////////////////
XMyActionSwitchingWeapon::XMyActionSwitchingWeapon(XModuleMyControl* pOwner) : XMyActionState(pOwner) 
, m_nWeaponChangeState(WCH_READY) 
, m_nWeaponSet(0) 
, m_nLastCheckEnterableTime(0)
{

}

void XMyActionSwitchingWeapon::Init()
{
	m_nWeaponChangeState		= WCH_READY;
	m_nWeaponSet				= 0;
	m_nCurrentWeaponSet			= -1;
}

void XMyActionSwitchingWeapon::End()
{
	m_pOwner->DoActionIdle();
}

bool XMyActionSwitchingWeapon::CheckEnterable(void* pParam)
{
	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable())
		return false;

	Init();

	if (m_pOwner->GetMyPlayerOwner()->IsDead()) 
	{
		return false;
	}

	// 쿨타임 체크
	if (CheckEnterable_CheckCoolTime() == false)
	{
		return false;
	}

	// 현재 무기 슬롯 번호를 기억을 한다.
	m_nCurrentWeaponSet = gvar.MyInfo.EquipmentSlot.GetWeaponSet();
	// 전투 상태가 아니라면...
	// 무기만 바꾼다.
	if (m_pOwner->GetMyPlayerOwner()->IsCurrStanceBattle() == false) 
	{
		SwitchingWeapon();
		return false;
	}

	bool bDoAction = false;
	XMotionTypes nMotionTypes = MT_DEFAULT; //GetCurrMotionType();

	// 현재 바꿀수 있는 애니메이션 상태인가?
	int8 nStanceTo = CS_NORMAL;
	if (m_pOwner->GetMyPlayerOwner()->IsCurrStanceBattle())
	{
		nStanceTo = CS_NORMAL;
	}
	else 
	{
		nStanceTo = CS_BATTLE;
	}

	if (m_pOwner->GetMyPlayerOwner()->ValidateChangeStance((CHAR_STANCE)(nStanceTo)) == false) 
	{
		return false;
	}

	// 바꿀 무기가 있는가?
	if(GetNextSwitcingWeapon(m_nCurrentWeaponSet) == -1)
		return false;

	// 브레이커블파츠 무기 사용중이어서 사용 가능한 액션인가?
	if(m_pOwner->UsableActionState_OverlappedEquipmentItem(GetID()) == false)
		return false;

	return true;
}

bool XMyActionSwitchingWeapon::CheckEnterable_CheckCoolTime()
{
	const uint32 SWITCHING_WEAPON_COOL_TIME = 500;

	uint32 nNowTime = XGetNowTime();
	if (m_nLastCheckEnterableTime != 0 && nNowTime - m_nLastCheckEnterableTime < SWITCHING_WEAPON_COOL_TIME) return false;

	m_nLastCheckEnterableTime = nNowTime;

	return true;
}

void XMyActionSwitchingWeapon::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);

	// 무기 체인지를 서버에게 알린다.
	// 바꿀 무기가 있는지에 대한 체크는 이미 CheckEnterable()에서 했다.
	SwitchingWeaponReady();
}

void XMyActionSwitchingWeapon::Exit()
{
	Init();
	XMyActionState::Exit();
}

XEventResult XMyActionSwitchingWeapon::Event(XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTL_UPPER_MOTION_PLAYONCE:
	case XEVTL_MOTION_PLAYONCE:
		{
			// 애니메이션 완료
			if(m_nWeaponChangeState == WCH_WEAPON_UNEQUIP)
			{
				// 무기 변경 메세지 보냄
				SwitchingWeapon();
			}
			else if(m_nWeaponChangeState == WCH_WEAPON_EQUIP)
			{
				// 변경 된 무기 장착 완료
				XMyActionIdleParam idleParam;
				idleParam.bSetIdleAnimation = false;
				m_pOwner->DoAction(ACTION_STATE_IDLE, &idleParam);
			}

			return MR_TRUE;
		}
		break;

	case XEVTL_MYACTION_SWITCHING_WEAPON_BEGIN:
		{
			if(m_nWeaponChangeState == WCH_WEAPON_CHANGE_READY)
			{
				WeaponUnEquip();	
			}
		}
		break;

	case XEVTL_MYACTION_SWITCHING_WEAPON_START:
		{
			if(m_nWeaponChangeState == WCH_WEAPON_CHANGE)
			{
				m_nWeaponSet		= *static_cast<int8*>(msg.m_pData);

				// 무기 변경
				WeaponEquip();
			}

			return MR_TRUE;
		}
	}

	return XMyActionState::Event(msg);
}

void XMyActionSwitchingWeapon::ChangeStance(SWITCHING_WEAPON_STATE nSwitch)
{
	// 무기 변경 모션 처리
	bool bQuick = false;
	if (gvar.MyInfo.Talent.HasExtraPassiveTalent(TEPT_QUICK_DRAW)) 
	{
		bQuick = true;
	}

	m_pOwner->GetMyPlayerOwner()->ChangeWeapon(nSwitch, bQuick);

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (!pModuleAction) return;

	//pModuleAction->StopMove();
}

void XMyActionSwitchingWeapon::WeaponUnEquip()
{
	m_nWeaponChangeState	= WCH_WEAPON_UNEQUIP;
	ChangeStance(CS_NORMAL);
}

void XMyActionSwitchingWeapon::SwitchingWeaponReady()
{
	m_nWeaponChangeState = WCH_WEAPON_CHANGE_READY;

	XPostRequestSwitchingWeaponSetBegin();
	return;
}

bool XMyActionSwitchingWeapon::SwitchingWeapon()
{
	m_nWeaponChangeState = WCH_WEAPON_CHANGE;

	int nNextWeaponSet = GetNextSwitcingWeapon(m_nCurrentWeaponSet);
	if(nNextWeaponSet == -1)
	{
		return false;
	}

	XPostRequestSwitchingWeaponSet(nNextWeaponSet);
	return true;
}

void XMyActionSwitchingWeapon::WeaponEquip()
{
	m_nWeaponChangeState	= WCH_WEAPON_EQUIP;

	// 무기를 변경한다.
	SwitchingWeaponModel();

	// 변경한 무기를 토대로 전투 준비
	ChangeStance(CS_BATTLE);
}

void XMyActionSwitchingWeapon::SwitchingWeaponModel()
{
	gvar.Game.pMyPlayer->SwitchingWeaponSet(m_nWeaponSet);
}

int XMyActionSwitchingWeapon::GetNextSwitcingWeapon( int nWeaponSet )
{
	// -1은 바꿀 무기가 없다.
	if(nWeaponSet == 0)
	{
		// 현재 주무기 이면...
		XItem* pItemR = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_RWEAPON2);

		if(pItemR)
		{
			return 1;
		}
	}
	else
	{
		// 현재 보조무기 이면...
		XItem* pItemR = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_RWEAPON);

		if(pItemR)
		{
			return 0;
		}
	}

	return -1;
}

bool XMyActionSwitchingWeapon::CheckExitable( int nNextStateID )
{
	if(nNextStateID == ACTION_STATE_DEACTIVATIVE ||
		nNextStateID == ACTION_STATE_HIT ||
		nNextStateID == ACTION_STATE_GRAPPLED ||
		nNextStateID == ACTION_STATE_SWALLOWED)
		return true;

	if(m_nWeaponChangeState != WCH_WEAPON_EQUIP)
	{
		return false;
	}

	return true;
}