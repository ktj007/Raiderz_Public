#pragma once

#include "GActorObserver.h"
#include "CSTalentInfoEnum.h"
#include "GBuffObserver.h"

class GEntityActor;
class GTalentInfo;
class GBuffInfo;
class GBuff;

class GBuffReleaser : public GActorObserver, public GBuffObserver, public MTestMemPool<GBuffReleaser>
{
public:
	GBuffReleaser(GBuff* pBuff);
	virtual ~GBuffReleaser(void);

	// 틱 이벤트
	void Update(float fDelta);

private:
	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 피격 성공 이벤트 (탤런트 종료될 때 1회 호출)
	virtual void OnHitSuccess( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) override;
	// 모든 피격 성공 이벤트 (히트캡슐판정 용도)
	virtual void OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) override;
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
	virtual void OnPerfectGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo ) override;
	// 절대 방어 이벤트
	virtual void OnAbsoluteGuard( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo ) override;
	// 적의 공격을 막았을때 이벤트
	virtual void OnGuardEnemy(GEntityActor* pOwner, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo) override;
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo) override;
	// 버프에 피격됐을때 이벤트
	virtual void OnBuffHit(MUID uidUser, GBuffInfo* pBuffInfo) override;
	// 데미지를 얻었을 때 이벤트
	virtual void OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID) override;
	// 버프가 최대 스택된 경우
	virtual void OnMaxStacked() override;
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

	// 이동, 공격등 적극적인 행동을 했을 때 (Player 전용)
	virtual void OnDoSomething() override;
private:
	// Triggered 로 등록된 버프가 작용했을 때
	virtual void OnAffected() override;

private:
	// 매틱마다 저항 체크
	void OnResist();

	// 버프가 풀릴만한 조건인가?
	bool CheckType( TALENT_CONDITION condition );
	bool CheckParam( int nConditionParam );

	// 버프 취소
	void Cancel();
	void CancelForced();
private:
	GBuff*		m_pBuff;
	bool		m_bCreateThisTick;
	// 버프 저항 알리미
	MRegulator	m_rgrResist;

	// Instance Data To Check
	int			m_nAccumulatedHitCount;		///< 누적된 피격 수
	int			m_nAccumulatedCriHitCount;	///< 누적된 크리티컬 피격 수
	int			m_nAccumulatedDamage;		///< 누적된 데미지
};
