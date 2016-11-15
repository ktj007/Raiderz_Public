#pragma once

#include "CSTalentInfo.h"
#include "CSSocialMotion.h"
#include "CSCommonLib.h"

class CSCOMMON_API CSTalentInfoHelper
{
private:
	static int _NormalAttackTalentID(WEAPON_TYPE nWeaponType, TALENT_NORMAL_ATTACK_TYPE nAttackType);
	static int NormalAdvanceAttackTalentID(WEAPON_TYPE nWeaponType);
	static int NormalRisingComboTalentID(WEAPON_TYPE nWeaponType);
	static int NormalRisingAttackTalentID(WEAPON_TYPE nWeaponType);
		static int NormalUpperAttackTalentID(WEAPON_TYPE nWeaponType);
	static int NormalDownAttackTalentID(WEAPON_TYPE nWeaponType);
	static int NormalSuperAttackTalentID(WEAPON_TYPE nWeaponType);
	static int NormalCounterAttackTalentID(WEAPON_TYPE nWeaponType, bool bEquipShield);
public:
	/// 기본 공격 탤런트 ID 반환
	static int NormalAttackTalentID(WEAPON_TYPE nWeaponType, TALENT_NORMAL_ATTACK_TYPE nAttackType, bool bEquipShield=true);
	///// 기본 방어 탤런트 ID 반환
	static int NormalGuardTalentID(WEAPON_TYPE nWeaponType, int nGuardLevel, bool bIsEquippedShield);

	static int NormalSwordDrawingTalentID(WEAPON_TYPE nWeaponType);

	static bool IsNormalAttackTalent(int nTalentID);

	static bool IsPlayerNormalAttackTalent(int nTalentID);

	/// 플레이어 기본 탤런트인지 여부 반환
	static bool IsPlayerNormalTalentID(int nSKillID);

	/// 서버에 인증없이 클라이언트가 즉시 사용하는 탤런트인지 여부
	static bool IsImmediateTalent(CSTalentInfo* pTalentInfo);

	/// Collision값으로 판정이 가능한 스킬타입인지 여부
	static bool IsCollidableSkillType(TALENT_SKILL_TYPE nSkillType);

	/// 소셜 모션 탤런트 ID 반환
	static int SocialMotionTalentID(CSSocialMotionType nSocialMotionType);

	/// 캐릭터 생성시 미리 배운 엑스트라 탤런트인지 여부
	static bool IsInitialLearnedExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType);

	// 이동 패턴 -------------------------------------------------

	/// 직선
	static vec3 MoveStraight(vec3 vStartPos, vec3 vVelocity, float fElapsedTime);
	/// 포물선
	static vec3 MoveParabolic(vec3 vStartPos, vec3 vVelocity, float fWeight, float fElapsedTime);
	/// 유도(다이렉트 회전)
	static vec3 MoveInduction(vec3& vOutDir, vec3 vCurrentPos, vec3 vTargetPos, vec3 vVelocity, float fTickTime);
	static vec3 MoveSmooth(vec3 vCurrDir, vec3& vOutDir, vec3 vCurrentPos, vec3 vTargetPos, vec3 vVelocity, float fTickTime, float fStartDist);
	static vec3 MoveSmoothInTime(vec3 vCurrDir, vec3& vOutDir, vec3 vCurrentPos, vec3 vTargetPos, float fTickTime, float fStartDist, float& fRemainTime);

	// 사거리가 유효한가?
	static bool CheckEffectiveRange(vec3& vTargetPos, vec3& vMyPos, float fTalentRange);

	static int NormalDodgeFrontTalentID(WEAPON_TYPE nWeaponType);
	static int NormalDodgeBackTalentID(WEAPON_TYPE nWeaponType);
	static int NormalDodgeLeftTalentID(WEAPON_TYPE nWeaponType);
	static int NormalDodgeRightTalentID(WEAPON_TYPE nWeaponType);
};