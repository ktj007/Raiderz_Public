#pragma once

class GEntityActor;

class GGuardCalculator
{
public:
	static float CalcAPFactor(int nGuardTalentRank, bool bEquipShield, uint32 nGuardTime);

	static float CalcPerfectGuardRate(GEntityActor* pAttacker, GEntityActor* pGuarder);
	static float CalcPartialGuardRate(GEntityActor* pAttacker, GEntityActor* pGuarder);
	static float CalcPartialGuardDamageMod(GEntityActor* pGuarder, GEntityActor* pAttacker);
};
