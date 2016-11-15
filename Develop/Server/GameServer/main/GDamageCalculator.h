#ifndef _GDAMAGE_CALCULATOR_H
#define _GDAMAGE_CALCULATOR_H

#include "GDef.h"
#include "GMath.h"
#include "GDamageMODTable.h"
#include "CSItemData.h"

class GDamageCalculator
{
private:
	GPlayerDMODTable		m_PlayerDMODTable;
	GNPC_DMODTable			m_NPC_DMODTable;
public:
	// DamageOutput_Player
	inline float CalcFactorTalentWeapon();
	inline float CalcFactorTalentMagic();
	inline int CalcDamage_Character_Weapon(WEAPON_TYPE nWeaponType, int nSTR, int nDEX);
	inline int CalcDamage_Character_Magic(int nINT);
	inline float CalcARTalent(int nTalentRank_CombatSpecialization, int nTalentRank_WeaponSpecialization);

	// DamageAbsorb_Player
	inline float CalcDFTalent(int nTalentRank_Defense, int nTalentRank_Armor);
	inline int CalcDFCharacter(int nCON, int nCHA);
	inline float CalcPlayer_DMOD(int nPCLevel, int nNPCLevel);
	inline float CalcNPC_DMOD(int nPCLevel, int nNPCLevel);
};




inline float GDamageCalculator::CalcFactorTalentWeapon()
{
	return 1.0f;
}
inline float GDamageCalculator::CalcFactorTalentMagic()
{
	return 2.5f;
}
inline int GDamageCalculator::CalcDamage_Character_Weapon(WEAPON_TYPE nWeaponType, int nSTR, int nDEX)
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH: return GMath::TruncateToInt(nSTR * 0.5);
	case WEAPON_1H_BLUNT: return GMath::TruncateToInt(nSTR * 0.5);
	case WEAPON_1H_PIERCE: return GMath::TruncateToInt(nSTR * 0.2 + nDEX * 0.3);
	case WEAPON_TWO_HANDED: return GMath::TruncateToInt(nSTR * 0.6);
	case WEAPON_STAFF: return GMath::TruncateToInt(nSTR * 0.6);
	case WEAPON_ARCHERY: return GMath::TruncateToInt(nSTR * 0.1 + nDEX* 0.4);
	case WEAPON_DUAL_WIELD: return GMath::TruncateToInt(nSTR * 0.5);
	}

	return 0;
}
inline int GDamageCalculator::CalcDamage_Character_Magic(int nINT)
{
	return GMath::TruncateToInt(nINT * 0.5);
}
inline float GDamageCalculator::CalcARTalent(int nTalentRank_CombatSpecialization, int nTalentRank_WeaponSpecialization)
{
	return (0.75f + (nTalentRank_CombatSpecialization * 0.05f) + (nTalentRank_WeaponSpecialization * 0.1f));
}

// DamageAbsorb_Player
inline float GDamageCalculator::CalcDFTalent(int nTalentRank_Defense, int nTalentRank_Armor)
{
	return float(0.5 + 0.05 * nTalentRank_Defense + 0.1 * nTalentRank_Armor);
}

inline int GDamageCalculator::CalcDFCharacter(int nCON, int nCHA)
{
	return (int)(nCON * 0.05 + nCHA * 0.02);
}
inline float GDamageCalculator::CalcPlayer_DMOD(int nPCLevel, int nNPCLevel)
{
	return m_PlayerDMODTable.GetValue(nPCLevel, nNPCLevel);
}
inline float GDamageCalculator::CalcNPC_DMOD(int nPCLevel, int nNPCLevel)
{
	return m_NPC_DMODTable.GetValue(nPCLevel, nNPCLevel);
}

#endif