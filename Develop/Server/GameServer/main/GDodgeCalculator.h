#pragma once

class GTalentInfo;
class GEntityActor;

class GDodgeCalculator
{
public:
	/// 회피율 계산
	static float CalcDodgePercent(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);

protected:
	static float CalcAttackTalentDodgePercent(const GTalentInfo* pTalentInfo);
	static float CalcLevelFactorDodgePercent(const GEntityActor* pAttacker, const GEntityActor* pVictim);
	static float CalcAttackerDodgePercent(GEntityActor* pAttacker);
	static float CalcVictimDodgePercent(GEntityActor* pVictim);
	static float CalcBuffDodgePercent(GEntityActor* pActor, TALENT_CATEGORY nTalentCategory);
	static float CalcBuffHitPercent(GEntityActor* pActor, TALENT_CATEGORY nTalentCategory);	
};
