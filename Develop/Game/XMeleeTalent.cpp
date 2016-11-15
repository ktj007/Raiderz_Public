#include "stdafx.h"
#include "XMeleeTalent.h"
#include "XModuleEffect.h"
#include "XModuleMotion.h"
#include "XEventID.h"

void XMeleeTalent::OnEnterPhasePrepare()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_PREPARE, m_pTalentInfo->m_szCastingAnimation.c_str(), m_pTalentInfo->m_szCastingLoopAnimation.c_str(), m_pTalentInfo->m_nMovableType);
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_PREPARE);
	// 검광
	SwordTarilStart();
}

void XMeleeTalent::OnEnterPhaseAct()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_ACT, m_pTalentInfo->m_szUseAnimation.c_str(), L"", m_pTalentInfo->m_nMovableType);
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_ACT);
	// 검광
	SwordTarilStart();
}

void XMeleeTalent::OnEnterPhaseFinish()
{
}

void XMeleeTalent::SwordTarilStart()
{
	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_SWORDTRAIL);
	}
}

void XMeleeTalent::SwordTarilEnd()
{
	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_END_SWORDTRAIL);
	}
}

XEventResult XMeleeTalent::OnEvent( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_ON_CHANGE_MOTION:
		{
			SwordTarilEnd();
		}
		break;
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			if(m_pTalentInfo->m_nMovableType == MUT_NONE &&
				m_fActElapsedTime > 0.0f)
			{
				PostDelayTalent();
			}
		}
		break;
	}

	// XBaseTalent::OnEvent 처리를 하지 말자. 문제가 생긴다. 
	// 예로 방어시 공격을 하면 m_bActive가 false가 되어서 move_dummy가 작동이 안된다.
	// Module 순서 때문에... 그런 현상이 생긴다. 꼭 XBaseTalent::OnEvent에 있는 메세지를 쓰고 싶으면 여기서 따로 작성하자.
	return MR_FALSE;
}
