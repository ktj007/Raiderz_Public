#include "stdafx.h"
#include "XEventID.h"
#include "XItemTalent.h"
#include "XTarget.h"
#include "XModuleEntity.h"
#include "XProjectile.h"

void XItemTalent::OnEnterPhasePrepare()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_PREPARE, m_pTalentInfo->m_szCastingAnimation.c_str(), m_pTalentInfo->m_szCastingLoopAnimation.c_str());
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_PREPARE);
}

void XItemTalent::OnEnterPhaseAct()
{
	// 애니메이션 변경
	ChangeTalentMotion(TALENT_PHASE_ACT, m_pTalentInfo->m_szUseAnimation.c_str(), L"", m_pTalentInfo->m_nMovableType);
	// 이펙트 변경
	ChangeTalentEffect(TALENT_PHASE_ACT);
}

bool XItemTalent::IsNeedTarget()
{
	if(m_pTalentInfo->m_EffectInfo.m_nPoint == CSEffectInfo::POINT_TARGET)
		return true;

	return false;
}
