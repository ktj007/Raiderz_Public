#pragma once

#include "GActorObserver.h"

class GEntityPlayer;

class GPlayerRide : public GActorObserver
{
public:
	class RandomDismountHandler
	{
	public:
		virtual float CalcDismountRate( GEntityActor* pAttacker, GEntityActor* pVictim );
	};
public:
	GPlayerRide(GEntityPlayer* pOwner);
	~GPlayerRide(void);

	bool IsMounted() const;
	void Dismount();

	void SetRandomHandler(RandomDismountHandler* pHandler);
	float GetMoveSpeed() const;

private:
	bool IsRideBuff(int nBuffID) const;
	int GetTestMode() const;
	void HandleDismountEvent(const string strEvent, GEntityActor* pAttacker=NULL);
	float CalcDismountRate(GEntityActor* pAttacker);
private:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) override;
	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 버프를 얻었을때 이벤트
	virtual void OnGainBuff(int nBuffID) override;
	// 버프를 잃었을때 이벤트
	virtual void OnLostBuff(int nBuffID) override;
private:
	GEntityPlayer* m_pOwner;
	GBuffInfo*		m_pMountBuff;
	RandomDismountHandler* m_pRandomHandler;
};
