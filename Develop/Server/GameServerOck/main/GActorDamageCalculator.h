#pragma once

#include "GDef.h"

class GEntityActor;
class GTalentInfo;
class GBuffInfo;

// 전투 판정시 결과
struct GCombatTurnResult
{
	uint32					nResultFlags;	// COMBAT_TURN_RESULT 조합
	int						nDamage;
	int						nHealAmount;
	GCombatTurnResult() : nResultFlags(0), nDamage(0), nHealAmount(0) {}
	GCombatTurnResult(uint32 result_flags, int damage) : nResultFlags(result_flags), nDamage(damage), nHealAmount(0) {}
	GCombatTurnResult(uint32 result_flags, int damage, int heal) : nResultFlags(result_flags), nDamage(damage), nHealAmount(heal) {}
};

class GActorDamageCalculator
{
public:
	struct Desc
	{
		GEntityActor* pAttacker;
		GEntityActor* pVictim;
		GTalentInfo* pTalentInfo;
		GDamageRangedInfo DamageInfo;
		GHealRangedInfo HealInfo; 
		int			nHitCapGroup;
		int			nHitCapIndex;

		Desc()
			: pAttacker(NULL), pVictim(NULL), pTalentInfo(NULL), nHitCapGroup(INVALID_HITCAPSULEGROUP_ID), nHitCapIndex(-1)
		{}
	};
private:
	void SetBitIfDead(GCombatTurnResult& outResult, GEntityActor* pVictim);
public:
	GCombatTurnResult CalcDamage( const Desc& desc, float* pfoutMotionFactor=NULL);
	GCombatTurnResult CalcBuffDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GBuffInfo* pBuffInfo );
};
