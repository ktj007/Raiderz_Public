#include "stdafx.h"
#include "GDodgeCalculator.h"
#include "GEntityPlayer.h"
#include "GCalculator.h"

float GDodgeCalculator::CalcAttackerDodgePercent(GEntityActor* pActor)
{
	if (pActor->GetTypeID() == ETID_PLAYER)
	{
		const GEntityPlayer* pEntityPlayer = static_cast<const GEntityPlayer*>(pActor);
		int nDEX = pEntityPlayer->GetDEX();

		return GMath::Round4Combat((nDEX - 60) * 0.0005f, 2) * 100.0f;
	}
	else if (pActor->GetTypeID() == ETID_NPC)
	{
		int nGrade = pActor->GetGrade();
		int nLevel = pActor->GetLevel();

		return (nGrade - 3) + GMath::TruncateToInt(nLevel / 5.0f) * 0.15f;
	}

	return 0.0f;
}

float GDodgeCalculator::CalcVictimDodgePercent(GEntityActor* pVictim)
{	
	if (pVictim->IsAllowCalcVictimDodge())
	{		
		// 공격자 회피 계산과 동일하다.
		return CalcAttackerDodgePercent(pVictim);
	}

	return 0.0f;
}

float GDodgeCalculator::CalcBuffDodgePercent(GEntityActor* pActor, TALENT_CATEGORY nTalentCategory)
{
	float fDodgeAmp = 0.0f;

	switch(nTalentCategory)
	{
	case TC_MELEE:
		fDodgeAmp = pActor->GetChrStatus()->ActorModifier.fMeleeDodgeAmp;
		break;
	case TC_RANGE:
		fDodgeAmp = pActor->GetChrStatus()->ActorModifier.fRangeDodgeAmp;
		break;
	case TC_MAGIC:			
		fDodgeAmp = pActor->GetChrStatus()->ActorModifier.fMagicDodgeAmp;
		break;
	default:
		fDodgeAmp = 0.0f;
		break;
	}

	return fDodgeAmp * 100.0f;
}

float GDodgeCalculator::CalcBuffHitPercent(GEntityActor* pActor, TALENT_CATEGORY nTalentCategory)
{
	float fHitRateAmp = 0.0f;

	switch(nTalentCategory)
	{
	case TC_MELEE:
		fHitRateAmp = pActor->GetChrStatus()->ActorModifier.fMeleeHitRateAmp;
		break;
	case TC_RANGE:
		fHitRateAmp = pActor->GetChrStatus()->ActorModifier.fRangeHitRateAmp;
		break;
	case TC_MAGIC:
		fHitRateAmp = pActor->GetChrStatus()->ActorModifier.fMagicHitRateAmp;
		break;
	default:
		fHitRateAmp = 0.0f;
		break;
	}

	return fHitRateAmp * 100.0f;
}

float GDodgeCalculator::CalcDodgePercent(GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo)
{
	// TODO: 회피 예외사항중 Sleep 구현안됨
	if (pVictim->GetActionState() == AS_MF && pVictim->GetCurrentMotionFactor() == MF_STUN)
	{
		return 0.0f;
	}

	uint8 nTalentDodgeDifficulty = pTalentInfo->m_nDodgeDifficulty;
	if (nTalentDodgeDifficulty == 255) return 0.0f;

	// 탤런트 기본 회피율
	float fTalentDodgeRate = CalcAttackTalentDodgePercent(pTalentInfo);

	// 레벨 팩터에 의한 수정치
	float fLFPercent = CalcLevelFactorDodgePercent(pAttacker, pVictim);

	// 공격자 회피 수정치
	float fAttackerDodgePercent = CalcAttackerDodgePercent(pAttacker);		

	// 방어자 회피 수정치
	float fVictimDodgePercent = CalcVictimDodgePercent(pVictim);


	// 버프 회피율
	float fBuffDodgePercent = CalcBuffDodgePercent(pVictim, pTalentInfo->m_nCategory);

	float fFinalDodgePercent = max(min(fTalentDodgeRate + fLFPercent + fVictimDodgePercent - fAttackerDodgePercent + fBuffDodgePercent, 100.0f), 0.0f);
	fFinalDodgePercent = GMath::Round4Combat(fFinalDodgePercent, 2);


	// 최후 명중률 보정
	fFinalDodgePercent -= CalcBuffHitPercent(pAttacker, pTalentInfo->m_nCategory);
	fFinalDodgePercent = max(min(fFinalDodgePercent, 100), 0);


	return fFinalDodgePercent;
}

float GDodgeCalculator::CalcAttackTalentDodgePercent(const GTalentInfo* pTalentInfo)
{
	VALID_RET(pTalentInfo != NULL, 0.0f);

	int nDodgeDifficulty = pTalentInfo->m_nDodgeDifficulty;

	return GMath::Round4Combat((255 - nDodgeDifficulty) / 400.0f * 100, 2);
}

float GDodgeCalculator::CalcLevelFactorDodgePercent(const GEntityActor* pAttacker, const GEntityActor* pVictim)
{
	int nAttackerLevel = pAttacker->GetLevel();
	int nVictimLevel = pVictim->GetLevel();

	int nLevelFactor = GCalculator::CalcLevelFactor(nAttackerLevel, nVictimLevel);

	return (nLevelFactor - 10) * 0.3f;
}

