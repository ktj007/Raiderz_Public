#include "StdAfx.h"
#include "GEFfectTimingChecker.h"
#include "GEntitySync.h"
#include "GEntityActor.h"


GEFfectTimingChecker::GEFfectTimingChecker() 
{

}

void GEFfectTimingChecker::AddListener( TALENT_CONDITION nTiming, Listener* pListenner )
{
	if (m_mapTimings.find(nTiming) == m_mapTimings.end())
	{
		m_mapTimings.insert(make_pair(nTiming, vector<Listener*>()));
	}

	m_mapTimings[nTiming].push_back(pListenner);
}

void GEFfectTimingChecker::OnHit(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo)
{
	NotifyEvent(TC_HIT_ME);
}

void GEFfectTimingChecker::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_HIT_ENEMY);
}

void GEFfectTimingChecker::OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_CRITICAL_HIT_ME);
}

void GEFfectTimingChecker::OnCriHitEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_CRITICAL_HIT_ENEMY);
}

void GEFfectTimingChecker::OnMiss( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NotifyEvent(TC_MISS_ME);
}

void GEFfectTimingChecker::OnMissEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	NotifyEvent(TC_MISS_ENEMY);
}

void GEFfectTimingChecker::OnGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NotifyEvent(TC_GUARD_ME);
}

void GEFfectTimingChecker::OnPerfectGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NotifyEvent(TC_PERFECT_GUARD_ME);
}

void GEFfectTimingChecker::OnAbsoluteGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NotifyEvent(TC_PERFECT_GUARD_ME);
}

void GEFfectTimingChecker::OnGuardEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	NotifyEvent(TC_GUARD_ENEMY);
}

void GEFfectTimingChecker::OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	NotifyEvent(TC_BUFF_GAIN);
	NotifyEvent(TC_BUFF_GAIN_N_PERIOD);
}

void GEFfectTimingChecker::OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute)
{
	NotifyEvent(TC_BUFF_GONE);
}

void GEFfectTimingChecker::OnPeriod()
{
	NotifyEvent(TC_BUFF_PERIOD);
	NotifyEvent(TC_BUFF_GAIN_N_PERIOD);
}

void GEFfectTimingChecker::OnExpired(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	NotifyEvent(TC_BUFF_EXPIRED);
}

void GEFfectTimingChecker::OnDispelled()
{
	NotifyEvent(TC_BUFF_DISPELLED);
}

void GEFfectTimingChecker::OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	NotifyEvent(TC_BUFF_DUPLICATED);
}

void GEFfectTimingChecker::OnMaxStacked()
{
	NotifyEvent(TC_BUFF_MAX_STACKED);
}

void GEFfectTimingChecker::OnStacked( float fDurationTime, float fPeriodTime )
{
	NotifyEvent(TC_BUFF_STACKED);
}

void GEFfectTimingChecker::NotifyEvent(TALENT_CONDITION nTiming)
{
	const vector<Listener*>& vecListeners = m_mapTimings[nTiming];
	for each (Listener* each in vecListeners)
	{
		if (each->OnEvent(nTiming))
			return; // 소유자가 죽었음
	}
}

void GEFfectTimingChecker::OnMagicHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_MAGIC_HIT_ME);
}

void GEFfectTimingChecker::OnMagicHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_MAGIC_HIT_ENEMY);
}

void GEFfectTimingChecker::OnMagicActTalent( GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_MAGIC_ACT);
}

void GEFfectTimingChecker::OnMeleeHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_MELEE_HIT_ME);
}

void GEFfectTimingChecker::OnMeleeHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NotifyEvent(TC_MELEE_HIT_ENEMY);
}



