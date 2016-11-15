#pragma once

#include "GActorObserver.h"
#include "GBuffObserver.h"

class GEntitySync;
class GEntityActor;
class GBuffInfo;

class GEFfectTimingChecker : public GActorObserver, public GBuffObserver, public MTestMemPool<GEFfectTimingChecker>
{
public:
	class Listener
	{
	public:
		virtual bool OnEvent(TALENT_CONDITION nTiming) = 0;
	};

	GEFfectTimingChecker();
	void AddListener(TALENT_CONDITION nTiming, Listener* pListenner);

private:
	void NotifyEvent(TALENT_CONDITION nTiming);

private:
	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
	// 치명타 피격 이벤트
	virtual void OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 적을 치명타로 공격했을때 이벤트
	virtual void OnCriHitEnemy( GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pTalentInfo ) override;
	// 빗맞음 이벤트
	virtual void OnMiss(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	// 적의 공격을 회피했을때 이벤트
	virtual void OnMissEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo) override;
	// 방어 이벤트
	virtual void OnGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	// 완전 방어 이벤트
	virtual void OnPerfectGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	// 절대 방어 이벤트
	virtual void OnAbsoluteGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	// 적의 공격을 막았을때 이벤트
	virtual void OnGuardEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo) override;
	// 마법으로 자신이 피격됐을 떄
	virtual void OnMagicHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	// 마법으로 자신이 가격했을 때
	virtual void OnMagicHitEnemy(GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
	// 마법 탤런트 사용이 효과가 발동될때 호출되는 이벤트
	virtual void OnMagicActTalent( GTalentInfo* pTalentInfo ) override;
	// 접근공격으로 자신이 피격됐을 떄
	virtual void OnMeleeHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	// 접근공격으로 자신이 가격했을 때
	virtual void OnMeleeHitEnemy(GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;

private:
	// 버프 얻을때 효과처리
	virtual void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) override;
	// 버프 잃을때 효과처리
	virtual void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute) override;
	// 버프 잃을때 효과처리
	virtual void OnPeriod() override;
	// 버프가 만료 됐을 때
	virtual void OnExpired(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) override;
	// 버프가 디스펠 됐을 때
	virtual void OnDispelled() override;
	// 버프가 중복되서 걸린 경우
	virtual void OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) override;
	// 버프가 최대 스택된 경우
	virtual void OnMaxStacked() override;
	// 버프가 스택되서 걸린 경우
	virtual void OnStacked(float fDurationTime, float fPeriodTime) override;

private:
	typedef map<TALENT_CONDITION, vector<Listener*>> TimingMap;
	TimingMap			m_mapTimings;
};
