#include "stdafx.h"
#include "XNetActionGuard.h"
#include "XModuleNetControl.h"
#include "XEventID.h"
#include "XModuleAction.h"

XNetActionGuard::XNetActionGuard( XModuleNetControl* pOwner) : XNetActionState( pOwner)
{
	m_bGuardDefense = false;
	m_nGuardCnt = 0;
}


XNetActionGuard::~XNetActionGuard()
{
}


void XNetActionGuard::Enter( void* pParam /*=NULL*/)
{
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if(pModuleAction == NULL)
		return;

	pModuleAction->Stop();
	pModuleAction->Guard();

	XNetPlayer * pNetPlayer = m_pOwner->GetOwner();
	if(pNetPlayer) 
		pNetPlayer->SetGuard(true);

	m_bGuardDefense = false;
	m_nGuardCnt = 0;
}


void XNetActionGuard::Exit()
{
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if(pModuleAction == NULL)
		return;

	// 가드 넉백상태라면 가드릴리즈 애니메이션을 하지 않는다.
	if(m_pOwner->IsCurrentMotionGuardKnockback() == false)
	{
		pModuleAction->ActionGuardRelease();
	}

	XNetPlayer * pNetPlayer = m_pOwner->GetOwner();
	if(pNetPlayer)
		pNetPlayer->SetGuard(false);

	m_bGuardDefense = false;
	m_nGuardCnt = 0;

	XNetActionState::Exit();
}


XEventResult XNetActionGuard::Event( XEvent& msg)
{
	switch ( msg.m_nID)
	{
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			// 가드디펜스 이후 가드 상태이면 가드를 한다
			if(m_bGuardDefense == false ||
				m_nGuardCnt >= 1)
				return MR_FALSE;

			XNetPlayer* pNetPlayer = m_pOwner->GetOwner();
			if (pNetPlayer)
			{
				if(pNetPlayer->GetGuard())
				{
					XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
					if(pModuleAction)
					{
						pModuleAction->Guard();
						m_nGuardCnt++;
					}
				}
			}

			return MR_TRUE;
		}
	case XEVTL_GUARD_KNOCKBACK:
		{
			m_bGuardDefense = true;
			return MR_TRUE;
		}

	case XEVTL_GUARDED:
		{
			m_bGuardDefense = true;
			return MR_TRUE;
		}
	}

	return MR_FALSE;
}


void XNetActionGuard::Update( float fDelta)
{

}


bool XNetActionGuard::CheckEnterable( void* pParam)
{
	return true;
}

