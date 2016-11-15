#include "stdafx.h"
#include "XNetActionHit.h"
#include "XModuleNetControl.h"
#include "XModuleAction.h"
#include "XTalentHitParam.h"

// Hitted Action State //////////////////////////////////////////////////////////
void XNetActionHit::Enter(void* pParam)
{
	m_nMFState = MF_NONE;
	m_nMFWeight = 0;

 	XNetActionState::Enter(pParam);

	XTalentHitParam * pHitInfo = static_cast<XTalentHitParam *>(pParam);
	if(pHitInfo == NULL)
		return;

	stMotionFactorResult rtMotionFactor;
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if(pModuleAction)
	{
		m_nMFState = (MF_STATE)pHitInfo->nMFState;
		if (m_nMFState == MF_NONE &&	pModuleAction->CheckUsableBeatenFake()) 
			m_bDone = true;

		m_nMFWeight = pHitInfo->nMFWeight;

		stServerKnockbackVictimInfo knockbackInfo(pHitInfo->vServerKnockbackDir, pHitInfo->vServerKnockbackVictimPos, pHitInfo->vServerKnockbackVictimTargetPos);
		pModuleAction->ActMF(m_nMFState, m_nMFWeight, pHitInfo->nAttackerUIID, knockbackInfo, pHitInfo->nTalentID, rtMotionFactor);
	}

	// 죽은 처리...
	if(m_pOwner->GetOwner()->IsDead())
	{
		m_pOwner->OnDie(pHitInfo->nMFState, rtMotionFactor.bUseMFAnimation);
	}
}

XEventResult XNetActionHit::Event(XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTD_END_MF_STUN:
	case XEVTD_END_MF_KNOCKDOWN:
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:		// 갑자기 Idle이면 종료
		{
			// 스턴, 넉다운
			// 끝났다.
			EndMFLockTime();
		}
		break;
	}

	if (XNetActionState::Event(msg) == MR_TRUE) return MR_TRUE;

	return MR_FALSE;
}

bool XNetActionHit::CheckExitable(int nNextStateID)
{
	if(nNextStateID == ACTION_STATE_DIE)
	{
		return true;
	}
		
	// 일정시간동안 lock인 경우는 Exit 할 수 없다.
	if(m_nMFState == MF_STUN || m_nMFState == MF_KNOCKDOWN)
	{
		return false;
	}

	return true;
}

void XNetActionHit::EndMFLockTime()
{
	// 스턴, 넉다운
	// 끝났다.
	if(m_nMFState == MF_STUN ||
		m_nMFState == MF_KNOCKDOWN)
	{
		m_nMFState = MF_NONE;
	}

	// 현재 애니메이션이 idle 이라면... DoAction 발동
	// 애니메이션이 없는 경우에 따른 처리
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if(pModuleAction)
	{
		m_pOwner->DoAction( NET_ACTION_STATE_NA);
	}
}

void XNetActionHit::Update( float fDelta )
{
	if(m_bDone)
	{
		XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
		if(pModuleAction)
		{
			m_pOwner->DoAction( NET_ACTION_STATE_NA);
		}
	}
}