#include "stdafx.h"
#include "XMyActionFalling.h"
#include "XModuleAction.h"
#include "XModuleMyControl.h"
#include "XModuleBuff.h"
#include "XMyPlayer.h"
#include "XPost_Action.h"

bool XMyActionFalling::CheckEnterable(void* pParam)
{
	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable())
		return false;


	if (m_pOwner->GetMyPlayerOwner()->IsDead()) return false;

	return true;
}

void XMyActionFalling::Enter(void* pParam)
{
	bAutoRun = (pParam) ? *(bool*)pParam : false;

	XPostFalling(m_pOwner->GetOwner()->GetPosition(), m_pOwner->GetOwner()->GetDirection());

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction)
	{
		pModuleAction->ActionFalling();
	}
}

void XMyActionFalling::Exit()
{
	m_pOwner->SetAutoRun(bAutoRun);
}

XEventResult XMyActionFalling::Event(XEvent& msg)
{
	return XMyActionState::Event(msg);
}
