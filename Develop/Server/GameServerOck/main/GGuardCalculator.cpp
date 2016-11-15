#include "stdafx.h"
#include "GGuardCalculator.h"
#include "GEntityPlayer.h"
#include "GCalculator.h"
#include "GConst.h"
#include "GGlobal.h"
#include "GItemHolder.h"
#include "GTestSystem.h"
#include "GEquipmentSlot.h"

float GGuardCalculator::CalcPerfectGuardRate( GEntityActor* pAttacker, GEntityActor* pGuarder )
{
	int nAttackerLevel = pAttacker->GetLevel();
	int nGuarderLevel = pGuarder->GetLevel();
	int8 nAttackerGrade = pAttacker->GetGrade();
	int8 nGuarderGrade = pGuarder->GetGrade();
	int nGuardTalentRank = 0;

	if (pGuarder->IsPlayer())
	{
		GEntityPlayer* pGuarderPlayer = static_cast<GEntityPlayer*>(pGuarder);
		nGuardTalentRank = pGuarderPlayer->GetPassiveValue(TEPT_GUARD);
	}

	int nLF = GCalculator::CalcLevelFactor(nGuarderLevel, nAttackerLevel);

	// 완전방어 성공율 = MIN(ROUND(SQRT(10-레벨팩터)*30,1)+10 + (방패숙련 랭크 * 7) - MAX(공격자 등급 - 3, 0) * 2.5 + MAX(방어자 등급 - 3, 0) * 2.5, 100)
	float fRate = GMath::Round4Combat(sqrt(10.0f-(float)nLF)*30.0f,1)+10 + (nGuardTalentRank*7) - (max(nAttackerGrade-3,0)*2.5f) + (max(nGuarderGrade-3,0)*2.5f);
	fRate = min(fRate, 100.0f);
	fRate = max(fRate, 0.0f);

	if (IsRunForTest())
	{
		if (GConst::TEST_SHOW_CLIENT_FORMULA_GUARD && pGuarder->IsPlayer())
		{
			wchar_t buff[512];
			swprintf_s(buff, L"LF = %d, GuardRank = %d, AtkGrade = %d, DefGrade = %d\n%.1f = 100 - %d + %d - %d + %d", 
				nLF, nGuardTalentRank, nAttackerGrade, nGuarderGrade,
				fRate, (max(nLF-4, 0)*20), (nGuardTalentRank*10), (max(nAttackerGrade-3,0)*5), (max(nGuarderGrade-3,0)*5));
			gsys.pTestSystem->RouteCombatCalcDebug(ToEntityPlayer(pGuarder), L"완전방어", buff);
		}
	}

	return fRate;
}

float GGuardCalculator::CalcPartialGuardRate(GEntityActor* pAttacker, GEntityActor* pGuarder)
{
	int nAttackerLevel = pAttacker->GetLevel();
	int nGuarderLevel = pGuarder->GetLevel();
	int8 nAttackerGrade = pAttacker->GetGrade();
	int8 nGuarderGrade = pGuarder->GetGrade();
	int nGuardTalentRank = 0;

	if (pGuarder->IsPlayer())
	{
		GEntityPlayer* pGuarderPlayer = static_cast<GEntityPlayer*>(pGuarder);
		nGuardTalentRank = pGuarderPlayer->GetPassiveValue(TEPT_GUARD);
	}

	int nLF = GCalculator::CalcLevelFactor(nGuarderLevel, nAttackerLevel);

	// 부분방어 성공율 = MIN(MAX(100 - MAX(레벨팩터 - 4, 0) * 20, 0) + (방패숙련 랭크 * 10) - MAX(공격자 등급 - 3, 0) * 5 + MAX(방어자 등급 - 3, 0) * 5, 100)

	float fRate = 100.0f - (max(nLF-4,0)*20) + (nGuardTalentRank*10) - (max(nAttackerGrade-3,0)*5) + (max(nGuarderGrade-3,0)*5);
	fRate = min(fRate, 100.0f);
	fRate = max(fRate, 0.0f);

	if (IsRunForTest())
	{
		if (GConst::TEST_SHOW_CLIENT_FORMULA_GUARD && pGuarder->IsPlayer())
		{
			wchar_t buff[512];
			swprintf_s(buff, L"LF = %d, GuardRank = %d, AtkGrade = %d, DefGrade = %d\n%.1f = 100 - %d + %d - %d + %d", 
				nLF, nGuardTalentRank, nAttackerGrade, nGuarderGrade,
				fRate, (max(nLF-4, 0)*20), (nGuardTalentRank*10), (max(nAttackerGrade-3,0)*5), (max(nGuarderGrade-3,0)*5));
			gsys.pTestSystem->RouteCombatCalcDebug(ToEntityPlayer(pGuarder), L"부분방어", buff);
		}
	}

	return fRate;
}

float GGuardCalculator::CalcPartialGuardDamageMod(GEntityActor* pGuarder, GEntityActor* pAttacker)
{
	// 무기 부분방어 기본 흡수율
	float fDefaultDefRate = 1.0f;
	// 방어숙련도
	float fDefenseExpertRate = 0.0f;
	if (pGuarder->IsPlayer())
	{
		GEntityPlayer* pGuardPlayer = ToEntityPlayer(pGuarder);			
		GItemHolder* pItemHolder = pGuardPlayer->GetItemHolder();
		if (pGuardPlayer->IsEquipShield())
		{
			switch(pItemHolder->GetEquipment().GetRightWeaponType())
			{
			case WEAPON_1H_SLASH:	fDefaultDefRate = 0.32f;  break;
			case WEAPON_1H_BLUNT:	fDefaultDefRate = 0.32f;  break;
			}
		}
		else
		{
			switch(pItemHolder->GetEquipment().GetRightWeaponType())
			{
			case WEAPON_1H_SLASH:	fDefaultDefRate = 0.5f;  break;
			case WEAPON_1H_BLUNT:	fDefaultDefRate = 0.5f;  break;
			case WEAPON_TWO_HANDED:	fDefaultDefRate = 0.45f;  break;
			case WEAPON_2H_BLUNT:	fDefaultDefRate = 0.35f;  break;
			case WEAPON_DUAL_PIERCE:fDefaultDefRate = 0.5f;  break;
			case WEAPON_STAFF:		fDefaultDefRate = 0.4f;  break;
			}
		}

		fDefenseExpertRate = pGuardPlayer->GetPassiveRank(TEPT_GUARD) * 0.1f;
	}

	float fMod = fDefaultDefRate - 
		fDefenseExpertRate;
	//					 max(pAttacker->GetGrade()-3,0)*0.05f -
	//					 max(pGuarder->GetGrade()-3,0)*0.05f;
	fMod = min(fMod,1.0f);
	fMod = max(fMod,0.0f);
	return fMod;
}

float GGuardCalculator::CalcAPFactor(int nGuardTalentRank, bool bEquipShield, uint32 nGuardTime)
{
	if (nGuardTime > 2000) return 1.0f;

	float fGuardAPFactor = 1.0f;

	switch (nGuardTalentRank)
	{
	case 0:	fGuardAPFactor = (bEquipShield) ? 2.0f : 1.0f; break;
	case 1:	fGuardAPFactor = (bEquipShield) ? 4.0f : 2.0f; break;
	case 2:	fGuardAPFactor = (bEquipShield) ? 6.0f : 3.0f; break;
	case 3:	fGuardAPFactor = (bEquipShield) ? 8.0f : 4.0f; break;
	case 4:	fGuardAPFactor = (bEquipShield) ? 10.0f : 5.0f; break;
	case 5:	fGuardAPFactor = (bEquipShield) ? 12.0f : 6.0f; break;
	}

	return fGuardAPFactor;
}
