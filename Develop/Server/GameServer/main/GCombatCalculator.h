#pragma once

class GEntityActor;
class GTalentInfo;
struct GTalentResist;

__interface ICombatCalculator
{
public:
	virtual bool CheckMiss(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual bool CheckRiposte(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual bool CheckResist(float& foutResistPercent, GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentResist& Resist);
	virtual bool CheckCritical(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual int CalcDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, float fResistFactor, const GDamageRangedInfo& DamageInfo, float* fOutMotionFactor);
};


class GCombatCalculator : public ICombatCalculator, public MTestMemPool<GCombatCalculator>
{
public:
	GCombatCalculator();

	// 회피 성공했으면 true 반환
	virtual bool CheckMiss(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual bool CheckRiposte(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	// 저항 성공했으면 true 반환
	virtual bool CheckResist(float& foutResistPercent, GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentResist& Resist);
	// 크리티컬 성공했으면 true 반환
	virtual bool CheckCritical(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual int CalcDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, float fResistFactor, const GDamageRangedInfo& DamageInfo,float* pfoutMotionFactor = NULL);
	virtual int CalcHealAmount( GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, const GHealRangedInfo& infoHeal);
	
	virtual float CalcPerfectGuardRate(GEntityActor* pAttacker, GEntityActor* pGuarder);	
	static float CalcPenetrationFactor(GEntityActor* pAttacker, const GTalentInfo* pTalentInfo);

private:
	bool IsFineDurationWeapon(GEntityActor* pAttacker, const GTalentInfo* pTalentInfo);
};
