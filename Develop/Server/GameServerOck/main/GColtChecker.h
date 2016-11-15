#pragma once

class GEntityNPC;
class GEntityActor;
struct GColtCheck;
struct GColtAction;
class GColtActionInst;

#define DECL_COLT_CHECK(_func)			bool _func(const GColtCheck& check, GEntityNPC* m_pSelfNeighborNPC);

class GColtChecker : public MTestMemPool<GColtChecker>
{
public:
	GColtChecker();

	bool CheckCondition(const GColtCheck& check, GEntityNPC* pNPC);
private:
	DECL_COLT_CHECK(OnHP);
	DECL_COLT_CHECK(OnEnemyHP);
	DECL_COLT_CHECK(OnUnbreakpart);
	DECL_COLT_CHECK(OnBreakpart);
	DECL_COLT_CHECK(OnDamage);
	DECL_COLT_CHECK(OnMode);
	DECL_COLT_CHECK(OnMotionfactor);
	DECL_COLT_CHECK(OnEnemyMotionfactor);
	DECL_COLT_CHECK(OnDistance);
	DECL_COLT_CHECK(OnAngle);
	DECL_COLT_CHECK(OnStress);
	DECL_COLT_CHECK(OnHitCapsule);
	DECL_COLT_CHECK(OnUsedTalent);
	DECL_COLT_CHECK(OnGainedBuff);
	DECL_COLT_CHECK(OnEnemyGainedBuff);
	DECL_COLT_CHECK(OnRear);
	DECL_COLT_CHECK(OnFront);
	DECL_COLT_CHECK(OnRage);
	DECL_COLT_CHECK(OnDead);
	DECL_COLT_CHECK(OnCombat);
	DECL_COLT_CHECK(OnPeace);
	
};
