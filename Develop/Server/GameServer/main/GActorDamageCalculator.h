#pragma once

#include "GDef.h"

class GEntityActor;
class GTalentInfo;
class GBuff;

// 전투 판정시 결과
struct GCombatTurnResult
{
	uint32					nResultFlags;	// COMBAT_TURN_RESULT 조합
	int						nDamage;
	bool					bBackHit;
	int						nHealAmount;
	int						nRestoreENAmount;
	int						nRestoreSTAAmount;
	GCombatTurnResult(uint32 result_flags = 0, int damage = 0, bool backhit = false, int heal = 0, int restore_en = 0, int restore_sta = 0) 
		: nResultFlags(result_flags), nDamage(damage), bBackHit(backhit), nHealAmount(heal), nRestoreENAmount(restore_en), nRestoreSTAAmount(restore_sta) {}
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
	GCombatTurnResult CalcBuffDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GBuff* pBuff );
};
