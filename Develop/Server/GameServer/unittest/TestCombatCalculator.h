#pragma once

#include "GCombatCalculator.h"
#include "GAttackDamageCalculator.h"

class SimpleCombatCalculator : public GCombatCalculator
{
public:
	virtual bool CheckMiss(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo)								override {	return false;	}
	virtual bool CheckResist(float& foutResistPercent, GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentResist& Resist)	override {	return false;	}
	virtual bool CheckCritical(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo)							override {	return false;	}
};

class SimpleAttackDamageCalculator : public GAttackDamageCalculator
{
public:
	virtual float CalcCharacterFactor(GEntityPlayer* pAttacker, 
		const GTalentInfo* pTalentInfo)
	{ return 1.0f; }
};

class CombatCalculator_ForCritical : public GCombatCalculator
{
public:
	virtual bool CheckMiss(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo)	override							
			{	return false;	}
	virtual bool CheckResist(float& foutResistPercent, GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentResist& Resist) override	
			{	return false;	}
	virtual bool CheckCritical(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo) override					
			{	return true;	}
};

class CombatCalculator_ForMiss : public GCombatCalculator
{
public:
	virtual bool CheckMiss(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo) override
			{	return true;	}
	virtual bool CheckResist(float& foutResistPercent, GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentResist& Resist) override
			{	return false;	}
	virtual bool CheckCritical(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo) override
			{	return false;	}
};
