#pragma once

#include "GCalculator.h"
#include "GTalentInfo.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"


// 계산용 공격 구분
enum ATTACK_TYPE_FOR_CALCULATE
{
	ATC_STR_WEAPON_ATTACK,		// 대부분의 근접무기 공격
	ATC_DEX_WEAPON_ATTACK,		// 장거리 무기, 찌르기 무기, 이도류 공격
	ATC_MAGIC_ATTACK			// 마법공격
};


class GCriticalCalculator;

// 공격 데미지 계산
// http://iworks2.maiet.net/wiki/sh:formulas:do
//
class GAttackDamageCalculator : public MTestMemPool<GAttackDamageCalculator>
{
protected:
	GCriticalCalculator*	m_pCriticalCalculator;
	static float m_fArmorFactor1Table[ARMOR_MAX][2];
	static float m_fArmorFactor2Table[ARMOR_MAX][WEAPON_MAX];
	virtual float GetArmorFactor1Table(ARMOR_TYPE nEquipmentType, bool bHolyUnholyMagic);
	virtual float GetArmorFactor2Table(ARMOR_TYPE nEquipmentType, WEAPON_TYPE nWeaponType);
	virtual float ClacTalentFactor2Fomula(int nBaseTalentRank, int nSpecializationTalentRank);
protected:
	float CalcPlayerTalentFactor1( GEntityPlayer* pPlayerAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRate );
	virtual float CalcTalentFactor2(GEntityPlayer* pAttacker, const GTalentInfo* pTalentInfo);	
	virtual float CalcTalentFactor2_NotMagic(GEntityPlayer* pAttacker, WEAPON_TYPE nWeaponType);
	virtual float CalcTalentFactor2_NotMagic(GEntityPlayer* pAttacker, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType);

	virtual float CalcArmorFactor1( GEntityActor* pAttacker, const GTalentInfo* pTalentInfo );
	virtual float CalcArmorFactor2( GEntityActor* pAttacker, const GTalentInfo* pTalentInfo );
	virtual float CalcArmorFactor2_Player(GEntityPlayer* pPlayerAttacker, const GTalentInfo* pTalentInfo, WEAPON_TYPE nWeaponType);
	virtual float CalcArmorFactor2_Player(GEntityPlayer* pPlayerAttacker, const GTalentInfo* pTalentInfo, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType);

	virtual int CalcNPCWeaponDamage(const GNPCInfo* pAttackerNPCInfo);
	virtual int CalcPlayerWeaponDamage(GEntityPlayer* pAttacker, const GTalentInfo* pTalentInfo, bool bCritical);
	virtual int doCalcWeaponDamage(GEntityActor* pActor, const GItem* pLeftWeaponItem, const GItem* pRightWeaponItem, bool bCritical);
	virtual int doCalcWeaponDamage(GEntityActor* pActor, const GItem* pWeaponItem, bool bCritical);

	bool	HasWeaponPassiveTalent( GEntityActor* pActor, const GItem* pWeaponItem );

	virtual float CalcBuffPlusFactor(GEntityActor* pAttacker, const GTalentInfo* pTalentInfo);
	virtual float CalcBuffDefenceFactor( GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo );
	virtual int CalcTalentDamage(const GDamageRangedInfo& DamageInfo, bool bCritical);

	virtual float CalcCharacterFactorForMagic(GEntityPlayer* pAttacker);
	virtual float CalcCharacterFactorForMelee(GEntityPlayer* pAttacker);
	virtual float CalcCharacterFactorForRange(GEntityPlayer* pAttacker);

	// debug를 위해 public으로 풀어둠
public:
	GAttackDamageCalculator(GCriticalCalculator* pCriticalCalculator=NULL);
	virtual ~GAttackDamageCalculator();
	virtual float CalcCharacterFactor(GEntityPlayer* pAttacker, const GTalentInfo* pTalentInfo);	
	virtual float CalcAttackDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, const GDamageRangedInfo& DamageInfo);
	virtual int MakeDamageFromMinMaxDamage(const GDamageRangedInfo& DamageInfo);	

	static ATTACK_TYPE_FOR_CALCULATE CalcAttackTypeForCalculate(GEntityPlayer* pAttacker, const GTalentInfo* pTalentInfo);
	virtual float CalcTalentFactor1( GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRatepTalentInfo );
};
