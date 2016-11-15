#include "stdafx.h"
#include "CSTalentInfo.h"
#include <algorithm>
#include "MCsvParser.h"
#include "CSChrInfo.h"
#include "MUtil.h"
#include "CSCommonParser.h"
#include "CSStrings.h"
#include "CSTalentDef.h"

#define TEAT_SPAWN_COOLTIME 300.0f
#define TEAT_SPAWN_DEPAWNTIME 60

//////////////////////////////////////////////////////////////////////////
bool CSTalentInfo::IsPassiveSkill() const
{
	if ((ST_PASSIVE == m_nSkillType) || (ST_EXTRA_PASSIVE == m_nSkillType))	return true;
	return false;
}

bool CSTalentInfo::IsNeedTarget() const
{
	switch (m_nSkillType)
	{
	case ST_MAGIC:
	case ST_EXTRA_ACTIVE:
		{
			if (m_EffectInfo.m_nPoint == CSEffectInfo::POINT_TARGET)		
				return true;
		}
		break;
	case ST_ARCHERY:
		return true;
	}

	return false;
}

bool CSTalentInfo::IsMagicDamage() const
{
	return (m_fWeaponApplyRate < 0.001f);
}

void CSTalentInfo::Cooking()
{
	m_bCooked = true;

	// Grapple 탤런트일 경우 Weight 값 처리
	if (m_nExtraActive == TEAT_NPC_ONLY_GRAPPLE && m_nForceMF == MF_GRAPPLED)
	{
		m_nForceMFWeight = max(m_nForceMFWeight, 1000);
	}

	// Eat 탤런트일 경우 Weight 값 처리
	if (m_nExtraActive == TEAT_NPC_ONLY_EAT && m_nForceMF == MF_SWALLOWED)
	{
		m_nForceMFWeight = max(m_nForceMFWeight, 1000);
	}

	// Spawn 탤런트일 경우 기본 쿨타임 처리
	if (m_nExtraActive == TEAT_SPAWN && m_fCoolTime == 0.0f)
	{
		m_fCoolTime = TEAT_SPAWN_COOLTIME;
	}

	// Spawn 탤런트일 경우 기본 디스폰시간 처리
	if (m_nExtraActive == TEAT_SPAWN && m_nExtraActiveParam2== 0)
	{
		m_nExtraActiveParam2 = TEAT_SPAWN_DEPAWNTIME;
	}

	if (ExistMode())
	{
		for (int i = 0; i < MAX_TALENT_MODE; i++)
		{
			if (m_Modes.m_Modes[i] != NULL)
			{
				m_Modes.m_Modes[i]->m_szName = this->m_szName;

				m_Modes.m_Modes[i]->Cooking();
			}
		}
	}
}

bool CSTalentInfo::HasNextFocus()
{
	return m_nNextFocus != TFT_NONE;
}

bool CSTalentInfo::HasRequireFocus()
{
	return m_nRequireFocus != TFT_NONE;
}

bool CSTalentInfo::HasDamage()
{
	return m_nMaxDamage > 0;
}

bool CSTalentInfo::HasHealEffect()
{
	return m_nMaxHeal > 0;
}

bool CSTalentInfo::HasMotionfactor()
{
	if (m_MotionFactorGroup.IsModified())		return true;
	return m_ActorModifier.IsMotionfactorModified();
}

CSTalentInfo::CSTalentInfo()
{
	m_nID = INVALID_ID;
	m_nMode = 0;
	m_nOverride = INVALID_ID;
	m_bCooked = false;

	m_fCastingTime = 0.0f;
	m_nEnabledStanceFlag = 0;
	m_nRank = 0;
	m_nTalentLine = 0;
	m_nCooltimeLine = INVALID_ID;
	m_nGuardCrashLevel = GCL_PARTIAL;
	m_bPerfectGuardOnly = false;
	m_bPartialGuardHit = false;
	m_bCancelable = true;
	m_nHitCapsuleGroupIndex = NONE_HITCAPSULEGROUP_ID;
	m_nExtraHitCapsuleGroup = NONE_HITCAPSULEGROUP_ID;
	m_nExtraHitCapsuleGroup2 = NONE_HITCAPSULEGROUP_ID;
	m_nExtraHitCapsuleGroup3 = NONE_HITCAPSULEGROUP_ID;

	m_vCastingPos = vec3::ZERO;
	m_fCastingRot = 0.0f; 
	m_vCastingLoopPos = vec3::ZERO;
	m_fCastingLoopRot = 0.0f; 
	m_vExtraPos = vec3::ZERO;
	m_fExtraRot = 0.0f;
	m_vExtra2Pos = vec3::ZERO;
	m_fExtra2Rot = 0.0f;
	m_vExtra3Pos = vec3::ZERO;
	m_fExtra3Rot = 0.0f;

	m_nCategory = TC_COMMON;
	m_nTalentType = TT_SKILL;
	m_nSkillType = ST_NONE;
	m_nAICategories = 0;
	m_nStyle = TS_NONE;
	m_nExtraPassive = TEPT_NONE;
	m_nExtraPassive2 = TEPT_NONE;
	m_nExtraPassiveParam = 1;
	m_nExtraPassiveParam2 = 1;
	m_nExtraActive = TEAT_NONE;
	m_nExtraActiveParam1 = 1;
	m_nExtraActiveParam2 = 0;
	m_nTiming = TC_USE_TALENT;
	m_bIgnoreMesmerize = false;
	m_bAvailableOnGuard = false;

	m_nGuardDirection = GUARD_DIRECTION_FRONT;
	//		m_nAttackType = SA_NONE;
	m_nDamageAttrib = DA_NONE;
	m_nMinDamage = m_nMaxDamage = 0;
	m_nMinHeal = m_nMaxHeal = 0;

	m_fHitStartTime = 0.0f;
	m_fHitDurationTime = 0.0f;
	m_vHitPosition = vec3::ZERO;
	m_fHitSize = 0.0f;

	m_nHPCost = 0;
	m_nENCost = 0;
	m_nSTACost = 0;
	m_fCoolTime = 0.0f;
	m_fDurationTime = 0.0f;
	
	m_bSetDirection = true;
	m_bUseAiming = true;

	m_fHitDelay = 0.0f;

	m_nDodgeDifficulty = 230;
	m_nHateAdj = 0;
	m_fHateFactor = 1.0f;

	m_bUseBloodEffect = false;

	m_fExtraMotionTime = 0.0f;
	m_fExtraMotionTime2 = 0.0f;
	m_fExtraMotionTime3 = 0.0f;
//	m_fHitMotionDelay = 0.0f;
	m_bAffectedMotionSpd = false;

	m_bUntrainable = true;

	m_bCriticalEnable = false;
	m_fWeaponApplyRate = 0.0f;
	m_WeaponReference = WR_RIGHT;
	m_fCriticalApplyRate = 1.0f;	
	m_fExtraPhaseTime = 2.5f;
	m_fExtraPhaseTime2 = 0.0f;
	m_fExtraPhaseTime3 = 0.0f;
	m_bUseInvincibleTime = false;
	m_bUseSuperarmorTime = false;
	m_fPostDelay = 0.0f;
	m_fInvincibleTimeStart = 0.0f;
	m_fInvincibleTimeEnd = 0.0f;
	m_fSuperarmorTimeStart = 0.0f;
	m_fSuperarmorTimeEnd = 0.0f;
	m_bSeparateHit = false;
	m_bHitCapsulePosSync = false;
	m_fRange = 0.0f;

	m_bAvoidable = true;
	m_bUseAvoidTime = false;
	m_fAvoidTimeStart = 0.0f;	// 회피 시작 시간
	m_fAvoidTimeEnd = 0.0f;	// 회피 종료 시간

	m_nForceMF = MF_NONE;
	m_nForceMFWeight = 0;

	m_bNeedTraining = true;

	m_nTNeedLevel = 0;
	m_nTNeedTalent = 0;
	m_nTNeedStyleTP = 0;

	// 포커스 -------------------------
	m_nRequireFocus = TFT_NONE;
	m_nNextFocus = TFT_NONE;

	m_fRequireMoveSpeed = -1.0f;

	m_bValidation = true;

	m_bByLicense = false;
	m_bUsableAtDead = false;

	m_nMovableType = MUT_NONE;

	m_eTargetingType = TTT_STICKY;	
}

CSTalentInfo* CSTalentInfo::GetMode( int nMode )
{
	if (m_Modes.m_bExistMode == false) return NULL;
	if (nMode < 0 || nMode >= MAX_TALENT_MODE) return NULL;
	
	return m_Modes.m_Modes[nMode];
}

void CSTalentInfo::InsertMode( CSTalentInfo* pModeTalentInfo )
{
	if (pModeTalentInfo->m_nMode < 0 || pModeTalentInfo->m_nMode >= MAX_TALENT_MODE) 
	{
		_ASSERT(0);
		return;
	}

	if (pModeTalentInfo != NULL)
	{
		m_Modes.m_bExistMode = true;
	}

	m_Modes.m_Modes[pModeTalentInfo->m_nMode] = pModeTalentInfo;
}

bool CSTalentInfo::IsPhysicalAttack(void) const
{
	return m_nCategory == TC_MELEE || m_nCategory == TC_RANGE;
}

bool CSTalentInfo::IsToggle()
{
	switch (m_nExtraActive)
	{
	case TEAT_TOGGLE_PRECISION: 	return true;
	case TEAT_TOGGLE_SNEAK:			return true;
	}

	return false;
}

int CSTalentInfo::GetToggleBuffID()
{
	//
	// 주의: "CSTalentInfo::IsToggle()" 에도 추가해야합니다.
	//
	switch (m_nExtraActive)
	{
	case TEAT_TOGGLE_PRECISION: 	return BUFF_FOCUS_ID[TFT_PRECISION];
	case TEAT_TOGGLE_SNEAK:			return BUFF_FOCUS_ID[TFT_SNEAK];
	}

	return INVALID_BUFF_ID;
}

bool CSTalentInfo::IsSelfRebirth() const
{
	if (TEAT_REBIRTH != m_nExtraActive) return false;
	if (CSEffectInfo::POINT_SELF != m_EffectInfo.m_nPoint) return false;
	if (CSEffectInfo::RELATION_ALLIED_DEAD != m_EffectInfo.m_nRelation) return false;

	return true;		
}

CSTalentHitInfo::CSTalentHitInfo() :
m_nID(0)
,m_nReferenceID(0)
, m_nReferenceMode(0)
{
}

bool CSTalentHitInfo::TestCollision(MCapsule& c)
{
	for (size_t i = 0; i < m_vSegments.size(); ++i)
	{
		for (size_t j = 0; j < m_vSegments[i].m_vCapsules.size(); ++j)
		{
			if (m_vSegments[i].m_vCapsules[j].CollisionCapsule(c)) return true;
		}
	}

	return false;
}

void CSTalentHitInfo::Clear()
{
	m_nID = 0;
	m_nReferenceID = 0;
	m_nReferenceMode = 0;
	m_vSegments.clear();
}

CSTalentInfoModes::CSTalentInfoModes()
{
	m_bExistMode = false;
	for (int i = 0; i < MAX_TALENT_MODE; i++)
	{
		m_Modes[i] = NULL;
	}
}

CSTalentInfoModes::~CSTalentInfoModes()
{
	for (int i = 0; i < MAX_TALENT_MODE; i++)
	{
		if (m_Modes[i] != NULL)
		{
			SAFE_DELETE(m_Modes[i]);
		}
	}
}

void CSTalentInfoModes::ClearButNotDelete()
{
	m_bExistMode = false;
	for (int i = 0; i < MAX_TALENT_MODE; i++)
	{
		m_Modes[i] = NULL;
	}
}


