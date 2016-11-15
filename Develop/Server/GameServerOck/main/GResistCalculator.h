#pragma once

class GEntityActor;
struct GTalentResist;

class GResistCalculator
{
public:
	static float CalcResistPercent(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GTalentResist& Resist);

private:
	static float CalcDefRate(const GEntityActor* pVictim, const GTalentResist& Resist);
};
