#include "stdafx.h"
#include "XMyActionJump.h"
#include "XModuleMotion.h"
#include "XModuleMovable.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XModuleEntity.h"
#include "XModuleBuff.h"
#include "XObject.h"
#include "XMyPlayer.h"
#include "XEventID.h"
#include "XController.h"
#include "XMyActionIdle.h"
#include "XPost_Action.h"

// Jump //////////////////////////////////////////////////////////
bool XMyActionJump::CheckEnterable(void* pParam)
{
	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable()  ||  attrBuff.IsMovable() == false)
		return false;


	if (m_pOwner->GetMyPlayerOwner()->IsDead())
		return false;

	m_bAutoRunning = m_pOwner->IsAutoRun();

	return true;
}

void XMyActionJump::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);

	// OnJump에서 방향값을 주게 되어 있다.
	vec3 VOutDir(vec3::ZERO);
	bool bAction = false;
	XMotionTypes nMotionTypes = MT_DEFAULT;

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (!pModuleAction) return;

	bAction = pModuleAction->Jump(nMotionTypes, false, &VOutDir);
	if (bAction)
	{
		XPostJump(m_pOwner->GetOwner()->GetPosition(), VOutDir);
	}
	else
	{
		XEvent msg2;
		msg2.m_nID = XEVTL_JUMP_FAILED;
		m_pOwner->GetOwner()->Event(msg2);
	}
}

XEventResult XMyActionJump::Event(XEvent& msg)
{
	switch (msg.m_nID)
	{
		case XEVTL_MYACTION_USE_TALENT:
			{
				OnEvent_UseTalent(msg);

				return MR_TRUE;
			}
			break;
	}

	return XMyActionState::Event(msg);
}

void XMyActionJump::Exit()
{
	m_pOwner->SetAutoRun(m_bAutoRunning);
}

bool XMyActionJump::OnSpecialAction(SPECIAL_ACTION_COMMAND nSpecialActionCommand)
{
	// 점프에서는 스페셜 액션 명령어가 필요가 없다.
	XModuleMovable* pModuleMovable = m_pOwner->GetOwner()->GetModuleMovable();
	if (!pModuleMovable) return false;

	// 이동키가 작동해야만 다음 조건이 성립된다.
	// 착지하는 순간이라면... 그만하자.
	if(pModuleMovable->GetMovableFactor().GENERAL_STATE.m_bLanding)
	{
		if ( (gvar.Game.pMyPlayer->GetStance() == CS_BATTLE) &&
			gg.controller->IsVirtualKeyPressedAsync( VKEY_ACTION) == true)
		{
			m_pOwner->DoAction( ACTION_STATE_NORMAL_ATTACK, NA_NORMAL1);
			return false;
		}

		if(gg.controller->IsVirtualKeyPressedAsync(VKEY_FORWARD) ||
			gg.controller->IsVirtualKeyPressedAsync(VKEY_BACKWARD) ||
			gg.controller->IsVirtualKeyPressedAsync(VKEY_LEFT) ||
			gg.controller->IsVirtualKeyPressedAsync(VKEY_RIGHT) ||
			m_pOwner->m_bAutoRun)
		{
			XMyActionIdleParam idleParam;
			idleParam.bSetIdleAnimation = false;
			m_pOwner->DoAction(ACTION_STATE_IDLE, &idleParam);
			return true;
		}
	}
	return false;
}

void XMyActionJump::OnEvent_UseTalent( XEvent &msg )
{
	XModuleMovable* pModuleMovable = m_pOwner->GetOwner()->GetModuleMovable();
	if (!pModuleMovable) 
		return;

	// 착지시에 탤런트 사용 가능하다.
	if(pModuleMovable->GetMovableFactor().GENERAL_STATE.m_bLanding == false)
		return;

	int nTalentID = *(int*)(msg.m_pData);
	m_pOwner->DoAction(ACTION_STATE_USE_TALENT, nTalentID);		
}