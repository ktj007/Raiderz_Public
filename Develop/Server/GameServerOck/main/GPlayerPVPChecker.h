#pragma once

#include "GActorObserver.h"

class GEntityPlayer;

class GPlayerPVPChecker : public GActorObserver, public MTestMemPool<GPlayerPVPChecker>
{
public:
	GPlayerPVPChecker(GEntityPlayer* pOwner);
	~GPlayerPVPChecker();
	
	// 전투 해제
	void doMakePeace();
	// PVP 중인지 여부
	bool IsNowPVP() const;
	// 틱 처리
	void Update(float fDelta);
private:
	// PVP 시작 설정
	void BeginPVP();
	// PVP 종료 설정
	void FinishPVP();

	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
private:
	GEntityPlayer* m_pOwner;
	MRegulator	m_rgrReleaser;
	bool		m_bNowPVP;
};
