#include "StdAfx.h"
#include "GBuffReleaser.h"
#include "GBuff.h"
#include "GCombatCalculator.h"
#include "GBuffInfo.h"
#include "GEntityActor.h"

GBuffReleaser::GBuffReleaser( GBuff* pBuff )
: m_pBuff(pBuff)
, m_bCreateThisTick(true)
, m_nAccumulatedHitCount(0)
, m_nAccumulatedCriHitCount(0)
, m_nAccumulatedDamage(0)
{
	m_pBuff->AttachObserver(this);

	m_rgrResist.SetTickTime(1.0f);		// 저항체크는 초단위로 진행됨
	m_rgrResist.Start();
}

GBuffReleaser::~GBuffReleaser(void)
{
	m_pBuff->DetachObserver(this);
}

void GBuffReleaser::Update( float fDelta )
{
	m_bCreateThisTick = false;

	if (m_rgrResist.IsReady(fDelta))
	{
		OnResist();
	}
}

bool GBuffReleaser::CheckType( TALENT_CONDITION condition )
{
	VALID_RET(m_pBuff, false);
	GBuffInfo* pBuffInfo = m_pBuff->GetInfo();
	VALID_RET(pBuffInfo, false);

	for each (TALENT_CONDITION conditionAdditionRelease in pBuffInfo->m_Condition.infoRelease.vecAdditionType)
	{
		if (conditionAdditionRelease == condition)
			return true;
	}

	return (pBuffInfo->m_Condition.infoRelease.nType == condition);
}

bool GBuffReleaser::CheckParam( int nConditionParam )
{
	VALID_RET(m_pBuff, false);
	GBuffInfo* pBuffInfo = m_pBuff->GetInfo();
	VALID_RET(pBuffInfo, false);

	const int nReleaseParam = pBuffInfo->m_Condition.infoRelease.nParam;

	if (nReleaseParam == 0)
		return false;
	
	return (nReleaseParam <= nConditionParam);
}

void GBuffReleaser::Cancel()
{
	VALID(m_pBuff);
	m_pBuff->Cancel();
}

void GBuffReleaser::CancelForced()
{
	VALID(m_pBuff);
	m_pBuff->CancelForced();
}

void GBuffReleaser::OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	if (CheckType(TC_HIT_ME))
	{
		Cancel();
	}

	if (CheckType(TC_HIT_ME_N) &&
		CheckParam(++m_nAccumulatedHitCount))
	{
		Cancel();
	}
}

void GBuffReleaser::OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_HIT_ALL_TALENT))
		return;

	Cancel();
}

void GBuffReleaser::OnHitSuccess( GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_HIT_SUCCESS))
		return;

	Cancel();
}

void GBuffReleaser::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_HIT_ENEMY))
		return;

	Cancel();
}

void GBuffReleaser::OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	if (CheckType(TC_CRITICAL_HIT_ME))
	{
		Cancel();
	}

	if (CheckType(TC_CRIHIT_ME_N) &&
		CheckParam(++m_nAccumulatedCriHitCount))
	{
		Cancel();
	}
}

void GBuffReleaser::OnCriHitEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_CRITICAL_HIT_ENEMY))
		return;

	Cancel();
}

void GBuffReleaser::OnMiss( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (!CheckType(TC_MISS_ME))
		return;

	Cancel();
}

void GBuffReleaser::OnMissEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	if (!CheckType(TC_MISS_ENEMY))
		return;

	Cancel();
}

void GBuffReleaser::OnGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (!CheckType(TC_GUARD_ME))
		return;

	Cancel();
}

void GBuffReleaser::OnPerfectGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (!CheckType(TC_PERFECT_GUARD_ME))
		return;

	Cancel();
}

void GBuffReleaser::OnAbsoluteGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (!CheckType(TC_PERFECT_GUARD_ME))
		return;

	Cancel();
}

void GBuffReleaser::OnGuardEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	if (!CheckType(TC_GUARD_ENEMY))
		return;

	Cancel();
}

void GBuffReleaser::OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_USE_TALENT))
		return;

	Cancel();
}

void GBuffReleaser::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	if (m_bCreateThisTick)
		return;

	if (!CheckType(TC_FINISH_TALENT))
		return;

	Cancel();
}

void GBuffReleaser::OnBuffHit( MUID uidUser, GBuffInfo* pBuffInfo )
{
	if (!CheckType(TC_BUFF_HIT))
		return;

	Cancel();
}

void GBuffReleaser::OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID)
{
	if (!CheckType(TC_DAMAGE))
		return;

	m_nAccumulatedDamage += nDamage;
	if (!CheckParam(m_nAccumulatedDamage))
		return;

	Cancel();
}

void GBuffReleaser::OnAffected()
{
	if (!CheckType(TC_AFFECTED))
		return;

	Cancel();
}

void GBuffReleaser::OnResist()
{
	if (!CheckType(TC_RESIST))
		return;

	if (!m_pBuff->GetOwner()->IsActor())	
		return;

	// 저항 확률 체크
	GCombatCalculator calc;
	float fResistPercent; // dummy
	if (!calc.CheckResist(fResistPercent, NULL, ToEntityActor(m_pBuff->GetOwner()), m_pBuff->GetInfo()->m_Resist))
		return; // 저항 실패

	Cancel();
}

void GBuffReleaser::OnMaxStacked()
{
	if (!CheckType(TC_BUFF_MAX_STACKED))
		return;

	CancelForced();
}

void GBuffReleaser::OnMagicHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_MAGIC_HIT_ME))
		return;

	Cancel();
}

void GBuffReleaser::OnMagicHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_MAGIC_HIT_ENEMY))
		return;

	Cancel();
}

void GBuffReleaser::OnMagicActTalent( GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_MAGIC_ACT))
		return;

	Cancel();
}

void GBuffReleaser::OnMeleeHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_MELEE_HIT_ME))
		return;

	Cancel();
}

void GBuffReleaser::OnMeleeHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	if (!CheckType(TC_MELEE_HIT_ENEMY))
		return;

	Cancel();
}

void GBuffReleaser::OnDoSomething()
{
	if (!CheckType(TC_DO_SOMETHING))
		return;

	Cancel();
}

