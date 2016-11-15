#include "stdafx.h"
#include "CSTalentInfoHelper.h"

int CSTalentInfoHelper::_NormalAttackTalentID(WEAPON_TYPE nWeaponType, TALENT_NORMAL_ATTACK_TYPE nAttackType)
{
	int nCombo = 0;
	if (nAttackType == NA_NORMAL1) nCombo = 0;
	else if (nAttackType == NA_NORMAL2) nCombo = 1;
	else if (nAttackType == NA_NORMAL3) nCombo = 2;
	else if (nAttackType == NA_NORMAL4) nCombo = 3;
	else if (nAttackType == NA_NORMAL5) nCombo = 4;
	else if (nAttackType == NA_NORMAL6) nCombo = 5;

	switch (nWeaponType)
	{
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + nCombo + 1);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + nCombo + 1);
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + nCombo + 1);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + nCombo + 1);
	case WEAPON_STAFF:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + nCombo + 1);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + nCombo + 1);	
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + nCombo + 1);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + nCombo + 1);
	case WEAPON_ARCHERY:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC + 1);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalAdvanceAttackTalentID(WEAPON_TYPE nWeaponType)
{
	const int ADVANCE_ATTACK_DEFAULT_ID = 11;

	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_STAFF:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + ADVANCE_ATTACK_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + ADVANCE_ATTACK_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalRisingComboTalentID(WEAPON_TYPE nWeaponType)
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + 25);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + 25);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + 25);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + 25);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + 25);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + 25);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + 25);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + 25);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalRisingAttackTalentID(WEAPON_TYPE nWeaponType)
{
	const int RISING_ATTACK_DEFAULT_ID = 92;

	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + RISING_ATTACK_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + RISING_ATTACK_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalDodgeFrontTalentID(WEAPON_TYPE nWeaponType)
{
	const int DODGE_FRONT_DEFAULT_ID = 70;

	switch (nWeaponType)
	{
	case WEAPON_NONE:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_NONE + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_ARCHERY:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + DODGE_FRONT_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalDodgeBackTalentID(WEAPON_TYPE nWeaponType)
{
	const int DODGE_FRONT_DEFAULT_ID = 71;

	switch (nWeaponType)
	{
	case WEAPON_NONE:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_NONE + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_ARCHERY:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + DODGE_FRONT_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalDodgeLeftTalentID(WEAPON_TYPE nWeaponType)
{
	const int DODGE_FRONT_DEFAULT_ID = 72;

	switch (nWeaponType)
	{
	case WEAPON_NONE:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_NONE + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_ARCHERY:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + DODGE_FRONT_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalDodgeRightTalentID(WEAPON_TYPE nWeaponType)
{
	const int DODGE_FRONT_DEFAULT_ID = 73;

	switch (nWeaponType)
	{
	case WEAPON_NONE:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_NONE + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_ARCHERY:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC + DODGE_FRONT_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + DODGE_FRONT_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalUpperAttackTalentID(WEAPON_TYPE nWeaponType)
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + 23);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + 23);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + 23);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + 23);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + 23);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + 23);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + 23);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + 23);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalDownAttackTalentID(WEAPON_TYPE nWeaponType)
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + 20);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + 20);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + 20);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + 20);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + 20);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + 20);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + 20);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + 20);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}
int CSTalentInfoHelper::NormalSuperAttackTalentID(WEAPON_TYPE nWeaponType)
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + 24);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + 24);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + 24);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + 24);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + 24);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + 24);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + 24);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + 24);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}





/// 방어 공격(카운터) 탤런트 ID 반환
int CSTalentInfoHelper::NormalCounterAttackTalentID(WEAPON_TYPE nWeaponType, bool bEquipShield)
{
	const int GUARD_ATTACK_DEFAULT_ID = 74;

	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	
		{
			if (bEquipShield) return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + GUARD_ATTACK_DEFAULT_ID + 1);
			return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + GUARD_ATTACK_DEFAULT_ID);
		}
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + GUARD_ATTACK_DEFAULT_ID);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + GUARD_ATTACK_DEFAULT_ID);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + GUARD_ATTACK_DEFAULT_ID);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + GUARD_ATTACK_DEFAULT_ID);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + GUARD_ATTACK_DEFAULT_ID);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + GUARD_ATTACK_DEFAULT_ID);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + GUARD_ATTACK_DEFAULT_ID);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalAttackTalentID( WEAPON_TYPE nWeaponType, TALENT_NORMAL_ATTACK_TYPE nAttackType, bool bEquipShield/*=true*/ )
{
	int nTalentID = 0;

	switch (nAttackType)
	{
	case NA_NORMAL1:
	case NA_NORMAL2:
	case NA_NORMAL3:
	case NA_NORMAL4:
	case NA_NORMAL5:
	case NA_NORMAL6:
		nTalentID = _NormalAttackTalentID(nWeaponType, nAttackType);	break;
	case NA_ADVANCE:		nTalentID = NormalAdvanceAttackTalentID(nWeaponType);		break;

	case NA_DODGE_FRONT:	nTalentID = NormalDodgeFrontTalentID(nWeaponType);		break;
	case NA_DODGE_BACK:		nTalentID = NormalDodgeBackTalentID(nWeaponType);		break;
	case NA_DODGE_LEFT:		nTalentID = NormalDodgeLeftTalentID(nWeaponType);		break;
	case NA_DODGE_RIGHT:	nTalentID = NormalDodgeRightTalentID(nWeaponType);		break;
	case NA_UPPER:			nTalentID = NormalUpperAttackTalentID(nWeaponType);		break;
	case NA_DOWN:			nTalentID = NormalDownAttackTalentID(nWeaponType);		break;
	case NA_SUPER:			nTalentID = NormalSuperAttackTalentID(nWeaponType);		break;
	case NA_RISING:			nTalentID = NormalRisingAttackTalentID(nWeaponType);	break;
	case NA_SWORD_DRAWING:	nTalentID = NormalSwordDrawingTalentID(nWeaponType);	break;
	case NA_COUNTER_ATTACK:	nTalentID = NormalCounterAttackTalentID(nWeaponType, bEquipShield);	break;		// NormalGuardAttackTalentID 함수를 직접 사용
	case NA_UPPERCOMBO1:	nTalentID = NormalRisingComboTalentID(nWeaponType);		break;
	default:
		assert(0);
	}
	return nTalentID;
}

int CSTalentInfoHelper::NormalGuardTalentID( WEAPON_TYPE nWeaponType, int nGuardLevel, bool bIsEquippedShield )
{
	int nGuardTypeIndex = 0;

	if (bIsEquippedShield)	nGuardTypeIndex = 0;
	else					nGuardTypeIndex = 10;

	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + 50 + nGuardTypeIndex + nGuardLevel);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + 50 + nGuardTypeIndex + nGuardLevel);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + 50 + nGuardTypeIndex + nGuardLevel);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + 50 + nGuardTypeIndex + nGuardLevel);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + 50 + nGuardTypeIndex + nGuardLevel);
	case WEAPON_STAFF:		return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + 50 + nGuardTypeIndex + nGuardLevel);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + 50 + nGuardTypeIndex  + nGuardLevel);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + 50 + nGuardTypeIndex  + nGuardLevel);
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

int CSTalentInfoHelper::NormalSwordDrawingTalentID( WEAPON_TYPE nWeaponType )
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS + 30);
	case WEAPON_1H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB + 30);
	case WEAPON_1H_PIERCE:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP + 30);
	case WEAPON_TWO_HANDED: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD + 30);
	case WEAPON_2H_BLUNT:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB + 30);
	case WEAPON_STAFF:	return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF + 30);
	case WEAPON_DUAL_WIELD: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD + 30);
	case WEAPON_DUAL_PIERCE: return (MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP + 30);
	case WEAPON_ARCHERY:	return 0; //활은 발도가 없다.
	case WEAPON_NONE: return 0;
	default:
		assert(0); // 잘못된 WeaponType
		return 0;
	}
	return 0;
}

bool CSTalentInfoHelper::IsNormalAttackTalent( int nTalentID )
{
	return nTalentID >= MIN_TALENT_NORMAL_ATTACK && nTalentID <= MAX_TALENT_NORMAL_ATTACK;
}

bool CSTalentInfoHelper::IsPlayerNormalTalentID( int nSKillID )
{
	// 	if ((nSKillID >= MIN_TALENT_NORMAL_ATTACK) && (nSKillID <= MAX_TALENT_NORMAL_ATTACK))
	// 	{
	// 		return true;
	// 	}
	return false;
}

bool CSTalentInfoHelper::IsImmediateTalent( CSTalentInfo* pTalentInfo )
{
	if ((pTalentInfo->m_nTalentType == TT_SKILL) && (pTalentInfo->m_fCastingTime <= 0))
	{
		if ((pTalentInfo->m_nSkillType != ST_NONE) && (pTalentInfo->m_nSkillType != ST_PASSIVE) && (pTalentInfo->m_nSkillType != ST_EXTRA_PASSIVE))
		{
			return true;
		}
	}
	return false;
}

bool CSTalentInfoHelper::IsCollidableSkillType( TALENT_SKILL_TYPE nSkillType )
{
	if (nSkillType == ST_MELEE || 
		nSkillType == ST_MAGIC ||
		nSkillType == ST_EXTRA_ACTIVE ||
		nSkillType == ST_SOCIAL) return true;
	return false;
}

/// 직선
vec3 CSTalentInfoHelper::MoveStraight(vec3 vStartPos, vec3 vVelocity, float fElapsedTime)
{
	// 직선 운동(중력 없음)
	vec3 vPos = vStartPos + (vVelocity * fElapsedTime);
	return vPos;
}

/// 포물선
vec3 CSTalentInfoHelper::MoveParabolic(vec3 vStartPos, vec3 vVelocity, float fWeight, float fElapsedTime)
{
	// 포물선 운동
	// 공기 저항 빠짐
	vec3 vEnv			= vec3(0.0f, 0.0f, GRAVITY * fWeight);		// 중력값
	vec3 vDirX			= vVelocity;
	vec3 vDirY			= vVelocity;
	vDirX.z				= 0.0f;											// 높이값이 빠진 값
	vDirY.x	= vDirY.y	= 0.0f;											// 높이값만 가진 값

	vec3 vPos			= vStartPos + ((vDirX + vDirY) * fElapsedTime) + (0.5f * vEnv * fElapsedTime * fElapsedTime);

	return vPos;
}

/// 유도(다이렉트 회전)
vec3 CSTalentInfoHelper::MoveInduction(vec3& vOutDir, vec3 vCurrentPos, vec3 vTargetPos, vec3 vVelocity, float fTickTime)
{
	// 방향 회전
	vec3 vDir = vTargetPos - vCurrentPos;
	vDir.Normalize();
	vOutDir = vDir;

	float fSpeed = vVelocity.Length();

	// 이동
	vec3 vPos = vCurrentPos + (vOutDir * fSpeed * fTickTime);

	return vPos;
}


vec3 CSTalentInfoHelper::MoveSmooth(vec3 vCurrDir, vec3& vOutDir, vec3 vCurrentPos, vec3 vTargetPos, vec3 vVelocity, float fTickTime, float fStartDist)
{
	// 방향 회전
	vec3 vDir = vTargetPos - vCurrentPos;
	float dist = vDir.Length();

	float r = (1 - dist / fStartDist);
	if (r < 0.05) r = 0.05f;
	if (r > 1) r = 1.0f;

	vCurrDir.Normalize();
	vDir.Normalize();
	vOutDir = vCurrDir * (1 - r) + vDir * r;
	vOutDir.Normalize();

	float fSpeed = vVelocity.Length();

	// 이동
	vec3 vPos = vCurrentPos + (vOutDir * fSpeed * fTickTime);

	return vPos;
}


bool CSTalentInfoHelper::CheckEffectiveRange( vec3& vTargetPos, vec3& vMyPos, float fTalentRange )
{
	// 사거리가 0이면 무제한으로 인정한다.
	if(fTalentRange == 0)
	{		
		return true;
	}

	return ((vTargetPos - vMyPos).Length() <= fTalentRange);
}

int CSTalentInfoHelper::SocialMotionTalentID( CSSocialMotionType nSocialMotionType )
{
	switch (nSocialMotionType)
	{
	case SOCIAL_MOTION_BEG:			return 72001;
	case SOCIAL_MOTION_BOW:			return 72002;
	case SOCIAL_MOTION_CHEER:		return 72003;
	case SOCIAL_MOTION_CLAP:		return 72004;
	case SOCIAL_MOTION_CRY:			return 72005;
	case SOCIAL_MOTION_DANCE:		return 72006;
	case SOCIAL_MOTION_IGNORE:		return 72007;
	case SOCIAL_MOTION_KISS:		return 72008;
	case SOCIAL_MOTION_LAUGH:		return 72009;
	case SOCIAL_MOTION_LOOKAROUND:	return 72010;
	case SOCIAL_MOTION_NEGATIVE:	return 72011;
	case SOCIAL_MOTION_POINT:		return 72012;
	case SOCIAL_MOTION_POSITIVE:	return 72013;
	case SOCIAL_MOTION_SALUTE:		return 72014;
	case SOCIAL_MOTION_SIGH:		return 72015;
	case SOCIAL_MOTION_TALK1:		return 72016;
	case SOCIAL_MOTION_TALK2:		return 72017;
	case SOCIAL_MOTION_TAUNT:		return 72018;
	case SOCIAL_MOTION_WAVE:		return 72019;
	default:
		_ASSERT(0);
	}

	_ASSERT(0);
	return 0;
}

bool CSTalentInfoHelper::IsPlayerNormalAttackTalent( int nTalentID )
{
	int nCal = 0;

	// 기본 공격 탤런트인가? 1타 ~ 5타
	if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWP;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_DWD;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HB;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_ARC;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_STF;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_2HD;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HP;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HB;
	}
	else if(nTalentID > MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS)
	{
		nCal = nTalentID - MIN_PLAYER_NORMAL_ATTACK_TALENT_ID_1HS;
	}

	if(nCal > (NA_NORMAL1) && nCal < (NA_NORMAL6 + 1))
		return true;

	return false;
}

bool CSTalentInfoHelper::IsInitialLearnedExtraPassiveTalent( TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType )
{
	switch (nExtraPassiveType)
	{
	case TEPT_3COMBO:
	case TEPT_SWORD_DRAWING:
	case TEPT_UPPERCUT:
	case TEPT_COUNTER:
		{
			return true;
		}
	}

	return false;
}



