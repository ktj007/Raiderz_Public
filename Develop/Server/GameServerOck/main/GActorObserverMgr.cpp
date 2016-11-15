#include "StdAfx.h"
#include "GActorObserverMgr.h"
#include "GActorObserver.h"


void GActorObserverMgr::AttachObserver( GActorObserver* pOvr )
{
	VALID(pOvr);
	VALID(m_setActorObservers.find(pOvr) == m_setActorObservers.end());
	m_setActorObservers.insert(pOvr);
}

void GActorObserverMgr::DetachObserver( GActorObserver* pOvr )
{
	VALID(pOvr);
	VALID(m_setActorObservers.find(pOvr) != m_setActorObservers.end());
	m_setActorObservers.erase(pOvr);
}

void GActorObserverMgr::OnDespawn()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDespawn());
}

void GActorObserverMgr::OnDestroy()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDestroy());
}

void GActorObserverMgr::OnCombatBegin(GEntityActor* pOwner)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnCombatBegin(pOwner));
}

void GActorObserverMgr::OnCombatEnd(GEntityActor* pOwner)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnCombatEnd(pOwner));
}

void GActorObserverMgr::OnAddHateTarget( GEntityActor* pEnemy )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnAddHateTarget(pEnemy));
}

void GActorObserverMgr::OnLostHateTarget( MUID uidEnemy )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnLostHateTarget(uidEnemy));
}

void GActorObserverMgr::OnAddHateEntry( GEntityActor* pEnemy )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnAddHateEntry(pEnemy));
}

void GActorObserverMgr::OnAddHateEntryMember( GEntityActor* pEnemyMember )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnAddHateEntryMember(pEnemyMember));
}

void GActorObserverMgr::OnLostHateEntry(MUID nEnemyUID)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnLostHateEntry(nEnemyUID));
}

void GActorObserverMgr::OnEmptyHateTable()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnEmptyHateTable());
}

void GActorObserverMgr::OnHit(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnHit(pOwner, pAttacker, pTalentInfo));
}

void GActorObserverMgr::OnHitSuccess( GEntityActor* pOwner, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnHitSuccess(pOwner, pTalentInfo));
}

void GActorObserverMgr::OnHitAll(GEntityActor* pOwner, GTalentInfo* pTalentInfo)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnHitAll(pOwner, pTalentInfo));
}

void GActorObserverMgr::OnMiss( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMiss(pOwner, nCombatResultFalg, pAttacker, pAttackTalentInfo););
}

void GActorObserverMgr::OnGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnGuard(pOwner, pAttacker, pAttackTalentInfo));
}

void GActorObserverMgr::OnAbsoluteGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnAbsoluteGuard(pOwner, pAttacker, pAttackTalentInfo));
}

void GActorObserverMgr::OnPerfectGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnPerfectGuard(pOwner, pAttacker, pAttackTalentInfo));
}

void GActorObserverMgr::OnPartialGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnPartialGuard(pOwner, pAttacker, pAttackTalentInfo));
}

void GActorObserverMgr::OnGainBuff( int nBuffID )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnGainBuff(nBuffID));
}

void GActorObserverMgr::OnLostBuff( int nBuffID )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnLostBuff(nBuffID));
}

void GActorObserverMgr::OnBuffHit( MUID uidUser, GBuffInfo* pBuffInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnBuffHit(uidUser, pBuffInfo));
}

void GActorObserverMgr::OnHitEnemy(GEntityActor* pOwner,  uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnHitEnemy(pOwner, nCombatResultFalg, pTarget, pTalentInfo));
}

void GActorObserverMgr::OnCriHit(GEntityActor* pOwner,  GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnCriHit(pOwner, pAttacker, pTalentInfo));
}

void GActorObserverMgr::OnCriHitEnemy(GEntityActor* pOwner,  GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnCriHitEnemy(pOwner, pTarget, pTalentInfo));
}

void GActorObserverMgr::OnMissEnemy(GEntityActor* pOwner,  uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMissEnemy(pOwner, nCombatResultFalg, pTarget, pAttackTalentInfo));
}

void GActorObserverMgr::OnGuardEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnGuardEnemy(pOwner, pTarget, pAttackTalentInfo));
}

void GActorObserverMgr::OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDamage(pOwner, uidAttacker, nDamage, nEffectSourceType, nEffectSourceID));
}

void GActorObserverMgr::OnKilled( GEntityActor* pTarget )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnKilled(pTarget));
}

void GActorObserverMgr::OnMagicHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMagicHit(pAttacker, pTalentInfo));
}

void GActorObserverMgr::OnMagicHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMagicHitEnemy(pTarget, pTalentInfo));
}

void GActorObserverMgr::OnMeleeHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMeleeHit(pAttacker, pTalentInfo));
}

void GActorObserverMgr::OnMeleeHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMeleeHitEnemy(pTarget, pTalentInfo));
}

void GActorObserverMgr::OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnUseTalent(pUser, pTalentInfo));
}

void GActorObserverMgr::OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnActTalent(pUser, pTalentInfo));
}

void GActorObserverMgr::OnMagicActTalent( GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnMagicActTalent(pTalentInfo));
}

void GActorObserverMgr::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnFinishTalent(pTalentInfo));
}

void GActorObserverMgr::OnTalentImmuned( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnTalentImmuned(pOwner, pAttacker, pTalentInfo));
}

void GActorObserverMgr::OnTalentImmunedEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnTalentImmunedEnemy(pOwner, pTarget, pTalentInfo));
}

void GActorObserverMgr::OnTalentAvoid( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnTalentAvoid(pOwner, pAttacker, pTalentInfo));
}

void GActorObserverMgr::OnTalentAvoidEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnTalentAvoidEnemy(pOwner, pTarget, pTalentInfo));
}

void GActorObserverMgr::OnDie()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDie());
}

void GActorObserverMgr::OnTogglePrecision()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnTogglePrecision());
}

void GActorObserverMgr::OnToggleSneak(int nSneakBuffType)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnToggleSneak(nSneakBuffType));
}

void GActorObserverMgr::OnToggleEnlighten()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnToggleEnlighten());
}

void GActorObserverMgr::OnApplyAdvent()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnApplyAdvent());
}

void GActorObserverMgr::OnInteraction()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnInteraction());
}

void GActorObserverMgr::OnChangeField()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnChangeField());
}

void GActorObserverMgr::OnWarpSameField()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnWarpSameField());
}

void GActorObserverMgr::OnDuelReady( const vector<MUID>& pEnemy )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDuelReady(pEnemy));
}

void GActorObserverMgr::OnDuelStart( const vector<MUID>& pEnemy )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDuelStart(pEnemy));
}

void GActorObserverMgr::OnDuelFinish( const vector<MUID>& pEnemy )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDuelFinish(pEnemy));
}

void GActorObserverMgr::OnCreateBuffEntity(int nBuffID, MUID uidEntity )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnCreateBuffEntity(nBuffID, uidEntity));
}

void GActorObserverMgr::OnDestroyBuffEntity(int nBuffID, MUID uidEntity )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDestroyBuffEntity(nBuffID, uidEntity));
}

void GActorObserverMgr::OnRewarded(int nNPCID)
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnRewarded(nNPCID));
}

void GActorObserverMgr::OnItemEnchantSuccess( GItem* pItem, int nEnchantItemID )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnItemEnchantSuccess(pItem, nEnchantItemID));
}

void GActorObserverMgr::OnItemEquipped( GItem* pItem )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnItemEquipped(pItem));
}

void GActorObserverMgr::OnItemUnequipped( GItem* pItem )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnItemUnequipped(pItem));
}

void GActorObserverMgr::OnSwitchingWeaponSet( SH_ITEM_SWITCH_WEAPON val )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnSwitchingWeaponSet(val));
}

void GActorObserverMgr::OnJobFinished( const GJob* pJob )
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnJobFinished(pJob));
}

void GActorObserverMgr::OnDoSomething()
{
	NOTIFY_OBSERVERS(GActorObserver, m_setActorObservers, OnDoSomething());
}

