#include "stdafx.h"
#include "XArcheryTalent.h"
#include "XTarget.h"
#include "XEventID.h"
#include "XAnim.h"
#include "XProjectile.h"

void XArcheryTalent::OnEnterPhasePrepare()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_PREPARE, m_pTalentInfo->m_szCastingAnimation.c_str(), m_pTalentInfo->m_szCastingLoopAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_PREPARE);
}

void XArcheryTalent::OnEnterPhaseAct()
{
	XEvent evt;
	evt.m_nID = XEVTL_ON_MOTION_EVENT; 
	evt.m_pData = MOTION_EVENT_DOSHOOT;
	m_pOwner->Event(evt);
}

void XArcheryTalent::OnStart()
{
}
