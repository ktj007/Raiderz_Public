#pragma once

#include "GCalculator.h"
#include "GTalentInfo.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"

/// 크리티컬 계산 공식
/// Factor는 1 == 1, Percent는 100(%) == 1
class GCriticalCalculator : public MTestMemPool<GCriticalCalculator>
{
protected:
	virtual int _GetSpecializationRankForDamageAttrib(GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib);

	// 치명타율
	virtual float CalcActorCriticalPercent(const GEntityActor* pActor, const GTalentInfo* pTalentInfo);	// 베이스 근접 크리율 계산
	virtual float _CalcPlayerCriticalPercent( TALENT_CATEGORY nTalentCategory, PLAYER_INFO* pPlayerInfo );	// PC의 캐릭터 치명타 율
	virtual float _CalcNPCCriticalPercent( TALENT_SKILL_TYPE nSkillType, GNPCInfo* pNPCInfo );

	virtual float CalcTalentPercent(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual float CalcDamageAttribPercent(GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib);
	virtual float _CalcDamageAttribPercent( DAMAGE_ATTRIB nDamageAttrib, int nSpecializationRank );

	// 데미지 관련
	virtual float CalcBuffDamagePercent(GEntityActor* pActor, const GTalentInfo* pTalentInfo);
	virtual int CalcDamageBonusPercent(GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib);
	virtual int _CalcDamageBonusPercent(DAMAGE_ATTRIB nDamageAttrib, int nSpecializationRank);

public:
	virtual float CalcCriticalPercent(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	virtual float CalcCriticalDamageFactor(GEntityActor* pAttacker, const GTalentInfo* pTalentInfo);
};

