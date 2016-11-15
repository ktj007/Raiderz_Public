#include "stdafx.h"
#include "XModuleAction.h"
#include "XModuleMyControl.h"
#include "XModuleMotion.h"
#include "XMyActionDeactivative.h"
#include "XEventID.h"
#include "XGame.h"
#include "XModuleBuff.h"
#include "XMyPlayer.h"
#include "XController.h"
#include "XModuleEntity.h"
#include "XMotionHelper.h"

XMyActionDeactivative::XMyActionDeactivative( XModuleMyControl* pOwner) : XMyActionState( pOwner), m_bCanUseTalent(false), m_nBuffID(0)
{
}


bool XMyActionDeactivative::CheckEnterable( void* pParam)
{
	return true;
}


void XMyActionDeactivative::Enter( void* pParam)
{
	m_fElapsed = 0.0f;

	XMyActionState::Enter( pParam);

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if ( pModuleAction)
	{
		pModuleAction->StopMove( true);
		pModuleAction->StopUpperAni();

		stDeactiveData stData = *(stDeactiveData*)pParam;
		m_strAnimationName = stData.strBuffAnimation;
		m_nBuffID = stData.nBuffID;
		if ( !m_strAnimationName.empty())
			pModuleAction->StartBuffAnimation(m_nBuffID, m_strAnimationName);
	}


	gg.controller->SetEnabled( false);
	gg.controller->ClearKeyBuffer();
}


bool XMyActionDeactivative::CheckExitable( int nNextStateID)
{
	// 다음이 탤런트 사용이면...
	if(nNextStateID == ACTION_STATE_USE_TALENT && m_bCanUseTalent)
		return true;

	// 히트는 풀린다?
	if(nNextStateID == ACTION_STATE_HIT ||
		nNextStateID == ACTION_STATE_GRAPPLED ||
		nNextStateID == ACTION_STATE_SWALLOWED)
		return true;

	// 버프에 의한 상태 확인
	if ( IsActivative() == true)
		return true;

	return false;
}


void XMyActionDeactivative::Exit()
{
	gg.controller->ClearKeyBuffer();
	gg.controller->SetEnabled( true);

	XMyActionState::Exit();
}


XEventResult XMyActionDeactivative::Event( XEvent& msg)
{
	m_bCanUseTalent = false;

	switch (msg.m_nID)
	{	
	case XEVTL_MYACTION_USE_TALENT:
			{
				int nTalentID = *(int*)(msg.m_pData);

				if(IsUseTalent(nTalentID))
				{
					m_bCanUseTalent = true;
					m_pOwner->DoAction(ACTION_STATE_USE_TALENT, nTalentID);
					return MR_TRUE;
				}
			}
	}

	return XMyActionState::Event( msg);
}


void XMyActionDeactivative::Update( float fDelta)
{
	m_bCanUseTalent = false;
	
	if (m_ActionDieTransChecker.IsReady(fDelta, m_pOwner->GetMyPlayerOwner())) 
	{
		if (m_pOwner->DoAction(ACTION_STATE_DIE))
		{
			return;
		}
	}

	m_fElapsed += fDelta;

	if ( m_fElapsed > 1.0f)
	{
		// 현재 애니메이션이 idle이면 원래 애니메이션으로 변경한다
		if ( !m_strAnimationName.empty())
		{
			XModuleEntity* pEntity = m_pOwner->GetOwner()->GetModuleEntity();
			XModuleMotion* pMotion = m_pOwner->GetOwner()->GetModuleMotion();
			XModuleAction* pAction = m_pOwner->GetOwner()->GetModuleAction();

			if ( pEntity  &&  pMotion  &&  pAction)
			{
				if ( pEntity->IsLoadingCompleted()  &&  XMotionHelper::IsIdleMotion(pMotion->GetCurrMotion()))
					pAction->StartBuffAnimation(m_nBuffID, m_strAnimationName);
			}
		}

		m_fElapsed = 0.0f;
	}
	
	XMyActionState::Update( fDelta);
}


bool XMyActionDeactivative::IsActivative()
{
	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable()  ||  attrBuff.IsMovable() == false)
		return false;

	return true;
}

bool XMyActionDeactivative::IsUseTalent( int nTalentID )
{
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	return attrBuff.IsUsableTalent(nTalentID);
}