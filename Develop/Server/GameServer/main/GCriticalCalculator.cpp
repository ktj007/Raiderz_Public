#include "stdafx.h"
#include "GCriticalCalculator.h"
#include "GConst.h"

// GCriticalCalculator
//////////////////////////////////
float GCriticalCalculator::CalcActorCriticalPercent( const GEntityActor* pActor, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType )
{
	float fCriticalPercent = 0.0f;
	if (pActor->GetTypeID() == ETID_PLAYER)
	{
		const GEntityPlayer* pPlayer = static_cast<const GEntityPlayer*>(pActor);
		fCriticalPercent = _CalcPlayerCriticalPercent(nDamageType, pPlayer);
	}
	else if (pActor->GetTypeID() == ETID_NPC)
	{
		const GEntityNPC* pNPC = static_cast<const GEntityNPC*>(pActor);
		fCriticalPercent = _CalcNPCCriticalPercent(nSkillType, pNPC->GetNPCInfo());
	}

	return fCriticalPercent;
}

float GCriticalCalculator::_CalcPlayerCriticalPercent( TALENT_DAMAGE_TYPE nDamageType, const GEntityPlayer* pPlayer )
{
	// float fCriticalPercent = 0.0f;
	/*
	switch (nTalentCategory)
	{
	case TC_COMMON:
	case TC_MELEE:
		fCriticalPercent = (pPlayer->GetDEX()/2) * 0.08f;
		break;
	case TC_RANGE:
		fCriticalPercent = (pPlayer->GetDEX()/2) * 0.08f;
		break;
	case TC_MAGIC:
		fCriticalPercent = (pPlayer->GetINT()/2) * 0.05f;
		break;
	}
	*/
	/*
	switch (nDamageType)
	{
	case TDT_PHYSIC:
		fCriticalPercent = (pPlayer->GetDEX()/2) * 0.08f;
		break;
	case TDT_MAGIC:
		fCriticalPercent = (pPlayer->GetINT()/2) * 0.05f;
		break;
	}
	*/

	// TODO: implement correct critical percent formula

	VALID_RET(pPlayer->GetLevel() > 0, 0.f);
	return pPlayer->GetDEX() * (0.1f + (0.2f / pPlayer->GetLevel()));
}

float GCriticalCalculator::_CalcNPCCriticalPercent( TALENT_SKILL_TYPE nSkillType, GNPCInfo* pNPCInfo )
{
	float fCriticalPercent = 0.0f;
	switch (nSkillType)
	{
	case ST_MELEE:
		{
			fCriticalPercent = pNPCInfo->m_fCriticalMelee * 100.0f;
		} break;
	case ST_ARCHERY:
		{
			fCriticalPercent = pNPCInfo->m_fCriticalRange * 100.0f;
		} break;
	case ST_MAGIC:
	case ST_EXTRA_ACTIVE:
		{
			fCriticalPercent = pNPCInfo->m_fCriticalMagic * 100.0f;
		} break;
	}
	return fCriticalPercent;
}

float GCriticalCalculator::CalcTalentPercent( GEntityActor* pAttacker, GEntityActor* pVictim, TALENT_DAMAGE_TYPE nDamageType, float fCriticalApplyRate /*= 1.f*/ )
{
	float fCriticalAmp = 0.0f;

	switch (nDamageType)
	{
	case TDT_PHYSIC:
		fCriticalAmp = pAttacker->GetChrStatus()->ActorModifier.fCriticalPhysicAmp + pVictim->GetChrStatus()->ActorModifier.fCriticalPhysicAmpForMe;
		break;
	case TDT_MAGIC:
		fCriticalAmp = pAttacker->GetChrStatus()->ActorModifier.fCriticalMagicAmp + pVictim->GetChrStatus()->ActorModifier.fCriticalMagicAmpForMe;
		break;
	default:
		fCriticalAmp = 0.0f;
	}

	fCriticalAmp += pAttacker->GetChrStatus()->ActorModifier.fCriticalAmp;

	return fCriticalAmp * fCriticalApplyRate * 0.1f;
}

float GCriticalCalculator::CalcBuffDamagePercent( GEntityActor* pActor, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType )
{
	const CHR_STATUS* pChrStatus = pActor->GetChrStatus();

	float fDamagePercent = 0.0f;
	if (pActor->GetTypeID() == ETID_PLAYER)
	{
		const GEntityPlayer* pPlayer = static_cast<const GEntityPlayer*>(pActor);

		switch (nDamageType)
		{
		case TDT_PHYSIC:
		default:
			{
				fDamagePercent = pChrStatus->ActorModifier.fCriticalPhysicDamageAmp;
			} break;
		case TDT_MAGIC:
			{
				fDamagePercent = pChrStatus->ActorModifier.fCriticalMagicDamageAmp;
			} break;
		}
	}
	else if (pActor->GetTypeID() == ETID_NPC)
	{
		const GEntityNPC* pNPC = static_cast<const GEntityNPC*>(pActor);
		GNPCInfo* pNPCInfo = pNPC->GetNPCInfo();
		switch (nSkillType)
		{
		case ST_MELEE:		
		case ST_ARCHERY:
			{
				fDamagePercent = pChrStatus->ActorModifier.fCriticalPhysicDamageAmp;
			} break;
		case ST_MAGIC:
		case ST_EXTRA_ACTIVE:
			{
				fDamagePercent = pChrStatus->ActorModifier.fCriticalMagicDamageAmp;
			} break;
		}
	}

	fDamagePercent = fDamagePercent * 100.0f;

	return fDamagePercent;
}

float GCriticalCalculator::CalcCriticalPercent( GEntityActor* pAttacker, GEntityActor* pVictim, DAMAGE_ATTRIB nDamageAttrib, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType, float fCriticalApplyRate /*= 1.f*/ )
{
	DCHECK(pAttacker);

	int nAttackerLevel = pAttacker->GetChrInfo()->nLevel;
	int nVictimLevel = pVictim->GetChrInfo()->nLevel;

	// 공격자 입장에서 레벨 팩터
	int nLevelFactor = GCalculator::CalcLevelFactor(nAttackerLevel, nVictimLevel);
	float fVictimMODGrade = GCalculator::CalcModGrade(pVictim->GetGrade());

	// 방어자 등급 수정치
	float fVictimMod = GMath::Round4Combat(1 - (fVictimMODGrade * fVictimMODGrade), 2);

	// 캐릭터 치명타율
	float fAttackerCriticalPercent = CalcActorCriticalPercent(pAttacker, nDamageType, nSkillType);

	// 데미지 속성 크리율
	float fDamageAttribPercent = 0.0f;
	if (pAttacker->GetTypeID() == ETID_PLAYER) 
	{
		GEntityPlayer* pAttackerPlayer = ToEntityPlayer(pAttacker);
		fDamageAttribPercent = CalcDamageAttribPercent(pAttackerPlayer, nDamageAttrib);
	}

	// 버프&아이템 치명율 수정치의 합
	float fBuffPercent = CalcTalentPercent(pAttacker, pVictim, nDamageType, fCriticalApplyRate);

#if 1 || 1
	printf("nLevelFactor: %d\n", nLevelFactor);
	printf("fVictimMod: %f\n", fVictimMod);
	printf("fAttackerCriPerc: %f\n", fAttackerCriticalPercent);
	printf("fDamageAttrPerc: %f\n", fDamageAttribPercent);
	printf("fBuffPerc: %f\n", fBuffPercent);
#endif

	float fFinalPercent = max((5-nLevelFactor) * 0.6f + 
		fVictimMod + 
		fAttackerCriticalPercent + 
		fDamageAttribPercent + 
		fBuffPercent, 0.0f);
	return min(fFinalPercent, 100.0f);
}

float GCriticalCalculator::CalcCriticalDamageFactor( GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, TALENT_DAMAGE_TYPE nDamageType, TALENT_SKILL_TYPE nSkillType )
{
	float fDamagePercent = 100.f * GConst::TALENT_CRITICAL_DAMAGE_APPLY_RATE;//150.0f;

	if (pAttacker->IsPlayer()) 
	{
		GEntityPlayer* pAttackerPlayer = static_cast<GEntityPlayer*>(pAttacker);
		fDamagePercent += CalcDamageBonusPercent(pAttackerPlayer, nDamageAttrib);
	}
	else if (pAttacker->IsNPC())
	{
		const GEntityNPC* pAttackerNPC = static_cast<GEntityNPC*>(pAttacker);
		fDamagePercent += (pAttackerNPC->GetNPCInfo()->m_fCriticalDmgAmp * 100.0f);
	}

	// 패시브, 버프
	float fBuffDamagePercent = CalcBuffDamagePercent(pAttacker, nDamageType, nSkillType);

	return (GMath::PercentToFactor(fDamagePercent + fBuffDamagePercent));
}


float GCriticalCalculator::CalcDamageAttribPercent( GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib )
{
	const int nSpecializationRank = _GetSpecializationRankForDamageAttrib(pPlayer, nDamageAttrib);
	return _CalcDamageAttribPercent(nDamageAttrib, nSpecializationRank);
}

float GCriticalCalculator::_CalcDamageAttribPercent( DAMAGE_ATTRIB nDamageAttrib, int nSpecializationRank )
{
	float fBonusPercent = 0.0f;

	switch (nDamageAttrib)
	{
	case DA_SLASH: 
		{
			fBonusPercent = nSpecializationRank * 1.0f;
		}
		break;
	case DA_BLUNT:
		{
			fBonusPercent = nSpecializationRank * 1.0f;
		}
		break;
	case DA_PIERCE:
		{
			fBonusPercent = nSpecializationRank * 1.1f;
		}
		break;
	}
	return fBonusPercent;
}

int GCriticalCalculator::CalcDamageBonusPercent( GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib )
{
	const int nSpecializationRank = _GetSpecializationRankForDamageAttrib(pPlayer, nDamageAttrib);
	return _CalcDamageBonusPercent(nDamageAttrib, nSpecializationRank);
}

int GCriticalCalculator::_CalcDamageBonusPercent( DAMAGE_ATTRIB nDamageAttrib, int nSpecializationRank )
{
	int nBonusPercent = 0;

	switch (nDamageAttrib)
	{
	case DA_SLASH: 
		{
			nBonusPercent = nSpecializationRank * 4;
		}
		break;
	case DA_BLUNT:
		{
			nBonusPercent = nSpecializationRank * 4;
		}
		break;
	case DA_PIERCE:
		{
			nBonusPercent = nSpecializationRank * 5;
		}
		break;
	}
	return nBonusPercent;
}

int GCriticalCalculator::_GetSpecializationRankForDamageAttrib( GEntityPlayer* pPlayer, DAMAGE_ATTRIB nDamageAttrib )
{
	switch (nDamageAttrib)
	{
	case DA_SLASH: 
		{
			return pPlayer->GetPassiveRank(TEPT_SPECIALIZATION_SLASH);
		}
		break;
	case DA_BLUNT:
		{
			return pPlayer->GetPassiveRank(TEPT_SPECIALIZATION_BLUNT);
		}
		break;
	case DA_PIERCE:
		{
			return pPlayer->GetPassiveRank(TEPT_SPECIALIZATION_PIERCE);
		}
		break;
	}

	return 0;
}
