#include "stdafx.h"
#include "XGuardTalent.h"

void XGuardTalent::OnEnterPhasePrepare()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_PREPARE, m_pTalentInfo->m_szCastingAnimation.c_str(), m_pTalentInfo->m_szCastingLoopAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_PREPARE);
}

void XGuardTalent::OnEnterPhaseAct()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_ACT, m_pTalentInfo->m_szUseAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_ACT);
}

void XGuardTalent::OnEnterPhaseFinish()
{
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	pActor->SetGuard(false);
}

void XGuardTalent::OnStart()
{
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	pActor->SetGuard(true);

	m_fGuardDurationTime = 0.0f;
}

void XGuardTalent::OnUpdate( float fDelta )
{
	// 시간이 지나면 탤런트 종료
	if(CheckGuardTalentDurationTimeLimite(fDelta) == true)
		Cancel();

	XBaseTalent::OnUpdate(fDelta);
}

bool XGuardTalent::CheckGuardTalentDurationTimeLimite( float fDelta )
{
	XTalentInfo* pTalentInfo = GetInfo();
	if(pTalentInfo &&
		(pTalentInfo->m_fDurationTime > 0.0f))
	{
		m_fGuardDurationTime += fDelta;
		if(pTalentInfo->m_fDurationTime <= m_fGuardDurationTime)
		{
			// 탤런트 종료
			m_fGuardDurationTime = 0.0f;
			return true;
		}		
	}

	return false;
}