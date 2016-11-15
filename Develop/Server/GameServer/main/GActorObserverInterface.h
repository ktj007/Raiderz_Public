#pragma once

class GEntityActor;
class GTalentInfo;
class GBuffInfo;
class GItem;
class GJob;

class GActorObserverInterface
{
public:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) {}
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) {}
	// 자신이 죽었을때
	virtual void OnDie() {}
	// 자신이 디스폰 되었을때 (NPC 전용)
	virtual void OnDespawn() {}
	// 자신의 객체가 소멸될때
	virtual void OnDestroy() {}
	// 자신이 필드를 변경할때 (Player 전용)
	virtual void OnChangeField() {}
	// 자신이 같은필드에서 순간이동 했을때 (Player 전용)
	virtual void OnWarpSameField() {}

	// 상대가 자신을 헤이트테이블에서 추가하였을때
	virtual void OnAddHateTarget( GEntityActor* pEnemy ) {}
	// 상대가 자신을 헤이트테이블에서 제거하였을때
	virtual void OnLostHateTarget( MUID uidEnemy ) {}
	// 자신의 헤이트테이블에 타켓이 추가되었을때 (NPC 전용)
	virtual void OnAddHateEntry( GEntityActor* pEnemy ) {}
	// 자신의 헤이트테이블에 추가적으로 파티멤버 타켓이 추가 되었을때 (NPC 전용)
	virtual void OnAddHateEntryMember(GEntityActor* pEnemyMember) {}
	// 자신의 헤이트테이블에 타겟이 제거되었을때 (NPC 전용)
	virtual void OnLostHateEntry(MUID nEnemyUID ) {}
	// 자신의 헤이트테이블이 비었을때
	virtual void OnEmptyHateTable() {}

	// 결투 준비 (Player 전용)
	virtual void OnDuelReady( const vector<MUID>& pEnemy ) {}
	// 결투 시작 (Player 전용)
	virtual void OnDuelStart( const vector<MUID>& pEnemy ) {}
	// 결투 종료 (Player 전용)
	virtual void OnDuelFinish( const vector<MUID>& pEnemy ) {}


	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) {}
	// 피격 성공 이벤트 (탤런트 종료될 때 1회 호출)
	virtual void OnHitSuccess( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) {}
	// 모든 피격 성공 이벤트 (히트캡슐판정 용도)
	virtual void OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) {}
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo)  {}
	// 치명타 피격 이벤트
	virtual void OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) {}
	// 적을 치명타로 공격했을때 이벤트
	virtual void OnCriHitEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo ) {}
	// 빗맞음 이벤트
	virtual void OnMiss(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  {}
	// 적이 공격을 빗맞췄을때 이벤트
	virtual void OnMissEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo)  {}
	// 방어 이벤트 (모든 방어)
	virtual void OnGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  {}
	// 완전 방어 이벤트
	virtual void OnPerfectGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  {}
	// 부분 방어 이벤트
	virtual void OnPartialGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  {}
	// 절대 방어 이벤트
	virtual void OnAbsoluteGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  {}
	// 적의 공격을 막았을때 이벤트
	virtual void OnGuardEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo)  {}
	// 데미지를 얻었을 때 이벤트
	virtual void OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID) {}
	// 누굴 죽였을 때 이벤트
	virtual void OnKilled(GEntityActor* pTarget) {}
	// 마법으로 자신이 피격됐을 떄
	virtual void OnMagicHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo) {}
	// 마법으로 자신이 가격했을 때
	virtual void OnMagicHitEnemy(GEntityActor* pTarget, GTalentInfo* pTalentInfo) {}
	// 접근공격으로 자신이 피격됐을 떄
	virtual void OnMeleeHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo) {}
	// 접근공격으로 자신이 가격했을 때
	virtual void OnMeleeHitEnemy(GEntityActor* pTarget, GTalentInfo* pTalentInfo) {}


	// 버프를 얻었을때 이벤트
	virtual void OnGainBuff(int nBuffID) {}
	// 버프를 잃었을때 이벤트
	virtual void OnLostBuff(int nBuffID) {}
	// 버프에 피격됐을때 이벤트
	virtual void OnBuffHit(MUID uidUser, GBuffInfo* pBuffInfo) {}

	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) {}
	// 탤런트 사용이 효과가 발동될때 호출되는 이벤트
	virtual void OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) {}
	// 마법 탤런트 사용이 효과가 발동될때 호출되는 이벤트
	virtual void OnMagicActTalent( GTalentInfo* pTalentInfo ) {}
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo) {}
	// 탤런트를 무효시켰을때 호출되는 이벤트
	virtual void OnTalentImmuned(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo) {}
	// 적이 탤런트를 무효화시켰을때 호출되는 이벤트
	virtual void OnTalentImmunedEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo) {}
	// 탤런트를 회피시켰을때 호출되는 이벤트
	virtual void OnTalentAvoid(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo) {}
	// 적이 탤런트를 회피시켰을때 호출되는 이벤트
	virtual void OnTalentAvoidEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo) {}


	// 저격으로 토글될때 호출되는 이벤트
	virtual void OnTogglePrecision() {}
	// 은신으로 토글될때 호출되는 이벤트
	virtual void OnToggleSneak(int nSneakBuffType) {}
	// 각성으로 토글될때 호출되는 이벤트
	virtual void OnToggleEnlighten() {}
	// 강신이 적용될때 호출되는 이벤트
	virtual void OnApplyAdvent() {}

	// 인터랙션 했을때 이벤트 (Player 전용)
	virtual void OnInteraction() {}

	// BuffEntity가 생성 됐을 때
	virtual void OnCreateBuffEntity(int nBuffID, MUID uidEntity) {}
	// BuffEntity가 소멸 됐을 때
	virtual void OnDestroyBuffEntity(int nBuffID, MUID uidEntity) {}	

	// NPC를 죽여서 보상을 받을때 이벤트 (Player 전용)
	virtual void OnRewarded(int nNPCID) {}


	// 아이템 강화 성공 (Player 전용)
	virtual void OnItemEnchantSuccess(GItem* pItem, int nBuffID) {}
	// 아이템 장착 (Player 전용)
	virtual void OnItemEquipped(GItem* pItem) {}
	// 아이템 장착해제 (Player 전용)
	virtual void OnItemUnequipped(GItem* pItem) {}
	// 장비셋 변경 (Player 전용)
	virtual void OnSwitchingWeaponSet(SH_ITEM_SWITCH_WEAPON val) {}

	// 잡이 완료 되었을 떄 (NPC 전용)
	virtual void OnJobFinished(const GJob* pJob) {}

	// 이동, 공격등 적극적인 행동을 했을 때 (Player 전용)
	virtual void OnDoSomething() {}
};

