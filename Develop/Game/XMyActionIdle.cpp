#include "stdafx.h"
#include "XMyActionIdle.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XEventID.h"
#include "XController.h"
#include "XModuleAction.h"
#include "XModuleMotion.h"
#include "XPost_Interaction.h"
#include "XPost_Sensor.h"
#include "XPost_Comm.h"
#include "XSpecialActionTransition.h"
#include "XSensorController.h"
#include "XSensorManager.h"
#include "XGameState.h"
#include "XMotionHelper.h"
#include "XInteractionHelper.h"
#include "XGatherMSG.h"
#include "XInteractionInfo.h"
#include "XMouseInteractor.h"
#include "XInteraction.h"


/// 전이 조건 설정 ---------------------
#define MAX_IDLE_KEYEVENT_CONDITION	9

static XMyActionTransition g_TestKeyEventCondition[MAX_IDLE_KEYEVENT_CONDITION] = { 
	// 다음 상태,						키,						키상태,			무기타입,					스탠스
	{ACTION_STATE_CHANGE_STANCE,		{VKEY_CHANGE_STANCE,	_PRESSED,		WC_ALL ^ (WC_NONE),			_STANCE_ALL}},
	{ACTION_STATE_NORMAL_ATTACK,		{VKEY_ACTION,			_PRESSEDONCE,	WC_ALL ^ (WC_ARC),			_STANCE_BATTLE}},
	{ACTION_STATE_SHOOT_ARROW,			{VKEY_ACTION,			_PRESSED,		WC_ARC,						_STANCE_BATTLE}},	
	{ACTION_STATE_JUMP,					{VKEY_JUMP,				_PRESSEDONCE,	WC_ALL,						_STANCE_ALL}},
	{ACTION_STATE_WEAPON_CHANGE,		{VKEY_CHANGE_WEAPON,	_PRESSED,		WC_ALL,						_STANCE_ALL}},
	{ACTION_STATE_SIT,					{VKEY_SIT,				_PRESSED,		WC_ALL,						_STANCE_ALL}},
};


// Idle //////////////////////////////////////////////////////////
bool XMyActionIdle::CheckEnterable(void* pParam)
{
	if (m_pOwner->GetMyPlayerOwner()->IsDead()) return false;

	return true;
}


bool XMyActionIdle::CheckExitable(int nNextStateID)
{
	if (nNextStateID != ACTION_STATE_CHANGE_CHANNEL)
	{
		// 수영중이면 나갈수 없다.
		if(m_pOwner->GetMyPlayerOwner()->IsDead() == false &&			// 안죽었을때...
			m_pOwner->GetMyPlayerOwner()->IsSwim() == true)
		{
			return false;
		}
	}

	// 오토런중에는...
	if(m_pOwner->IsAutoRun() == false &&
		nNextStateID != ACTION_STATE_CHANGE_STANCE &&
		nNextStateID != ACTION_STATE_WEAPON_CHANGE)
	{
		// 위 상태가 아니라면... 멈춘다.
		m_pOwner->SetAutoRun(false);
	}

	return true;
}

void XMyActionIdle::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);

	XMyActionIdleParam stIdleParam;
	if(pParam != NULL)
	{
		stIdleParam = *static_cast<XMyActionIdleParam*>(pParam);
	}

	if(stIdleParam.bSetIdleAnimation)
	{
		XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
		if (pModuleMotion) pModuleMotion->RemoveAllIdleMemory();

		XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
		if (pModuleAction) pModuleAction->Idle();
	}
}

void XMyActionIdle::Exit()
{
	gg.controller->SetFreeLookCamera(false);

	XMyActionState::Exit();
}

bool XMyActionIdle::OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	XSpecialActionTransChecker* pAttackChecker = m_pOwner->m_pAttackChecker;
	TALENT_NORMAL_ATTACK_TYPE nNextAttackType = pAttackChecker->Trans(nSpecialActionCommand);

	// 발도는 따로 처리
	if (nSpecialActionCommand == SPECIAL_KEY_SWORD_DRAWING)
	{
		if (m_pOwner->GetMyPlayerOwner()->GetStance() == CS_NORMAL &&
			m_pOwner->GetMyPlayerOwner()->IsSwim() == false)
		{
			if (pAttackChecker->CheckConst(NA_NA, NA_SWORD_DRAWING))
			{
				nNextAttackType = NA_SWORD_DRAWING;
			}
			else
			{
				m_pOwner->DoAction(ACTION_STATE_CHANGE_STANCE);
				return true;
			}
		}
	}

	if (pAttackChecker->CheckConst(NA_NA, nNextAttackType) == false) return false;

	switch (nNextAttackType)
	{
	case NA_SWORD_DRAWING:
		{
			m_pOwner->GetMyPlayerOwner()->SetStance(CS_BATTLE);
			m_pOwner->DoAction(ACTION_STATE_NORMAL_ATTACK, nNextAttackType);

			return true;
		}
		break;
	case NA_DODGE_FRONT:
	case NA_DODGE_BACK:
	case NA_DODGE_LEFT:
	case NA_DODGE_RIGHT:
	case NA_ADVANCE:
	case NA_UPPER:
	case NA_UPPERCOMBO1:
	case NA_COUNTER_ATTACK:
	case NA_SUPER:
		{
			m_pOwner->DoAction(ACTION_STATE_NORMAL_ATTACK, nNextAttackType);
			return true;
		}
		break;
	}

	return false;
}

XMyActionIdle::XMyActionIdle( XModuleMyControl* pOwner ) : XMyActionState(pOwner), m_bGuardKeyReleased(true)
{
	m_pInteraction = new XInteraction(pOwner);

	m_TargetNPCRegulator.SetTickTime(GAME_TARGET_NPC_CHECK_TICK);

	for (int i = 0; i < MAX_IDLE_KEYEVENT_CONDITION; i++)
	{
		m_KeyConditionChecker.AddCondition(g_TestKeyEventCondition[i]);
	}
}

XMyActionIdle::~XMyActionIdle()
{
	SAFE_DELETE(m_pInteraction);
}

void XMyActionIdle::Update( float fDelta )
{
	if (m_ActionDieTransChecker.IsReady(fDelta, m_pOwner->GetMyPlayerOwner())) 
	{
		if (m_pOwner->DoAction(ACTION_STATE_DIE))
		{
			return;
		}
	}

	m_pInteraction->Update(fDelta);

	UpdateUI(fDelta);

	bool bGuard = CheckGuardKey();

	// 자동 입력 상태이면..
	if ( XGETCFG_GAME_AUTONORMALATTACK  &&  global.ui->IsMouseVisible() == false  &&  bGuard == false && m_pOwner->GetMyPlayerOwner()->IsCurrStanceBattle())
	{
		XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
		if ( pModuleAction  &&  XMotionHelper::IsIdleMotion(pModuleAction->GetCurrentMotionName()) )
		{
			if ( gg.controller->IsVirtualKeyPressedAsync( VKEY_ACTION) == true)
				m_pOwner->DoAction( ACTION_STATE_NORMAL_ATTACK, NA_NORMAL1);
		}
	}

	if (CheckAutoStanceChanging())
	{
		if (m_pOwner->DoAction(ACTION_STATE_CHANGE_STANCE))
		{
			return;
		}
	}
}

XEventResult XMyActionIdle::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);

			if ( CheckKeyCondition(nVirtualKey, _PRESSEDONCE))
				return MR_TRUE;

		}
		break;
	case XEVTL_VIRTUAL_KEY_PRESSED:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);

			if ( CheckKeyCondition(nVirtualKey, _PRESSED))
				return MR_TRUE;

			if (nVirtualKey == VKEY_ACTION)
			{
				if (CheckEnabledToBattleStance())
				{
					if (m_pOwner->DoAction(ACTION_STATE_CHANGE_STANCE))
					{
						return MR_TRUE;
					}
				}
			}

			return m_pInteraction->Event(msg);
		}
		break;
	case XEVTL_VIRTUAL_KEY_RELEASED:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);

			switch (nVirtualKey)
			{
			case VKEY_GUARD:
				{
					m_bGuardKeyReleased = true;
				}
				break;
			}
		}
		break;
	case XEVTL_MYACTION_USE_TALENT:
		{
			int nTalentID = *(int*)(msg.m_pData);
			m_pOwner->DoAction(ACTION_STATE_USE_TALENT, nTalentID);
			return MR_TRUE;
		}
		break;
	case XEVTL_MOUSE_MOVE_CLICK:
		{
			return m_pInteraction->Event(msg);
		}
		break;
	}

	return MR_FALSE;
}

bool XMyActionIdle::CheckGuardKey()
{
	if (m_pOwner->IsCurrentGuardEnabled() == false)
		return false;

	m_pOwner->DoAction(ACTION_STATE_GUARD, &m_bGuardKeyReleased);

	if(m_bGuardKeyReleased)
		m_bGuardKeyReleased = false;

	return true;
}

bool XMyActionIdle::CheckKeyCondition( XVirtualKey nVirtualKey, unsigned int nKeyState )
{
	if ( gg.controller->IsVirtualKeyPressedAsync(VKEY_ACTION2)  &&  global.ui->IsMouseVisible() == false)
		return false;

	// 브레이커블파츠무기 사용
	if(m_pOwner->BPartsAction(nVirtualKey))	return true;

	ACTION_STATE nNextActionState = m_KeyConditionChecker.CheckKeyCondition(
		nVirtualKey, nKeyState,
		m_pOwner->GetMyPlayerOwner()->GetStance() );

	if (nNextActionState != ACTION_STATE_NONE)
	{
		m_pOwner->DoAction(nNextActionState);
		return true;
	}
	return false;
}

void XMyActionIdle::UpdateUI( float fDelta )
{
	m_CursorSmartReleaser.Update(fDelta, &global.ui->GetCursor(), global.ui, gg.controller);
}

bool XMyActionIdle::CheckAutoStanceChanging()
{
	if (!XGETCFG_GAME_AUTOCHANGE_TONORMALSTANCE) return false;

	GlobalVar_Game::MY_COMBAT_STANCE_AUTO_RELEASER_DATA& combatStanceAutoReleaserData = gvar.Game.MyCombatStanceAutoReleaserData;

	if (combatStanceAutoReleaserData.bReserved)
	{
		if (!gvar.Game.EnemyInfo.IsEmpty()) return false;

		const unsigned int MIN_AUTO_CHANGE_TIME = 5000;
		const unsigned int MAX_AUTO_CHANGE_TIME = 30000;

		unsigned int nNowTime = XGetNowTime();
		if (nNowTime - combatStanceAutoReleaserData.nReserveTime >= MIN_AUTO_CHANGE_TIME  &&
			nNowTime - combatStanceAutoReleaserData.nReserveTime <= MAX_AUTO_CHANGE_TIME)
		{
			if (GetOwnerPlayer()->GetStance() == CS_BATTLE)
			{
				return true;
			}
		}
	}

	return false;
}

bool XMyActionIdle::CheckEnabledToBattleStance()
{
	if (GetOwnerPlayer()->GetStance() == CS_NORMAL)
	{
		return true;
	}

	return false;
}