#include "stdafx.h"
#include "GDefenseFactorCalculator.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GMath.h"
#include "GCalculator.h"
#include "GBuff.h"
#include "GTalentInfo.h"
#include "GItemHolder.h"
#include "GBuffInfo.h"
#include "GEquipmentSlot.h"

float GDefenseFactorCalculator::m_fTable[DA_MAX][ARMOR_ATT_MAX] = {
	//						   lightning				 stone
	{1.0f,1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,1.0f,1.0f},	// none
	{1.0f,0.8f,0.8f,0.9f,0.9f, 0.9f,0.9f,0.9f,0.9f,1.0f, 1.0f,1.2f,1.1f,0.9f,1.2f},	// slash
	{1.0f,0.8f,0.8f,0.9f,0.9f, 0.9f,0.9f,0.8f,1.1f,1.2f, 1.3f,0.8f,0.9f,1.2f,0.9f},	// blunt
	{1.0f,0.8f,0.8f,0.9f,0.9f, 0.9f,0.9f,0.9f,0.9f,0.9f, 0.9f,1.2f,1.2f,0.9f,1.3f},	// pierce
	{1.0f,0.8f,0.8f,0.9f,0.9f, 0.9f,0.9f,0.9f,1.1f,1.0f, 1.2f,1.0f,1.0f,1.0f,1.0f},	// explosion
	{1.0f,0.8f,0.8f,0.9f,0.9f, 0.9f,0.9f,0.9f,0.9f,0.9f, 0.9f,1.2f,1.2f,0.9f,1.3f},	// inertia

	{1.0f,0.8f,1.1f,1.0f,1.0f, 1.0f,1.0f,0.5f,1.3f,0.9f, 0.9f,0.9f,0.8f,1.2f,1.0f},	// holy
	{1.0f,1.1f,0.8f,1.0f,1.0f, 1.0f,1.0f,1.3f,0.5f,0.9f, 0.9f,0.9f,1.2f,0.8f,1.0f},	// unholy
	{1.0f,0.8f,0.8f,0.5f,1.3f, 1.0f,1.0f,1.0f,1.0f,1.0f, 0.7f,1.1f,1.1f,1.0f,1.1f},	// fire
	{1.0f,0.8f,0.8f,1.3f,0.5f, 1.0f,1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,1.0f,1.0f},	// cold
	{1.0f,0.8f,0.8f,1.0f,1.0f, 0.5f,1.3f,1.0f,1.0f,1.1f, 0.8f,1.1f,1.1f,1.0f,1.0f},	// lightning
	{1.0f,0.8f,0.8f,1.0f,1.0f, 1.3f,0.5f,1.0f,1.0f,0.8f, 0.8f,1.2f,1.2f,1.0f,1.1f}	// poison

};

DAMAGE_ATTRIB GDefenseFactorCalculator::GetItemDamageAttrib( WEAPON_TYPE nWeaponType )
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH: return DA_SLASH;
	case WEAPON_1H_BLUNT: return DA_BLUNT;
	case WEAPON_1H_PIERCE: return DA_PIERCE;
	case WEAPON_TWO_HANDED: return DA_SLASH;
	case WEAPON_STAFF: return DA_BLUNT;
	case WEAPON_ARCHERY: return DA_SLASH;
	}

	return DA_NONE;
}

float GDefenseFactorCalculator::CalcAPFactor(const GEntityActor* pAttacker, const GEntityActor* pVictim)
{

	return CalcAPFactor(pAttacker->GetLevel(), 
						pVictim->GetLevel(), 
						pVictim->GetAP(), 
						pVictim->IsPlayer());
}

float GDefenseFactorCalculator::CalcAPFactor(int nAttackerLevel, int nVictimLevel, int nVictimAP, bool bVictimIsPlayer)
{
	// DefenseFactor는 방어자 입장에서 레벨 펙터를 구한다.
	int nLevelFactor = GCalculator::CalcLevelFactor(nVictimLevel, nAttackerLevel);
	float fLevelDiffFactor = CalcLevelDiffFactor(nLevelFactor);

	float fDefenseMaxConst = 0.0f;
	if (bVictimIsPlayer) 
		fDefenseMaxConst = 0.2f;

	return _CalcAPFactor(nVictimAP, nAttackerLevel, fLevelDiffFactor, fDefenseMaxConst);
}


float GDefenseFactorCalculator::_CalcAPFactor( int nAP, int nAttackerLevel, float fLevelDiffFactor, float fDefenseMaxConst )
{
	float fAPFactor = max(GMath::Round4Combat((1.0f - nAP / float(nAP + nAttackerLevel * 200)) * fLevelDiffFactor, 2), fDefenseMaxConst);

	return fAPFactor;
}


float GDefenseFactorCalculator::CalcAttributeFactor_ForBuff(const GEntityActor* pVictim, DAMAGE_ATTRIB nDamageAttrib)
{
	// 부여 속성에 의한 수정치
	return m_fTable[nDamageAttrib][pVictim->GetArmorAttrib()];	
}

float GDefenseFactorCalculator::CalcAttributeFactor(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GTalentInfo* pTalentInfo)
{
	DAMAGE_ATTRIB	nEnchantDamageAttrib = pAttacker->GetEnchantAttrib();
	ARMOR_ATTRIB	nArmorAttr = pVictim->GetArmorAttrib();

	// 부여 속성에 의한 수정치
	float fEnchantAttribFactor = m_fTable[nEnchantDamageAttrib][nArmorAttr];	

	// 무기 속성에 의한 수정치
	float fWeaponAttribFactor = 0.0f;
	if (false == pAttacker->IsPlayer())
	{
		fWeaponAttribFactor = 1.0f;
	}
	else
	{
		const GEntityPlayer* pAttackerPlayer = static_cast<const GEntityPlayer*>(pAttacker);		
		const GItemHolder* pAttackerItemHolder = pAttackerPlayer->GetItemHolder();
		switch (pTalentInfo->m_WeaponReference)
		{
		case WR_LEFT:
			{
				WEAPON_TYPE nLeftWeaponType = pAttackerItemHolder->GetEquipment().GetLeftWeaponType();
				fWeaponAttribFactor = CalcAttributeFactor_Player(nArmorAttr, nLeftWeaponType);				
			}
			break;
		case WR_RIGHT:
			{
				WEAPON_TYPE nRightWeaponType = pAttackerItemHolder->GetEquipment().GetRightWeaponType();
				fWeaponAttribFactor = CalcAttributeFactor_Player(nArmorAttr, nRightWeaponType);
			}
			break;
		case WR_BOTH:
			{
				WEAPON_TYPE nLeftWeaponType = pAttackerItemHolder->GetEquipment().GetLeftWeaponType();
				WEAPON_TYPE nRightWeaponType = pAttackerItemHolder->GetEquipment().GetRightWeaponType();
				fWeaponAttribFactor = CalcAttributeFactor_Player(nArmorAttr, nLeftWeaponType, nRightWeaponType);
			}
			break;
		default:
			{
				_ASSERT(0);
			}
		}
	}

	float fFinalAttrFactor = fEnchantAttribFactor * fWeaponAttribFactor;
	return fFinalAttrFactor;
}

float GDefenseFactorCalculator::CalcAttributeFactor_Player(ARMOR_ATTRIB nArmorAttr, WEAPON_TYPE nWeaponType)
{
	DAMAGE_ATTRIB nWeaponDamageAttrib = GetItemDamageAttrib(nWeaponType);
	float fWeaponAttribFactor = m_fTable[nWeaponDamageAttrib][nArmorAttr];

	return fWeaponAttribFactor;
}

float GDefenseFactorCalculator::CalcAttributeFactor_Player(ARMOR_ATTRIB nArmorAttr, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType)
{
	float fLeftWeaponAttribFactor = CalcAttributeFactor_Player(nArmorAttr, nLeftWeaponType);	
	float fRightWeaponAttribFactor = CalcAttributeFactor_Player(nArmorAttr, nRightWeaponType);
	float fWeaponAttribFactor = (fLeftWeaponAttribFactor + fRightWeaponAttribFactor) * 0.5f;

	return fWeaponAttribFactor;
}

// http://iworks2.maiet.net/wiki/sh:formulas:defensefactor
float GDefenseFactorCalculator::CalcDefenseFactor(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GTalentInfo* pTalentInfo)
{
	// AP에 의한 데미지 수정치
	float fAPFactor = CalcAPFactor(pAttacker, pVictim);

	// 속성에 의한 데미지 수정치
	float fAttrFactor = CalcAttributeFactor(pAttacker, pVictim, pTalentInfo);

	float fDefenseFactor = fAPFactor * fAttrFactor;
	return fDefenseFactor;
}

float GDefenseFactorCalculator::CalcDefenseFactor_ForBuff(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GBuffInfo* pBuffInfo)
{
	VALID_RET(pVictim, 0.0f);
	VALID_RET(pBuffInfo, 0.0f);
	
	// AP에 의한 데미지 수정치
	int nAttackLevel = pAttacker?pAttacker->GetLevel():pVictim->GetLevel();
	float fAPFactor = CalcAPFactor(nAttackLevel, pVictim->GetLevel(), pVictim->GetAP(), pVictim->IsPlayer());

	// 속성에 의한 데미지 수정치
	float fAttrFactor = CalcAttributeFactor_ForBuff(pVictim, pBuffInfo->m_nDamageAttrib);

	float fDefenseFactor = fAPFactor * fAttrFactor;
	return fDefenseFactor;
}

float GDefenseFactorCalculator::CalcLevelDiffFactor( int nLevelFactor )
{
	float fLevelDiffFactor = 1.0f;

	if (nLevelFactor >= 5)
	{
		fLevelDiffFactor = (nLevelFactor - 5) * 0.1f + 1.0f;
	}
	else
	{
		fLevelDiffFactor = (nLevelFactor - 5) * 0.05f + 1.0f;
	}
	return fLevelDiffFactor;
}

