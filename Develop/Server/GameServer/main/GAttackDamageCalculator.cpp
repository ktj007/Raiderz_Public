#include "stdafx.h"
#include "GAttackDamageCalculator.h"
#include "GCombatCalculator.h"
#include "GCriticalCalculator.h"
#include "GItemHolder.h"
#include "GConst.h"
#include "GEquipmentSlot.h"
#include "GGlobal.h"
#include "GItemEquiper.h"

/// GAttackDamageCalculator
///////////////////////////////////////
float GAttackDamageCalculator::CalcAttackDamage( GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, const GDamageRangedInfo& DamageInfo )
{
	int nTalentDamage = 0;			// 탤런트 데미지
	int nWeaponDamage = 0;			// 무기 데미지
	float fWeaponApplyRate = pTalentInfo->m_WeaponApplyRate.fApplyRate;		// 무기 데미지 적용율

	float fTalentFactor1 = 1.0f;	// 탤런트 보정율1
	float fTalentFactor2 = 1.0f;	// 탤런트 보정율267

	float fArmorFactor1 = 1.0f;		// 방어구 보정율1
	float fArmorFactor2 = 1.0f;		// 방어구 보정율2

	float fCharacterFactor = 1.0f;	// 스탯 보정율
	float fBuffPlusFactor = 0.0f;		// 버프에 의한 수정율
	float fBuffDefenceFactor = 1.0f;	// 버프에 의한 방어율
	float fCriticalFactor = 1.0f;	// 크리티컬 배율

	float fSpellPower = 1.0f;		// 주문 증폭력


	if (pAttacker->IsPlayer())
	{
		GEntityPlayer* pPlayerAttacker = static_cast< GEntityPlayer*>(pAttacker);

		// 무기 데미지
		nWeaponDamage = CalcPlayerWeaponDamage(pPlayerAttacker, pTalentInfo->m_nDamageType, pTalentInfo->m_WeaponReference);

		// 순수 마법일 경우
		if (pTalentInfo->IsMagicDamage())
		{
			// 탤런트 보정율1
			fTalentFactor1 = CalcTalentFactor1(pAttacker, pTalentInfo->m_nDamageAttrib, fWeaponApplyRate);

			// 방어구 보정율1
			fArmorFactor1 = CalcArmorFactor1(pAttacker, pTalentInfo->m_nDamageAttrib, fWeaponApplyRate);

			// 주문 증폭력
			fSpellPower = pPlayerAttacker->GetSpellPower();
		}
		else
		{
			// 탤런트 보정율2
			fTalentFactor2 = CalcTalentFactor2(pPlayerAttacker, pTalentInfo->m_nDamageType, pTalentInfo->m_WeaponReference);

			fArmorFactor2 = CalcArmorFactor2(pAttacker, pTalentInfo->m_WeaponApplyRate.fApplyRate, pTalentInfo->m_WeaponReference);
		}

		// 스탯 보정율
		fCharacterFactor = CalcCharacterFactor(pPlayerAttacker, pTalentInfo->m_nDamageType, pTalentInfo->m_WeaponReference);
	}
	else if (pAttacker->IsNPC())
	{
		GEntityNPC* pNPCAttacker = static_cast< GEntityNPC*>(pAttacker);

		// 무기 데미지
		nWeaponDamage = CalcNPCWeaponDamage(pNPCAttacker->GetNPCInfo());

		// 순수 마법일 경우
		if (pTalentInfo->IsMagicDamage())
		{
			// 주문 증폭력
			fSpellPower = pNPCAttacker->GetSpellPower();
		}
	}


	// 버프
	fBuffPlusFactor = CalcBuffPlusFactor(pAttacker, pTalentInfo->m_nDamageType);

	// 버프 방어율
	fBuffDefenceFactor = CalcBuffDefenceFactor(pAttacker, pVictim, pTalentInfo->m_nDamageAttrib, pTalentInfo->m_WeaponReference);


	// 크리티컬 배율
	if (bCritical)
	{
		fCriticalFactor = m_pCriticalCalculator->CalcCriticalDamageFactor(pAttacker, pTalentInfo->m_nDamageAttrib, pTalentInfo->m_nDamageType, pTalentInfo->m_nSkillType);
	}

	nTalentDamage = CalcTalentDamage(DamageInfo);

	_ASSERT(fTalentFactor1 < 10.0f);
	_ASSERT(fArmorFactor1 < 10.0f);
	_ASSERT(fTalentFactor2 < 10.0f);
	_ASSERT(fArmorFactor2 < 10.0f);
	_ASSERT(fWeaponApplyRate < 10.0f);
	
	float fFinalDamage = GMath::TruncateToInt(nTalentDamage * fTalentFactor1 * fArmorFactor1 + 
											  nWeaponDamage * fWeaponApplyRate * fTalentFactor2 * fArmorFactor2) 
						 * (fCharacterFactor + fBuffPlusFactor) 
						 * fCriticalFactor
						 * fBuffDefenceFactor
						 * fSpellPower;

	return fFinalDamage;
}

/*
float GAttackDamageCalculator::CalcBuffAttackDamage( GEntityActor* pAttacker, GEntityActor* pVictim, const GBuffInfo* pBuffInfo, bool bCritical, const GDamageRangedInfo& DamageInfo )
{
	int nTalentDamage = 0;
	int nWeaponDamage = 0;
	float fWeaponApplyRate = pBuffInfo->m_WeaponApplyRate.fApplyRate;

	float fTalentFactor1 = 1.0f;
	float fTalentFactor2 = 1.0f;

	float fArmorFactor1 = 1.0f;
	float fArmorFactor2 = 1.0f;

	float fCharacterFactor = 1.0f;
	float fBuffPlusFactor = 0.0f;
	float fBuffDefenceFactor = 1.0f;
	float fCriticalFactor = 1.0f;

	float fSpellPower = 1.0f;

	if (pAttacker)
	{
		if (pAttacker->IsPlayer())
		{
			GEntityPlayer* pPlayerAttacker = static_cast<GEntityPlayer*>(pAttacker);
			nWeaponDamage = CalcPlayerWeaponDamage(pPlayerAttacker, pBuffInfo->m_nDamageType);

			if (pBuffInfo->IsMagicDamage())
			{
				fTalentFactor1 = CalcTalentFactor1(pAttacker, pBuffInfo->m_nDamageAttrib, fWeaponApplyRate);
				fArmorFactor1 = CalcArmorFactor1(pAttacker, pBuffInfo->m_nDamageAttrib, fWeaponApplyRate);
				fSpellPower = pPlayerAttacker->GetSpellPower();
			}
			else
			{
				fTalentFactor2 = CalcTalentFactor2(pPlayerAttacker, pBuffInfo->m_nDamageType);
				fArmorFactor2 = CalcArmorFactor2(pAttacker, pBuffInfo->m_WeaponApplyRate.fApplyRate);
			}

			fCharacterFactor = CalcCharacterFactor(pPlayerAttacker, pBuffInfo->m_nDamageType);
		}
		else if (pAttacker->IsNPC())
		{
			GEntityNPC* pNPCAttacker = static_cast<GEntityNPC*>(pAttacker);
			nWeaponDamage = CalcNPCWeaponDamage(pNPCAttacker->GetNPCInfo());

			if (pBuffInfo->IsMagicDamage())
			{
				fSpellPower = pNPCAttacker->GetSpellPower();
			}
		}

		fBuffPlusFactor = CalcBuffPlusFactor(pAttacker, pBuffInfo->m_nDamageType);
		fBuffDefenceFactor = CalcBuffDefenceFactor(pAttacker, pVictim, pBuffInfo->m_nDamageAttrib);

		if (bCritical)
		{
			fCriticalFactor = m_pCriticalCalculator->CalcCriticalDamageFactor(pAttacker, pBuffInfo->m_nDamageAttrib, pBuffInfo->m_nDamageType, ST_NONE);
		}
	}

	nTalentDamage = CalcTalentDamage(DamageInfo);

	_ASSERT(fTalentFactor1 < 10.0f);
	_ASSERT(fArmorFactor1 < 10.0f);
	_ASSERT(fTalentFactor2 < 10.0f);
	_ASSERT(fArmorFactor2 < 10.0f);
	_ASSERT(fWeaponApplyRate < 10.0f);

	float fFinalDamage = GMath::TruncateToInt(nTalentDamage * fTalentFactor1 * fArmorFactor1 +
		nWeaponDamage * fWeaponApplyRate * fTalentFactor2 * fArmorFactor2)
		* (fCharacterFactor + fBuffPlusFactor)
		* fCriticalFactor
		* fBuffDefenceFactor
		* fSpellPower;

	return fFinalDamage;
}
*/

bool GAttackDamageCalculator::CalcBuffRangedDamage(GEntityActor* pAttacker, GEntityActor* pVictim, const GBuffInfo* pBuffInfo, GDamageRangedInfo* poutDamageRangedInfo, GHealRangedInfo* poutHealRangedInfo, float* pfoutCriticalFactor)
{
	int nWeaponMinDamage = 0;
	int nWeaponMaxDamage = 0;
	float fWeaponApplyRate = pBuffInfo->m_WeaponApplyRate.fApplyRate;

	float fTalentFactor1 = 1.0f;
	float fTalentFactor2 = 1.0f;

	float fArmorFactor1 = 1.0f;
	float fArmorFactor2 = 1.0f;

	float fCharacterFactor = 1.0f;
	float fBuffPlusFactor = 0.0f;
	float fBuffDefenceFactor = 1.0f;
	float fCriticalFactor = 1.0f;

	float fSpellPower = 1.0f;

	if (pAttacker)
	{
		if (pAttacker->IsPlayer())
		{
			GEntityPlayer* pPlayerAttacker = static_cast<GEntityPlayer*>(pAttacker);

			nWeaponMinDamage = CalcPlayerWeaponDamage(pPlayerAttacker, pBuffInfo->m_nDamageType, WR_RIGHT, CTWD_MIN);
			nWeaponMaxDamage = CalcPlayerWeaponDamage(pPlayerAttacker, pBuffInfo->m_nDamageType, WR_RIGHT, CTWD_MAX);

			if (pBuffInfo->IsMagicDamage())
			{
				fTalentFactor1 = CalcTalentFactor1(pAttacker, pBuffInfo->m_nDamageAttrib, fWeaponApplyRate);
				fArmorFactor1 = CalcArmorFactor1(pAttacker, pBuffInfo->m_nDamageAttrib, fWeaponApplyRate);
				fSpellPower = pPlayerAttacker->GetSpellPower();
			}
			else
			{
				fTalentFactor2 = CalcTalentFactor2(pPlayerAttacker, pBuffInfo->m_nDamageType);
				fArmorFactor2 = CalcArmorFactor2(pAttacker, pBuffInfo->m_WeaponApplyRate.fApplyRate);
			}

			fCharacterFactor = CalcCharacterFactor(pPlayerAttacker, pBuffInfo->m_nDamageType);
		}
		else if (pAttacker->IsNPC())
		{
			GEntityNPC* pNPCAttacker = static_cast<GEntityNPC*>(pAttacker);

			nWeaponMinDamage = CalcNPCWeaponDamage(pNPCAttacker->GetNPCInfo(), CTWD_MIN);
			nWeaponMaxDamage = CalcNPCWeaponDamage(pNPCAttacker->GetNPCInfo(), CTWD_MAX);

			if (pBuffInfo->IsMagicDamage())
			{
				fSpellPower = pNPCAttacker->GetSpellPower();
			}
		}

		fBuffPlusFactor = CalcBuffPlusFactor(pAttacker, pBuffInfo->m_nDamageType);
		fBuffDefenceFactor = CalcBuffDefenceFactor(pAttacker, pVictim, pBuffInfo->m_nDamageAttrib);

		fCriticalFactor = m_pCriticalCalculator->CalcCriticalDamageFactor(pAttacker, pBuffInfo->m_nDamageAttrib, pBuffInfo->m_nDamageType, ST_NONE);
	}

	_ASSERT(fTalentFactor1 < 10.0f);
	_ASSERT(fArmorFactor1 < 10.0f);
	_ASSERT(fTalentFactor2 < 10.0f);
	_ASSERT(fArmorFactor2 < 10.0f);
	_ASSERT(fWeaponApplyRate < 10.0f);

	int nFinalMinDamage = CalcBuffFinalDamage(pBuffInfo->m_nMinDamage, fTalentFactor1, fArmorFactor1, nWeaponMinDamage, fWeaponApplyRate, fTalentFactor2, fArmorFactor2, fCharacterFactor, fBuffPlusFactor, fBuffDefenceFactor, fSpellPower);
	int nFinalMaxDamage = CalcBuffFinalDamage(pBuffInfo->m_nMaxDamage, fTalentFactor1, fArmorFactor1, nWeaponMaxDamage, fWeaponApplyRate, fTalentFactor2, fArmorFactor2, fCharacterFactor, fBuffPlusFactor, fBuffDefenceFactor, fSpellPower);
	int nFinalMinHeal = CalcBuffFinalDamage(pBuffInfo->m_nMinHeal, fTalentFactor1, fArmorFactor1, nWeaponMinDamage, fWeaponApplyRate, fTalentFactor2, fArmorFactor2, fCharacterFactor, fBuffPlusFactor, fBuffDefenceFactor, fSpellPower);
	int nFinalMaxHeal = CalcBuffFinalDamage(pBuffInfo->m_nMaxHeal, fTalentFactor1, fArmorFactor1, nWeaponMaxDamage, fWeaponApplyRate, fTalentFactor2, fArmorFactor2, fCharacterFactor, fBuffPlusFactor, fBuffDefenceFactor, fSpellPower);

	if (poutDamageRangedInfo)
		(*poutDamageRangedInfo) = GDamageRangedInfo(nFinalMaxDamage, nFinalMinDamage);

	if (poutHealRangedInfo)
		(*poutHealRangedInfo) = GHealRangedInfo(nFinalMaxHeal, nFinalMinHeal);

	if (pfoutCriticalFactor)
		(*pfoutCriticalFactor) = fCriticalFactor;

	return true;
}

int GAttackDamageCalculator::CalcBuffFinalDamage(int nDamage, float fTalentFactor1, float fArmorFactor1, int nWeaponDamage, float fWeaponApplyRate, float fTalentFactor2, float fArmorFactor2, float fCharacterFactor, float fBuffPlusFactor, float fBuffDefenceFactor, float fSpellPower)
{
	float fFinalDamage = GMath::TruncateToInt(nDamage * fTalentFactor1 * fArmorFactor1 +
		nWeaponDamage * fWeaponApplyRate * fTalentFactor2 * fArmorFactor2)
		* (fCharacterFactor + fBuffPlusFactor)
		* fBuffDefenceFactor
		* fSpellPower;

	return static_cast<int>(fFinalDamage);
}

int GAttackDamageCalculator::MakeDamageFromMinMaxDamage( const GDamageRangedInfo& DamageInfo )
{
	return (GMath::RandomNumber(DamageInfo.nMinDamage, DamageInfo.nMaxDamage));
}

float GAttackDamageCalculator::CalcCharacterFactor( GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE nDamageType, WEAPON_REFRENCE nWeaponReference /*= WR_RIGHT*/ )
{
	ATTACK_TYPE_FOR_CALCULATE attackType = CalcAttackTypeForCalculate(pAttacker, nDamageType, nWeaponReference);

	switch(attackType)	
	{
	case ATC_STR_WEAPON_ATTACK:		return CalcCharacterFactorForMelee(pAttacker);
	case ATC_DEX_WEAPON_ATTACK:		return CalcCharacterFactorForRange(pAttacker);
	case ATC_MAGIC_ATTACK:			return CalcCharacterFactorForMagic(pAttacker);
	};

	return CalcCharacterFactorForMelee(pAttacker);
}

float GAttackDamageCalculator::CalcBuffPlusFactor( GEntityActor* pAttacker, TALENT_DAMAGE_TYPE nDamageType )
{
	float fBuffFactor = 0.0f;

	if (nDamageType == TDT_PHYSIC)
	{
		fBuffFactor += pAttacker->GetChrStatus()->ActorModifier.fPhysicDamageAmp;
	}
	else if (nDamageType == TDT_MAGIC)
	{
		fBuffFactor += pAttacker->GetChrStatus()->ActorModifier.fMagicDamageAmp;
	}

	return fBuffFactor;
}

int GAttackDamageCalculator::CalcPlayerWeaponDamage(GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE damageType/* = TDT_PHYSIC*/, WEAPON_REFRENCE weaponReference/* = WR_RIGHT*/, CALC_TYPE_FOR_WEAPON_DAMAGE calcType/* = CTWD_MINMAX*/)
{
	int nDamage = 0;
	GItemHolder* pItemHolder = pAttacker->GetItemHolder();

	/*
	switch (weaponReference)
	{
	case WR_LEFT:
		{
			const GItem* pLeftItem = pItemHolder->GetEquipment().GetLeftWeapon();
			if (pLeftItem)
			{
				nDamage = doCalcWeaponDamage(pAttacker, pLeftItem, bCritical);
			}
			else
			{
				nDamage = GConst::NO_WEAPON_DAMAGE_LEFT;
			}
		}
		break;
	case WR_RIGHT:
		{
			const GItem* pRightItem = pItemHolder->GetEquipment().GetRightWeapon();
			if (pRightItem)
			{
				nDamage = doCalcWeaponDamage(pAttacker, pRightItem, bCritical);
			}
			else
			{
				nDamage = GConst::NO_WEAPON_DAMAGE_RIGHT;	
			}
		}
		break;
	case WR_BOTH:
		{
			const GItem* pLeftItem = pItemHolder->GetEquipment().GetLeftWeapon();
			const GItem* pRightItem = pItemHolder->GetEquipment().GetRightWeapon();
			nDamage = doCalcWeaponDamage(pAttacker, pLeftItem, pRightItem, bCritical);

			if (pLeftItem == NULL)
			{
				nDamage += GConst::NO_WEAPON_DAMAGE_LEFT;
			}
			if (pRightItem == NULL)
			{
				nDamage += GConst::NO_WEAPON_DAMAGE_RIGHT;
			}
		}
		break;
	default:
		{
			_ASSERT(0);
		}
	}
	*/

	// all damages are now should based on right weapon.
	// even in the case of shield attack of defender (e.g. Rush, Stunning Smash etc) we have to refer right weapon damage.
	// (and you still can use those skills with 1H sword and without shield.)
	const GItem* pRightItem = pItemHolder->GetEquipment().GetRightWeapon();
	if (pRightItem)
	{
		nDamage = doCalcWeaponDamage(pAttacker, pRightItem, damageType, calcType);
	}
	else
	{
		nDamage = GConst::NO_WEAPON_DAMAGE_RIGHT;
	}

	return nDamage;
}

int GAttackDamageCalculator::doCalcWeaponDamage(GEntityActor* pActor, const GItem* pWeaponItem, TALENT_DAMAGE_TYPE damageType, CALC_TYPE_FOR_WEAPON_DAMAGE calcType/* = CTWD_MINMAX*/)
{
	if (!pWeaponItem)
		return 0;

	GItemData* pItemData = pWeaponItem->m_pItemData;
	VALID_RET(pItemData, 0);

	//int nWeaponDamage = //bCritical?  pItemData->m_nMaxDamage : 
	//								GMath::RandomNumber(pItemData->m_nMinDamage, pItemData->m_nMaxDamage);
	int nWeaponDamage = 0;
	if (damageType == TDT_PHYSIC)
	{
		switch (calcType)
		{
		case CTWD_MINMAX:
		default:
			nWeaponDamage = GMath::RandomNumber(pItemData->m_nMinDamage, pItemData->m_nMaxDamage); break;
		case CTWD_MIN: nWeaponDamage = pItemData->m_nMinDamage; break;
		case CTWD_MAX: nWeaponDamage = pItemData->m_nMaxDamage; break;
		}
	}
	else if (damageType == TDT_MAGIC)
	{
		switch (calcType)
		{
		case CTWD_MINMAX:
		default:
			nWeaponDamage = GMath::RandomNumber(pItemData->m_nMagicMinDamage, pItemData->m_nMagicMaxDamage); break;
		case CTWD_MIN: nWeaponDamage = pItemData->m_nMagicMinDamage; break;
		case CTWD_MAX: nWeaponDamage = pItemData->m_nMagicMaxDamage; break;
		}
	}

	if (!pWeaponItem->IsFineDuration() || !HasWeaponPassiveTalent(pActor, pWeaponItem))
	{
		return static_cast<int>(nWeaponDamage * 0.2f);
	}	

	return nWeaponDamage;
}

int GAttackDamageCalculator::doCalcWeaponDamage(GEntityActor* pActor, const GItem* pLeftWeaponItem, const GItem* pRightWeaponItem, TALENT_DAMAGE_TYPE damageType, CALC_TYPE_FOR_WEAPON_DAMAGE calcType/* = CTWD_MINMAX*/)
{
	int nLeftDamage = doCalcWeaponDamage(pActor, pLeftWeaponItem, damageType, calcType);
	int nRightDamage = doCalcWeaponDamage(pActor, pRightWeaponItem, damageType, calcType);

	int nDamage = static_cast<int>((nLeftDamage + nRightDamage) * 0.5f);

	return nDamage;
}

float GAttackDamageCalculator::CalcTalentFactor1( GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRate )
{
	const float fFactorDefault = 1.0f;

	if (!pAttacker)
		return fFactorDefault;

	if (!pAttacker->IsPlayer())
		return fFactorDefault;

	GEntityPlayer* pPlayerAttacker = static_cast< GEntityPlayer*>(pAttacker);
	return CalcPlayerTalentFactor1(pPlayerAttacker, nDamageAttrib, fWeaponApplyRate);
}

float GAttackDamageCalculator::CalcPlayerTalentFactor1( GEntityPlayer* pPlayerAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRate )
{
	float fFactor = 1.0f;

	// 탤런트 보정율1은 순수 마법에 한정하여 적용한다.
	if (fWeaponApplyRate < 0.001f)
	{
		switch (nDamageAttrib)
		{
		case DA_FIRE:
		case DA_LIGHTNING:
		case DA_COLD:
		case DA_POISON:
		case DA_UNHOLY:
			{
				// Factor1_talent = 1 + 마법 숙련 랭크 * 0.04 + 공격마법 특화 * 0.06
				int nBaseRank = pPlayerAttacker->GetPassiveRank(TEPT_BASE_ELEMENTAL);
				int nSpecialization = pPlayerAttacker->GetPassiveRank(TEPT_SPECIALIZATION_ELEMENTAL);

				fFactor = 1.0f + (nBaseRank * 0.04f) + (nSpecialization * 0.06f);
			}
			break;
		case DA_HOLY:
			{
				// Factor1_talent = 1 + 신성 숙련 랭크 * 0.04 + 신성 특화 * 0.06
				int nBaseRank = pPlayerAttacker->GetPassiveRank(TEPT_BASE_HOLY);
				int nSpecialization = pPlayerAttacker->GetPassiveRank(TEPT_SPECIALIZATION_HOLY);

				fFactor = 1.0f + (nBaseRank * 0.04f) + (nSpecialization * 0.06f);
			}
			break;
		}
	}
	return fFactor;
}

float GAttackDamageCalculator::CalcTalentFactor2( GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE nDamageType, WEAPON_REFRENCE nWeaponReference /*= WR_RIGHT*/ )
{
	float fCalcTalentFactor2 = 0.0f;
	GItemHolder* pItemHolder = pAttacker->GetItemHolder();

	if (nDamageType == TDT_MAGIC)
	{
		fCalcTalentFactor2 = 1.0f;
	}
	else
	{
		switch (nWeaponReference) 
		{
		case WR_LEFT:
			{
				WEAPON_TYPE nLeftWeaponType = pItemHolder->GetEquipment().GetLeftWeaponType();
				fCalcTalentFactor2 = CalcTalentFactor2_NotMagic(pAttacker, nLeftWeaponType);
			}
			break;
		case WR_RIGHT:
			{
				WEAPON_TYPE nRightWeaponType = pItemHolder->GetEquipment().GetRightWeaponType();
				fCalcTalentFactor2 = CalcTalentFactor2_NotMagic(pAttacker, nRightWeaponType);
			}
			break;
		case WR_BOTH:
			{
				WEAPON_TYPE nLeftWeaponType = pItemHolder->GetEquipment().GetLeftWeaponType();
				WEAPON_TYPE nRightWeaponType = pItemHolder->GetEquipment().GetRightWeaponType();
				fCalcTalentFactor2 = CalcTalentFactor2_NotMagic(pAttacker, nLeftWeaponType, nRightWeaponType);
			}
			break;
		default:
			{
				_ASSERT(0);
			}
		}		
	}

	return fCalcTalentFactor2;
}

float GAttackDamageCalculator::CalcTalentFactor2_NotMagic(GEntityPlayer* pAttacker, WEAPON_TYPE nWeaponType)
{
	int nBaseRank = 0;
	int nSpecializationRank = 0;

	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_ONE_HANDED);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_SLASH);
		}
		break;
	case WEAPON_1H_BLUNT:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_ONE_HANDED);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_BLUNT);
		}
		break;
	case WEAPON_1H_PIERCE:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_1H_PIERCE);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_PIERCE);
		}
		break;
	case WEAPON_TWO_HANDED:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_TWO_HANDED);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_SLASH);
		}
		break;
	case WEAPON_2H_BLUNT:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_2H_BLUNT);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_BLUNT);
		}
		break;
	case WEAPON_ARCHERY:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_ARCHERY);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_PIERCE);
		}
		break;
	case WEAPON_DUAL_PIERCE:
		{
			nBaseRank = pAttacker->GetPassiveRank(TEPT_BASE_WEAPON_DUAL_PIERCE);
			nSpecializationRank = pAttacker->GetPassiveRank(TEPT_SPECIALIZATION_PIERCE);
		}
		break;
	}

	return ClacTalentFactor2Fomula(nBaseRank, nSpecializationRank);
}

float GAttackDamageCalculator::CalcTalentFactor2_NotMagic(GEntityPlayer* pAttacker, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType)
{
	float fLeftCalcTalentFactor2 = CalcTalentFactor2_NotMagic(pAttacker, nLeftWeaponType);
	float fRightCalcTalentFactor2 = CalcTalentFactor2_NotMagic(pAttacker, nRightWeaponType);

	float fCalcTalentFactor2 = (fLeftCalcTalentFactor2 + fRightCalcTalentFactor2) * 0.5f;

	return fCalcTalentFactor2;	
}

float GAttackDamageCalculator::ClacTalentFactor2Fomula(int nBaseTalentRank, int nSpecializationTalentRank)
{
	return ( (nBaseTalentRank * 0.05f) + (nSpecializationTalentRank * 0.05f) + 1 );
}

float GAttackDamageCalculator::CalcArmorFactor1( GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRate )
{
	if (pAttacker->IsPlayer())
	{
		GEntityPlayer* pPlayerAttacker = static_cast< GEntityPlayer*>(pAttacker);
		GItemHolder* pItemHolder = pPlayerAttacker->GetItemHolder();

		// 탤런트 보정율1은 순수 마법에 한정하여 적용한다.
		if (fWeaponApplyRate < 0.001f)
		{
			bool bHolyUnholy = false;
			if (nDamageAttrib == DA_HOLY || nDamageAttrib == DA_UNHOLY)
			{
				bHolyUnholy = true;
			}

			float fFactor = 0.0f;

			fFactor+= GetArmorFactor1Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_HEAD), bHolyUnholy);
			fFactor+= GetArmorFactor1Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_BODY), bHolyUnholy);
			fFactor+= GetArmorFactor1Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_LEG), bHolyUnholy);
			fFactor+= GetArmorFactor1Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_HANDS), bHolyUnholy);
			fFactor+= GetArmorFactor1Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_FEET), bHolyUnholy);

			return (fFactor / 5.0f);
		}
	}

	return 1.0f;
}

float GAttackDamageCalculator::m_fArmorFactor1Table[ARMOR_MAX][2] = {
	// 공격마법, 신성/암흑
	{1.0f, 1.0f},		// none
	{1.0f, 1.0f},		// cloth
	{0.8f, 1.0f},		// light
	{0.6f, 1.0f},		// medium
	{0.4f, 1.0f},		// heavy
	{1.0f, 1.0f},		// shield
	{1.0f, 1.0f}		// accessory
};


float GAttackDamageCalculator::GetArmorFactor1Table( ARMOR_TYPE nEquipmentType, bool bHolyUnholyMagic )
{
	int nMagicCategoryIndex = (bHolyUnholyMagic) ? 1 : 0;

	if (nEquipmentType < 0 || nEquipmentType >= ARMOR_MAX) nEquipmentType = ARMOR_NONE;
	return m_fArmorFactor1Table[nEquipmentType][nMagicCategoryIndex];
}

float GAttackDamageCalculator::m_fArmorFactor2Table[ARMOR_MAX][WEAPON_MAX] = {
	// none, 1hs, 1hb, 1hp, 2hd, stf, arc, axe, dwd, dwp
	{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},		// none
	{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},		// cloth
	{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},		// light
	{1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 0.7f, 1.0f, 0.7f, 1.0f},		// medium
	{1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f, 0.5f, 1.0f, 0.5f, 1.0f},		// heavy
	{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},		// shield
	{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}		// accessory
};

float GAttackDamageCalculator::GetArmorFactor2Table( ARMOR_TYPE nEquipmentType, WEAPON_TYPE nWeaponType )
{
	return m_fArmorFactor2Table[nEquipmentType][nWeaponType];
}

float GAttackDamageCalculator::CalcArmorFactor2( GEntityActor* pAttacker, float fWeaponApplyRate, WEAPON_REFRENCE nWeaponReference /*= WR_RIGHT*/ )
{
	float fFactor = 1.0f;

	if (pAttacker->IsPlayer())
	{
		GEntityPlayer* pPlayerAttacker = static_cast<GEntityPlayer*>(pAttacker);
		GItemHolder* pItemHolder = pPlayerAttacker->GetItemHolder();
		switch (nWeaponReference)
		{
		case WR_LEFT:
			{
				WEAPON_TYPE nLeftWeaponType = pItemHolder->GetEquipment().GetLeftWeaponType();
				fFactor = CalcArmorFactor2_Player(pPlayerAttacker, fWeaponApplyRate, nLeftWeaponType);
			}
			break;
		case WR_RIGHT:
			{
				WEAPON_TYPE nRightWeaponType = pItemHolder->GetEquipment().GetRightWeaponType();
				fFactor = CalcArmorFactor2_Player(pPlayerAttacker, fWeaponApplyRate, nRightWeaponType);
			}
			break;
		case WR_BOTH:
			{
				WEAPON_TYPE nLeftWeaponType = pItemHolder->GetEquipment().GetLeftWeaponType();
				WEAPON_TYPE nRightWeaponType = pItemHolder->GetEquipment().GetRightWeaponType();
				fFactor = CalcArmorFactor2_Player(pPlayerAttacker, fWeaponApplyRate, nLeftWeaponType, nRightWeaponType);
			}
			break;
		default:{
				_ASSERT(0);
			}
		}
	}

	return fFactor;
}

float GAttackDamageCalculator::CalcArmorFactor2_Player(GEntityPlayer* pPlayerAttacker, float fWeaponApplyRate, WEAPON_TYPE nWeaponType)
{
	float fFactor = 1.0f;

	// 탤런트 보정율2는 순수 마법에 한정하여 적용한다.
	if (fWeaponApplyRate < 0.001f)
	{
		GItemHolder* pItemHolder = pPlayerAttacker->GetItemHolder();
		fFactor+= GetArmorFactor2Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_HEAD), nWeaponType);
		fFactor+= GetArmorFactor2Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_BODY), nWeaponType);
		fFactor+= GetArmorFactor2Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_LEG), nWeaponType);
		fFactor+= GetArmorFactor2Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_HANDS), nWeaponType);
		fFactor+= GetArmorFactor2Table(pItemHolder->GetEquipment().GetSlotMaterial(ITEMSLOT_FEET), nWeaponType);

		fFactor /= 5.0f;
	}

	return fFactor;
}

float GAttackDamageCalculator::CalcArmorFactor2_Player(GEntityPlayer* pPlayerAttacker, float fWeaponApplyRate, WEAPON_TYPE nLeftWeaponType, WEAPON_TYPE nRightWeaponType)
{		
	float fLeftFactor = CalcArmorFactor2_Player(pPlayerAttacker, fWeaponApplyRate, nLeftWeaponType);
	float fRightFactor = CalcArmorFactor2_Player(pPlayerAttacker, fWeaponApplyRate, nRightWeaponType);

	float fFactor = (fLeftFactor + fRightFactor) * 0.5f;

	return fFactor;
}

int GAttackDamageCalculator::CalcTalentDamage( const GDamageRangedInfo& DamageInfo )
{
	/*
	// 크리티컬 배율
	if (bCritical)
	{
		return DamageInfo.nMaxDamage;
	}
	*/
	return MakeDamageFromMinMaxDamage(DamageInfo);
}

float GAttackDamageCalculator::CalcBuffDefenceFactor( GEntityActor* pAttacker, GEntityActor* pVictim, DAMAGE_ATTRIB nDamageAttrib, WEAPON_REFRENCE nWeaponReference /*= WR_RIGHT*/ )
{
	float fBuffFactor = 1.0f;

	if (nDamageAttrib == DA_NONE)
	{
		// 피해타입이 없으면 무기 피해타입을 참조
		if (pAttacker->IsPlayer())
		{
			nDamageAttrib = ToEntityPlayer(pAttacker)->GetDamageType(nDamageAttrib, nWeaponReference);
		}
	}

	if (nDamageAttrib <= DA_NONE)
		return fBuffFactor;

	if (nDamageAttrib >= DA_MAX)
		return fBuffFactor;

	fBuffFactor += pVictim->GetChrStatus()->ActorModifier.fDefenceAmp[nDamageAttrib];
	return fBuffFactor;
}

GAttackDamageCalculator::GAttackDamageCalculator( GCriticalCalculator* pCriticalCalculator/*=NULL*/ )
{
	if (pCriticalCalculator==NULL)
	{
		m_pCriticalCalculator = new GCriticalCalculator();
	}
	else
	{
		m_pCriticalCalculator = pCriticalCalculator;
	}
}

GAttackDamageCalculator::~GAttackDamageCalculator()
{
	SAFE_DELETE(m_pCriticalCalculator);
}

int GAttackDamageCalculator::CalcNPCWeaponDamage( const GNPCInfo* pAttackerNPCInfo, CALC_TYPE_FOR_WEAPON_DAMAGE calcType/* = CTWD_MINMAX*/ )
{
	if (pAttackerNPCInfo)
	{
		switch (calcType)
		{
		case CTWD_MINMAX:
		default:
			return GMath::RandomNumber(pAttackerNPCInfo->m_nMinDamage, pAttackerNPCInfo->m_nMaxDamage);
		case CTWD_MIN: return pAttackerNPCInfo->m_nMinDamage;
		case CTWD_MAX: return pAttackerNPCInfo->m_nMaxDamage;
		}
	}
	return 0;
}

float GAttackDamageCalculator::CalcCharacterFactorForMagic(GEntityPlayer* pAttacker)
{
	int nInt = pAttacker->GetINT();

	return GMath::Round4Combat((nInt - 50) * 0.002f + 0.9f, 2);
}

float GAttackDamageCalculator::CalcCharacterFactorForMelee(GEntityPlayer* pAttacker)
{
	int nStr = pAttacker->GetSTR();

	return GMath::Round4Combat((nStr - 50) * 0.002f + 0.9f, 2);
}

float GAttackDamageCalculator::CalcCharacterFactorForRange(GEntityPlayer* pAttacker)
{
	int nStr = pAttacker->GetSTR();
	int nDex = pAttacker->GetDEX();

	return GMath::Round4Combat(((nStr + nDex) * 0.5f - 50) * 0.002f + 0.9f, 2);
}

ATTACK_TYPE_FOR_CALCULATE GAttackDamageCalculator::CalcAttackTypeForCalculate(GEntityPlayer* pAttacker, TALENT_DAMAGE_TYPE nDamageType, WEAPON_REFRENCE nWeaponReference /*= WR_RIGHT*/)
{
	switch (nDamageType)
	{
	case TDT_MAGIC:
		return ATC_MAGIC_ATTACK;
	case TDT_PHYSIC:
		{
			// 계산에 사용할 무기 타입을 구한다.
			GItemHolder* pItemHolder = pAttacker->GetItemHolder();
			WEAPON_TYPE nWeaponType = WEAPON_NONE;

			switch(nWeaponReference)
			{
			case WR_LEFT:			
				nWeaponType = pItemHolder->GetEquipment().GetLeftWeaponType();
				break;
			case WR_RIGHT:
				nWeaponType = pItemHolder->GetEquipment().GetRightWeaponType();
				break;
			case WR_BOTH:
				nWeaponType = pItemHolder->GetEquipment().GetWeaponType();
				break;
			};

			// 찌르기, 원거리, 듀얼무기
			if (nWeaponType == WEAPON_ARCHERY ||
				nWeaponType == WEAPON_1H_PIERCE ||
				nWeaponType == WEAPON_DUAL_WIELD || nWeaponType == WEAPON_DUAL_PIERCE)
			{
				return ATC_DEX_WEAPON_ATTACK;
			}
			else
			{
				return ATC_STR_WEAPON_ATTACK;
			}
		}		
	};	

	return ATC_STR_WEAPON_ATTACK;
}

bool GAttackDamageCalculator::HasWeaponPassiveTalent( GEntityActor* pActor, const GItem* pWeaponItem )
{
	if (!pActor->IsPlayer())
		return true;

	if (!gsys.pItemSystem->GetEquiper().CheckHasPassiveTalent(ToEntityPlayer(pActor), (GItem*) pWeaponItem))
		return false;

	return true;
}
