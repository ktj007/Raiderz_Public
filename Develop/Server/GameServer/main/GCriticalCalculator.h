#pragma once

#include "GCalculator.h"
#include "GTalentInfo.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GBuffInfo.h"

/// 크리티컬 계산 공식
/// Factor는 1 == 1, Percent는 100(%) == 1
class GCriticalCalculator : public MTestMemPool<GCriticalCalculator>
{
protected:
	virtual int _GetSpecializationRankForDamageAttrib(GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib);

	// 치명타율
	virtual float CalcActorCriticalPercent(const GEntityActor* pActor, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType);	// 베이스 근접 크리율 계산
	virtual float _CalcPlayerCriticalPercent( TALENT_DAMAGE_TYPE nDamageType, const GEntityPlayer* pPlayer );	// PC의 캐릭터 치명타 율
	virtual float _CalcNPCCriticalPercent( TALENT_SKILL_TYPE nSkillType, GNPCInfo* pNPCInfo );

	virtual float CalcTalentPercent(GEntityActor* pAttacker, GEntityActor* pVictim, TALENT_DAMAGE_TYPE nDamageType, float fCriticalApplyRate = 1.f);
	virtual float CalcDamageAttribPercent(GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib);
	virtual float _CalcDamageAttribPercent( DAMAGE_ATTRIB nDamageAttrib, int nSpecializationRank );

	// 데미지 관련
	virtual float CalcBuffDamagePercent(GEntityActor* pActor, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType);
	virtual int CalcDamageBonusPercent(GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib);
	virtual int _CalcDamageBonusPercent(DAMAGE_ATTRIB nDamageAttrib, int nSpecializationRank);

public:
	virtual float CalcCriticalPercent(GEntityActor* pAttacker, GEntityActor* pVictim, DAMAGE_ATTRIB nDamageAttrib, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType, float fCriticalApplyRate = 1.f);
	virtual float CalcCriticalDamageFactor(GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType);
};

