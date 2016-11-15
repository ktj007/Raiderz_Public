#include "stdafx.h"
#include "GResistCalculator.h"
#include "CSTalentInfo.h"
#include "GEntityActor.h"
#include "GCalculator.h"

float GResistCalculator::CalcDefRate(const GEntityActor* pVictim, const GTalentResist& Resist)
{
	float fValue = 0;
	switch (Resist.m_nType)
	{
	case TRT_NONE: return 0.0f;
	case TRT_FR:
	case TRT_CR:
	case TRT_PR:
	case TRT_LR:
	case TRT_HR:
	case TRT_UR:
		{
			fValue = (float)pVictim->GetResist(Resist.m_nType);
		}
		break;
	case TRT_CHA:
		{
			fValue = (float)pVictim->GetCHA();
		}
		break;
	default:
		_ASSERT(0);
	}

	return GMath::Round4Combat((fValue - 60) / 11.0f, 2);
}

float GResistCalculator::CalcResistPercent(const GEntityActor* pAttacker, const GEntityActor* pVictim, const GTalentResist& Resist)
{
	// 저항타입이 없을 경우는 diffculty/255로 확률 저항 처리
	if (Resist.m_nType == TRT_NONE)
	{
		if (Resist.m_nDifficulty == 255)
			return 0.0f; // 255이면 반드시 실패
		return float(255-Resist.m_nDifficulty) / 255.0f * 100.0f;
	}

	// 탤런트 기본 저항치
	float fTalentResistRate = (255 - Resist.m_nDifficulty) / 400.0f * 100.0f;		

	float fLevelRate = 0.0f;
	float fGradeRate = 0.0f;
	if (NULL != pAttacker)
	{
		int nAttackerLevel = pAttacker->GetChrInfo()->nLevel;
		int nVictimLevel = pVictim->GetChrInfo()->nLevel;

		// 레벨 보정치
		fLevelRate = (GCalculator::CalcLevelFactor(nAttackerLevel, nVictimLevel) - 5) * 2.0f;

		float fAttackerMODGrade = GCalculator::CalcModGrade(pAttacker->GetGrade());

		// 등급 보정치
		fGradeRate = GMath::Round4Combat((5 - fAttackerMODGrade * fAttackerMODGrade * 5) * 0.5f, 2);
	}


	// 저항치에 따른 저항율 보정치
	float fDefRate = CalcDefRate(pVictim, Resist);

	float fFinalRate = min(max(fTalentResistRate +
		fLevelRate +
		fGradeRate +
		fDefRate, 0), 100);
	return GMath::Round4Combat(fFinalRate, 2);
}
