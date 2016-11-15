#pragma once

#include "GActorObserver.h"
#include "GKillRewardOwner.h"

class GEntityActor;

TEST_FORWARD_FT(QPKillRewardOwner, Fixture, OnDamage_NotLocateAndBelongTeam);
TEST_FORWARD_FT(QPKillRewardOwner, Fixture, OnDamage_Duel);
TEST_FORWARD_FT(QPKillRewardOwner, Fixture, OnDamage_FarDistanceMember);
TEST_FORWARD_FT(QPKillRewardOwner, Fixture, OnDamage);
TEST_FORWARD_FT(QPKillRewardOwner, Fixture, ClearPastRewardOwner);

class GQPKillRewardOwner : private GActorObserver, public GKillRewardOwner, public MTestMemPool<GQPKillRewardOwner>
{
TEST_FRIEND_FT(QPKillRewardOwner, Fixture, OnDamage_NotLocateAndBelongTeam);
TEST_FRIEND_FT(QPKillRewardOwner, Fixture, OnDamage_Duel);
TEST_FRIEND_FT(QPKillRewardOwner, Fixture, OnDamage_FarDistanceMember);
TEST_FRIEND_FT(QPKillRewardOwner, Fixture, OnDamage);
TEST_FRIEND_FT(QPKillRewardOwner, Fixture, ClearPastRewardOwner);

public:
	GQPKillRewardOwner(GEntityPlayer* pOwner);
	virtual ~GQPKillRewardOwner(void);

	virtual void OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType=EST_NONE, int nEffectSourceID=0) override;
	virtual void Clear() override;

private:	
	void ClearPastRewardOwner( DWORD nNowTime );
	void SaveLastDamageTime( DWORD nNowTime );

private:
	GEntityPlayer*	m_pOwner;
	DWORD			m_nLastDamageTime;
};
