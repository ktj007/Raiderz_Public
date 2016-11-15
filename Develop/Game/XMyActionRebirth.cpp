#include "StdAfx.h"
#include "XMyActionRebirth.h"
#include "XMyPlayer.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XModuleEffect.h"
#include "XModuleBuff.h"
#include "XEventID.h"

XMyActionRebirth::XMyActionRebirth( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
{

}

bool XMyActionRebirth::CheckEnterable(void* pParam)
{
	//mlog("리버스는 들어오네?\n");
	if (m_pOwner->GetMyPlayerOwner()->IsDead()) return false;

	//mlog("리버스는 통과!!\n");
	return true;
}

void XMyActionRebirth::Enter( void* pParam/*=NULL*/ )
{
	XMyActionState::Enter(pParam);

	m_bPlayDone = false;

	RebirthEffect();

	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction) pModuleAction->ActionRebirth();
}

void XMyActionRebirth::Exit()
{
	gvar.MyInfo.nDieTime = 0;

	XMyActionState::Exit();
}

void XMyActionRebirth::RebirthEffect()
{
	XModuleEffect * pModuleEffect = m_pOwner->GetOwner()->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_REBIRTH);
	}
}