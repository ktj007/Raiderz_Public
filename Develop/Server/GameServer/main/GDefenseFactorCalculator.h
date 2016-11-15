#pragma once

#include "CSDef.h"
#include "CSChrInfo.h"

class GEntityActor;
class GBuffInfo;
class GTalentInfo;

class GDefenseFactorCalculator
{
protected:
	static float m_fTable[DA_MAX][ARMOR_ATT_MAX];

	DAMAGE_ATTRIB GetItemDamageAttrib(WEAPON_TYPE nWeaponType);

	float CalcLevelDiffFactor(int nLevelFactor);
	float CalcAPFactor(const GEntityActor* pAttacker, const GEntityActor* pVictim);
	float CalcAPFactor(int nAttackerLevel, int nVictimLevel, int nVictimAP, bool bVictimIsPlayer);
	float _CalcAPFactor(int nAP, int nAttackerLevel, float fLevelDiffFactor, float fDefenseMaxConst);
	float CalcAttributeFactor(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	float CalcAttributeFactor_Player(ARMOR_ATTRIB nArmorAttr, WEAPON_TYPE nWeaponType);
	float CalcAttributeFactor_Player(ARMOR_ATTRIB nArmorAttr, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType);
	float CalcAttributeFactor_ForBuff(const GEntityActor* pVictim, DAMAGE_ATTRIB nDamageAttrib);
public:
	float CalcDefenseFactor(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GTalentInfo* pTalentInfo);
	float CalcDefenseFactor_ForBuff(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GBuffInfo* pBuffInfo);
};

