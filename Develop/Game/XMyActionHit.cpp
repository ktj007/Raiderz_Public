#include "stdafx.h"
#include "XMyActionHit.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XModuleEntity.h"
#include "XModulePost.h"
#include "XModuleMovable.h"
#include "XObject.h"
#include "XEventID.h"
#include "XTalentInfo.h"
#include "XMyPlayer.h"
#include "XCharacter.h"
#include "XTalentHitParam.h"
#include "XMotionHelper.h"
#include "XMotionFactorHelper.h"
#include "XGameState.h"
#include "XGameTransformControllerManager.h"
#include "CSMotionFactorHelper.h"
#include "XPost_Action.h"

#define END_ALLOW_HEIGHT	50.0f

// Hitted Action State //////////////////////////////////////////////////////////
bool XMyActionHit::CheckEnterable(void* pParam)
{
	XTalentHitParam * pHitInfo = static_cast<XTalentHitParam *>(pParam);
	if(pHitInfo == NULL)
		return false;

	if (CSMotionFactorHelper::IsNoneOrFakes(pHitInfo->nMFState)) return false;

	return true;
}

void XMyActionHit::Enter(void* pParam)
{
	m_nEnterTime = XGetNowTime();
	m_bDone = false;
	m_bReservedEnd = false;

	m_nMFState = MF_NONE;
	m_nMFWeight = 0;

	m_vPrePos = GetOwnerPlayer()->GetPosition();

 	XMyActionState::Enter(pParam);

	XTalentHitParam * pHitInfo = static_cast<XTalentHitParam *>(pParam);
	if(pHitInfo == NULL)
		return;

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if(pModuleAction)
	{
		pModuleAction->StopMove(true);

		m_nMFState = (MF_STATE)pHitInfo->nMFState;
		if (m_nMFState == MF_NONE &&	pModuleAction->CheckUsableBeatenFake()) 
			m_bDone = true;

		m_nMFWeight = pHitInfo->nMFWeight;

		// 일정 시간동안 움직이지 못한는 State
		CheckMFLockTime(m_nMFState);

		stMotionFactorResult rtMotionFactor;
		stServerKnockbackVictimInfo knockbackInfo(pHitInfo->vServerKnockbackDir, pHitInfo->vServerKnockbackVictimPos, pHitInfo->vServerKnockbackVictimTargetPos);
		pModuleAction->ActMF(m_nMFState, m_nMFWeight, pHitInfo->nAttackerUIID, knockbackInfo, pHitInfo->nTalentID, rtMotionFactor);

		// 죽은 처리...
		if(m_pOwner->GetMyPlayerOwner()->IsDead())
		{
			m_pOwner->OnDie(pHitInfo->nMFState, rtMotionFactor.bUseMFAnimation);
		}
	}
}

void XMyActionHit::Exit()
{
	bool bDie = m_pOwner->GetMyPlayerOwner()->IsDead();
	// MF_KNOCKBACK
	// 
	// knockback 인 경우 위치 sync를 다시 맞춰준다. 
	if (bDie == false &&
		(m_nMFState == MF_KNOCKBACK ||
		m_nMFState == MF_THROWUP ||
		m_nMFState == MF_DRAG))
	{
		XModulePost* pModulePost = m_pOwner->GetOwner()->GetModulePost();
		pModulePost->PostStop(m_pOwner->GetOwner()->GetPosition(),
							  m_pOwner->GetOwner()->GetDirection());
	}


	m_bReservedEnd = false;

	// 스턴중인데... 나가라면...
	if(m_nMFState == MF_STUN)
	{
		XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
		if(pModuleAction)
		{
			// exit라면... 모션이벤트를 보내지 않습니다.
			m_nMFState = MF_NONE;
			m_bDoneLockTime = true;
			pModuleAction->EndStun(false);
		}
	}

	XMyActionState::Exit();
}


XEventResult XMyActionHit::Event(XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTD_END_MF_STUN:
	case XEVTD_END_MF_KNOCKDOWN:
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:		// 갑자기 Idle이면 종료
		{
			if(msg.m_nID == XEVTL_ON_CHANGED_MOTION_TO_IDLE &&
				m_pOwner->GetMyPlayerOwner()->IsDead())
			{
				// 죽었는데... Idle로? 에 대한 예외처리
				m_pOwner->DoAction(ACTION_STATE_DIE);

				return MR_TRUE;
			}

			// 스턴, 넉다운
			// 끝났다.
			EndMFLockTime();
		}
		break;
	}

	if (XMyActionState::Event(msg) == MR_TRUE) return MR_TRUE;

	return MR_FALSE;
}


void XMyActionHit::Update(float fDelta)
{
	bool bCheckMotion = !m_DeathController.IsDeadMotionImmediately(m_nMFState);

	if (m_ActionDieTransChecker.IsReadyForHitState(fDelta, m_pOwner->GetMyPlayerOwner(), bCheckMotion)) 
	{
		if (m_pOwner->DoAction(ACTION_STATE_DIE))
		{
			return;
		}
	}

	UpdateCheckMFReleaseTime();

	if (m_bDone)
	{
		m_pOwner->DoActionIdle();
	}

	m_vPrePos = GetOwnerPlayer()->GetPosition();
}

bool XMyActionHit::OnSpecialAction( SPECIAL_ACTION_COMMAND nSpecialActionCommand )
{
	if (nSpecialActionCommand == SPECIAL_KEY_ADVANCE_ATTACK)
	{
		if (gvar.MyInfo.Talent.HasExtraPassiveTalent(TEPT_RISING_ATTACK) == false) return false;
		if (m_pOwner->GetMyPlayerOwner()->GetStance() == CS_NORMAL) return false;

		XModuleEntity* pModuleEntity = m_pOwner->GetOwner()->GetModuleEntity();
		if (pModuleEntity)
		{
			wstring t = pModuleEntity->GetAnimationController()->GetAnimationName();
			if (t.length() >= 6)
			{
				if (t.substr(t.length()-6) == L"MF_KD2")
				{
					m_pOwner->DoAction(ACTION_STATE_NORMAL_ATTACK, NA_RISING);	// 요상한 하드코딩-_-
					return true;
				}
			}
		}
	}
	else if (nSpecialActionCommand == SPECIAL_KEY_QUICK_RISE)
	{
		//if (gvar.MyInfo.Talent.HasExtraPassiveTalent(TEPT_SWORD_DRAWING) == false) return false;
		XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
		if (!pModuleMotion) return false;

		if (pModuleMotion->IsCurrentMotion(MOTION_NAME_MF_KNOCKDOWN))
		{
			XPostStandUp();
		}
	}

	return false;
}

bool XMyActionHit::CheckExitable(int nNextStateID)
{
	if(nNextStateID == ACTION_STATE_DIE)
	{
		return true;
	}

		
	// 일정시간동안 lock인 경우는 Exit 할 수 없다.
	if(m_bDoneLockTime == false &&
		(m_nMFState == MF_STUN || m_nMFState == MF_KNOCKDOWN))
	{
		return false;
	}

	if (m_nMFState == MF_UPPERED)
	{
		return  CheckColTerrain();

	}

	return true;
}

void XMyActionHit::CheckMFLockTime( MF_STATE mfState )
{
	// 일정 시간동안 움직이지 못한는 State
	if(m_nMFState == MF_STUN ||
		m_nMFState == MF_KNOCKDOWN)
	{
		m_bDoneLockTime = false;
	}
}

void XMyActionHit::EndMFLockTime()
{
	// 스턴, 넉다운
	// 끝났다.
	if(m_nMFState == MF_STUN ||
		m_nMFState == MF_KNOCKDOWN)
	{
		m_nMFState = MF_NONE;
	}

	m_bDoneLockTime = true;

	// 현재 애니메이션이 idle 이라면... DoAction 발동
	// 애니메이션이 없는 경우에 따른 처리
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if(pModuleAction)
	{
		if (XMotionHelper::IsIdleMotion(pModuleAction->GetCurrentMotionName()))
		{
			m_pOwner->DoActionIdle();
		}
	}
}

bool XMyActionHit::IsPostMovement()
{
	if(m_nMFState == MF_KNOCKDOWN)
		return true;

	return false;
}

void XMyActionHit::UpdateCheckMFReleaseTime()
{
	// release가 안될 경우를 대비한 안전대책

	float fElapsedTime = (XGetNowTime() - m_nEnterTime) / 1000.0f;

	switch (m_nMFState)
	{
	case MF_UPPERED:
		{
			if (gg.currentgamestate == NULL || gg.currentgamestate->GetWorld() == NULL) return;

			float fHeight = CalcHeightFromGroundZ();

			const float TOLER_TIME = 0.1f;
			const float TOLER_HEIGHT = 40.0f;

			float fReleaseTime = XMotionFactorHelper::CalcUpperedTime(m_nMFWeight);
			if (fElapsedTime > fReleaseTime + TOLER_TIME &&
				CheckColTerrain())
			{
				wstring strCurrMotionSequence = GetOwnerPlayer()->GetModuleMotion()->GetCurrSequence();
				if (strCurrMotionSequence != L"jump_land")
				{
					m_bDone = true;
				}
			}
		}
		break;
	}
}

float XMyActionHit::CalcHeightFromGroundZ()
{
	vec3 vOwnerPos = GetOwnerPlayer()->GetPosition();
	float fGroundZ = gg.currentgamestate->GetWorld()->GetGroundZ( vOwnerPos);
	float fHeight = vOwnerPos.z - fGroundZ;

	return fHeight;
}

bool XMyActionHit::CheckColTerrain()
{
	XModuleMovable* pModuleMovable = m_pOwner->GetOwner()->GetModuleMovable();
	if(pModuleMovable == NULL)
		return false;

	return pModuleMovable->GetMovableFactor().GENERAL_STATE.m_bColTerrain;
}