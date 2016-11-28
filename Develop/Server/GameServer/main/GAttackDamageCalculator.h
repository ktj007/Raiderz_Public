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

enum CALC_TYPE_FOR_WEAPON_DAMAGE
{
	CTWD_MINMAX,
	CTWD_MIN,
	CTWD_MAX
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
	virtual float CalcTalentFactor2( GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE nDamageType, WEAPON_REFRENCE nWeaponReference = WR_RIGHT );	
	virtual float CalcTalentFactor2_NotMagic(GEntityPlayer* pAttacker, WEAPON_TYPE nWeaponType);
	virtual float CalcTalentFactor2_NotMagic(GEntityPlayer* pAttacker, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType);

	virtual float CalcArmorFactor1( GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRate );
	virtual float CalcArmorFactor2( GEntityActor* pAttacker, float fWeaponApplyRate, WEAPON_REFRENCE nWeaponReference = WR_RIGHT );
	virtual float CalcArmorFactor2_Player(GEntityPlayer* pPlayerAttacker, float fWeaponApplyRate, WEAPON_TYPE nWeaponType);
	virtual float CalcArmorFactor2_Player(GEntityPlayer* pPlayerAttacker, float fWeaponApplyRate, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType);

	virtual int CalcNPCWeaponDamage(const GNPCInfo* pAttackerNPCInfo, CALC_TYPE_FOR_WEAPON_DAMAGE calcType = CTWD_MINMAX);
	virtual int CalcPlayerWeaponDamage(GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE damageType = TDT_PHYSIC, WEAPON_REFRENCE weaponReference = WR_RIGHT, CALC_TYPE_FOR_WEAPON_DAMAGE calcType = CTWD_MINMAX);
	virtual int doCalcWeaponDamage(GEntityActor* pActor, const GItem* pLeftWeaponItem, const GItem* pRightWeaponItem, TALENT_DAMAGE_TYPE damageType = TDT_PHYSIC, CALC_TYPE_FOR_WEAPON_DAMAGE calcType = CTWD_MINMAX);
	virtual int doCalcWeaponDamage(GEntityActor* pActor, const GItem* pWeaponItem, TALENT_DAMAGE_TYPE damageType = TDT_PHYSIC, CALC_TYPE_FOR_WEAPON_DAMAGE calcType = CTWD_MINMAX);

	bool	HasWeaponPassiveTalent( GEntityActor* pActor, const GItem* pWeaponItem );

	virtual float CalcBuffPlusFactor(GEntityActor* pAttacker, TALENT_DAMAGE_TYPE nDamageType);
	virtual float CalcBuffDefenceFactor( GEntityActor* pAttacker, GEntityActor* pVictim, DAMAGE_ATTRIB nDamageAttrib, WEAPON_REFRENCE nWeaponReference = WR_RIGHT );
	virtual int CalcTalentDamage( const GDamageRangedInfo& DamageInfo );

	virtual float CalcCharacterFactorForMagic(GEntityPlayer* pAttacker);
	virtual float CalcCharacterFactorForMelee(GEntityPlayer* pAttacker);
	virtual float CalcCharacterFactorForRange(GEntityPlayer* pAttacker);

	virtual int CalcBuffFinalDamage(int nDamage, float fTalentFactor1, float fArmorFactor1, int nWeaponDamage, float fWeaponApplyRate, float fTalentFactor2, float fArmorFactor2, float fCharacterFactor, float fBuffPlusFactor, float fBuffDefenceFactor, float fSpellPower);

	// debug를 위해 public으로 풀어둠
public:
	GAttackDamageCalculator(GCriticalCalculator* pCriticalCalculator=NULL);
	virtual ~GAttackDamageCalculator();
	virtual float CalcCharacterFactor(GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE nDamageType, WEAPON_REFRENCE nWeaponReference = WR_RIGHT);
	virtual float CalcAttackDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, const GDamageRangedInfo& DamageInfo);
	// virtual float CalcBuffAttackDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GBuffInfo* pBuffInfo, bool bCritical, const GDamageRangedInfo& DamageInfo);
	virtual bool CalcBuffRangedDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GBuffInfo* pBuffInfo, GDamageRangedInfo* poutDamageRangedInfo, GHealRangedInfo* poutHealRangedInfo, float* pfoutCriticalFactor);
	virtual int MakeDamageFromMinMaxDamage(const GDamageRangedInfo& DamageInfo);	

	static ATTACK_TYPE_FOR_CALCULATE CalcAttackTypeForCalculate(GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE nDamageType, WEAPON_REFRENCE nWeaponReference = WR_RIGHT);
	virtual float CalcTalentFactor1( GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRate );
};
