#include "stdafx.h"
#include "CSStrings.h"
#include "CSChrInfo.h"
#include "CSCommonParser.h"
#include "GTalentInfo.h"
#include "MUtil.h"
#include "MMath.h"
#include "GDef.h"
#include "GMath.h"
#include "GRelationChecker.h"
#include "GConst.h"
#include "GGlobal.h"
#include "GBuffInfo.h"

GTalentInfo::GTalentInfo() 
: CSTalentInfo()
, m_bExistCastingFX(false)
, m_bExistUseFX(false)
, m_fMinAITime(0.2f) // 기본은 0.2초
, m_bExistModValue(false)
, m_bExistProjectileEvent(false)
, m_bIsProjectileType(false)
, m_bBubbleHit(true)
, m_fDistanceToLongestPoint(0.0f)
, m_fExpiredTime(0.0f)
, m_fLeastProjectileDistance(0.0f)
, m_bNonCombatOnly(false)
, m_nBackHitTalent(0)
, m_bHitOverlap(true)
, m_fTalentStartRange(0.0f)
, m_bDelayedAct(false)
, m_bHasExtraActiveEvent(false)
, m_bUseOnlyGM(false)
, m_bHeal(false)
, m_bHasAttack(false)
, m_bIsDodge(false)
{
}

GTalentInfo::~GTalentInfo()
{

}

int GTalentInfo::MakeDamageFromMinMaxDamage()
{
	return (GMath::RandomNumber(m_nMinDamage, m_nMaxDamage));
}

void GTalentInfo::Cooking()
{
	CSTalentInfo::Cooking();

	InitModValues();
	InitDistanceToLongestPoint();
	InitExpiredTime();
	InitEvent();
	InitLeastProjectileDistance();
	InitHitAABB();
	InitCheckHeal();
	InitHasAttack();
	InitDodge();
}

void GTalentInfo::InitDodge()
{
	m_bIsDodge = false;

	for (int i=WEAPON_1H_SLASH; i<WEAPON_MAX; ++i)
	{
		if (CSTalentInfoHelper::NormalDodgeFrontTalentID((WEAPON_TYPE)i) == m_nID)
		{
			m_bIsDodge = true;
			return;
		}

		if (CSTalentInfoHelper::NormalDodgeBackTalentID((WEAPON_TYPE)i) == m_nID)
		{
			m_bIsDodge = true;
			return;
		}

		if (CSTalentInfoHelper::NormalDodgeLeftTalentID((WEAPON_TYPE)i) == m_nID)
		{
			m_bIsDodge = true;
			return;
		}

		if (CSTalentInfoHelper::NormalDodgeRightTalentID((WEAPON_TYPE)i) == m_nID)
		{
			m_bIsDodge = true;
			return;
		}
	}
	
}

bool ImplHasAttack(int nBuffID)
{
	GBuffInfo* pBuff = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (!pBuff)
		return false;

	if (pBuff->HasDamage())
		return true;

	GBuffInfo* pTriggeredBuff = gmgr.pBuffInfoMgr->Get(pBuff->m_TriggeredBuff.nBuffID);
	if (!pTriggeredBuff)
		return false;

	if (pTriggeredBuff->HasDamage())
		return true;

	return false;
}

void GTalentInfo::InitHasAttack()
{
	if (m_nMaxDamage > 0)
	{
		m_bHasAttack = true;
		return;
	}

	if (HasMotionfactor())
	{
		m_bHasAttack = true;
		return;
	}

	if (ImplHasAttack(m_Buff1.nID))
	{
		m_bHasAttack = true;
		return;
	}

	if (ImplHasAttack(m_Buff2.nID))
	{
		m_bHasAttack = true;
		return;
	}
}

void GTalentInfo::InitModValues()
{
	m_bExistModValue = m_InstantModifier.IsModified() || m_ActorModifier.IsModified() || m_PlayerModifier.IsModified();
}

void GTalentInfo::Clone( CSTalentInfo* pTalentInfo )
{
	GTalentInfo* pgTalentInfo = static_cast<GTalentInfo*>(pTalentInfo);
	*pgTalentInfo = *this;

	// Clone에서 복사하지 않는 항목 ---

	pgTalentInfo->m_Modes.ClearButNotDelete();		// mode
	pgTalentInfo->m_HitInfo.Clear();				// talent_hit_info.xml

}

void GTalentInfo::InitDistanceToLongestPoint()
{
	m_fDistanceToLongestPoint = 0.0f;

	if (m_fTalentStartRange > 0.0f)
	{
		m_fDistanceToLongestPoint = m_fTalentStartRange;
		return;
	}

	if (ST_GUARD == m_nSkillType)
	{
		m_fDistanceToLongestPoint = 0.0f;
		return;
	}

	// 판정구에서 가장 먼 정점을 설정
	CSTalentHitInfo& hit_info = m_HitInfo;
	for (size_t nTalentColIndex = 0; nTalentColIndex < hit_info.m_vSegments.size(); nTalentColIndex++)
	{
		for (size_t k = 0; k < hit_info.m_vSegments[nTalentColIndex].m_vCapsules.size(); ++k)
		{
			bool bHitCheck = hit_info.m_vSegments[nTalentColIndex].m_vDoHitCheck[k];
			if (!bHitCheck)
				continue;

			MCapsule capTalent = hit_info.m_vSegments[nTalentColIndex].m_vCapsules[k];
			float cap_min_x = min(capTalent.bottom.x, capTalent.top.x);
			float cap_min_y = min(capTalent.bottom.y, capTalent.top.y);

			if (IsMoving() && m_bHitCapsulePosSync)
			{
				const float fCheckTime = hit_info.m_vSegments[nTalentColIndex].m_fCheckTime;
				int nIndex = (int)(fCheckTime * 10.f);
				if (nIndex >= 0 && nIndex < (int)m_MovingInfoList.size())
				{
					_TALENT_MOVE_INFO& move_info = m_MovingInfoList[nIndex];

					cap_min_x += move_info.vPos.x;
					cap_min_y += move_info.vPos.y;
				}
			}

			// 캡슐 최소 위치에 지름을 더하기
			float cap_max_x = abs(cap_min_x) + capTalent.radius;
			float cap_max_y = abs(cap_min_y) + capTalent.radius;

			m_fDistanceToLongestPoint = max(m_fDistanceToLongestPoint, cap_max_y);
		}
	}

	const float MAGIC_MOD = 0.8f;		// 칼같이 딱 그 점이 아니라 약간 안쪽으로 하기 위함.

	m_fDistanceToLongestPoint = m_fDistanceToLongestPoint * MAGIC_MOD;
}


void GTalentInfo::InitHitAABB()
{
	// 히트박스를 invalid하게 설정
	m_bxHit.minx = 1.0f;
	m_bxHit.miny = 0.0f;
	m_bxHit.minz = 0.0f;
	m_bxHit.maxx = -1.0f;
	m_bxHit.maxy = 0.0f;
	m_bxHit.maxz = 0.0f;
	

	// 판정구에서 가장 먼 정점을 설정
	CSTalentHitInfo& hit_info = m_HitInfo;
	for (size_t nTalentColIndex = 0; nTalentColIndex < hit_info.m_vSegments.size(); nTalentColIndex++)
	{
		for (size_t k = 0; k < hit_info.m_vSegments[nTalentColIndex].m_vCapsules.size(); ++k)
		{
			bool bHitCheck = hit_info.m_vSegments[nTalentColIndex].m_vDoHitCheck[k];
			if (!bHitCheck)
				continue;

			MCapsule capTalent = hit_info.m_vSegments[nTalentColIndex].m_vCapsules[k];
			float cap_min_x = min(capTalent.bottom.x, capTalent.top.x);
			float cap_min_y = min(capTalent.bottom.y, capTalent.top.y);

			if (IsMoving() && m_bHitCapsulePosSync)
			{
				const float fCheckTime = hit_info.m_vSegments[nTalentColIndex].m_fCheckTime;
				int nIndex = (int)(fCheckTime * 10.f);
				if (nIndex >= 0 && nIndex < (int)m_MovingInfoList.size())
				{
					_TALENT_MOVE_INFO& move_info = m_MovingInfoList[nIndex];

					cap_min_x += move_info.vPos.x;
					cap_min_y += move_info.vPos.y;
				}
			}

			// 캡슐 최소 위치에 지름을 더하기
			float cap_max_x = abs(cap_min_x) + capTalent.radius;
			float cap_max_y = abs(cap_min_y) + capTalent.radius;

			// AABB로 구하기
			MBox bxSeg(
				cap_min_x - capTalent.radius,		// min_x
				cap_min_y - capTalent.radius,		// min_y
				0.0f,								// min_z
				cap_max_x,							// max_x
				cap_max_y,							// max_y
				0.0f);								// max_z

			if (!m_bxHit.IsValid())
			{
				m_bxHit = bxSeg;
			}
			else
			{
				m_bxHit.minx = min(m_bxHit.minx, bxSeg.minx);
				m_bxHit.miny = min(m_bxHit.miny, bxSeg.miny);
				m_bxHit.maxx = max(m_bxHit.maxx, bxSeg.maxx);
				m_bxHit.maxy = max(m_bxHit.maxy, bxSeg.maxy);
			}
		}
	}

	// 4방향 영역 확장
	{
		float fLongest = max(max(max(abs(m_bxHit.maxx), abs(m_bxHit.maxy)), abs(m_bxHit.minx)), abs(m_bxHit.miny));
		m_bxHit.minx -= fLongest;
		m_bxHit.miny -= fLongest;
		m_bxHit.maxx += fLongest;
		m_bxHit.maxy += fLongest;
	}
}

void GTalentInfo::InitExpiredTime()
{
	m_fExpiredTime = 0.0f;

	// 피격 판정 시간
	CSTalentHitInfo& hit_info = m_HitInfo;
	for (size_t nTalentColIndex = 0; nTalentColIndex < hit_info.m_vSegments.size(); nTalentColIndex++)
	{
		float fLeastGap = 0.001f; // 피격 판정이 있다면 최소한 한틱이라도 Update를 하게 해야함
		m_fExpiredTime = max(m_fExpiredTime, hit_info.m_vSegments[nTalentColIndex].m_fCheckTime+fLeastGap);
	}

	// 이벤트 시간
	for each (const CSTalentEventInfo& each in m_Events.m_vecActEvents)
	{
		if (each.m_nEvent != TE_LAUNCH_PROJECTILE &&
			each.m_nEvent != TE_DELAYED_ACT &&
			each.m_nEvent != TE_EXTRAACTIVE_1 &&
			each.m_nEvent != TE_EXTRAACTIVE_2)
		{
			continue; // 서버에서 의미있는 이벤트만 적용
		}

		m_fExpiredTime = max(m_fExpiredTime, each.m_fTime1);
	}

	m_ActAnimationTime;
	
	// 지속시간, 애니메이션 시간
	m_fExpiredTime = max(m_fExpiredTime, m_fDurationTime);
}

void GTalentInfo::InitEvent()
{
	m_bExistProjectileEvent = false;
	m_bIsProjectileType = false;
	m_vLowestProjectileStartPos = vec3::ZERO;

	for each (const CSTalentEventInfo& each in m_Events.m_vecActEvents)
	{
		switch (each.m_nEvent)
		{
		case TE_LAUNCH_PROJECTILE:
			m_bExistProjectileEvent = true;

			if (each.m_ProjectileInfo.m_nType != TPT_NONE)
				m_bIsProjectileType = true;

			// 가장 낮은 발사위치 구하기
			if (m_vLowestProjectileStartPos == vec3::ZERO ||
				m_vLowestProjectileStartPos.z > each.m_ProjectileInfo.m_vLocalStartPos.z)
			{
				m_vLowestProjectileStartPos = each.m_ProjectileInfo.m_vLocalStartPos;
			}

			switch(each.m_ProjectileInfo.m_nType)
			{
			case TPT_MISSILE_GUIDED:
				// 유도미사일은 효과점을 타켓으로 고정
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			}
			break;
		case TE_DELAYED_ACT:
			m_bDelayedAct = true;
			break;
		case TE_EXTRAACTIVE_1:
		case TE_EXTRAACTIVE_2:
			m_bHasExtraActiveEvent = true;
		}
		
	}
	
	// 발생시간별로 정렬
	sort(m_Events.m_vecActEvents.begin(), m_Events.m_vecActEvents.end(), CSTalentEventInfo());
}

void GTalentInfo::InitLeastProjectileDistance()
{
	m_fLeastProjectileDistance = 0.0f;

	for each (const CSTalentEventInfo& each in m_Events.m_vecActEvents)
	{
		if (each.m_nEvent == TE_LAUNCH_PROJECTILE)
		{
			float fDistance = each.m_ProjectileInfo.m_vLocalStartPos.LengthSq();

			// 가장 먼 발사거리 구하기
			if (m_fLeastProjectileDistance < fDistance)
			{
				m_fLeastProjectileDistance = fDistance;
			}
		}
	}
}

bool ImplCheckHeal(int nBuffID)
{
	GBuffInfo* pBuff = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (!pBuff)
		return false;

	if (pBuff->HasHealEffect())
		return true;

	GBuffInfo* pTriggeredBuff = gmgr.pBuffInfoMgr->Get(pBuff->m_TriggeredBuff.nBuffID);
	if (!pTriggeredBuff)
		return false;

	if (pTriggeredBuff->HasHealEffect())
		return true;

	return false;
}

void GTalentInfo::InitCheckHeal()
{
	m_bHeal = false;

	// 마법진 체크
	if (m_nExtraActive == TEAT_BUFF_ENTITY)
	{
		m_bHeal = ImplCheckHeal(m_nExtraActiveParam1);
		if (m_bHeal)
			return;
	}

	if (HasBuff())
	{
		m_bHeal = ImplCheckHeal(m_Buff1.nID);
		if (m_bHeal)
			return;

		m_bHeal = ImplCheckHeal(m_Buff2.nID);
		if (m_bHeal)
			return;
	}

	if (m_EffectInfo.m_nRelation == CSEffectInfo::RELATION_SELF)
		return;  // 자기자신만 해당

	if (m_EffectInfo.m_nPoint == CSEffectInfo::POINT_SELF &&
		m_EffectInfo.m_fRadius <= 0.0f)
		return;  // 자기자신만 해당

	if (HasHealEffect())
	{
		m_bHeal = true;
		return;
	}
}

bool GTalentInfo::IsEffective(GEntityActor* pReqActor, GEntityActor* pTarActor)
{
	VALID_RET(pReqActor, false);
	VALID_RET(pTarActor, false);

	GRelationChecker relationChecker;

	switch (m_EffectInfo.m_nRelation)
	{
	case CSEffectInfo::RELATION_ALL:
		{
			if (true == relationChecker.IsAll(pReqActor, pTarActor)) return true;
		}
		break;
	case CSEffectInfo::RELATION_ENEMY:
		{
			if (true == relationChecker.IsEnemy(pReqActor, pTarActor)) return true;
		}
		break;
	case CSEffectInfo::RELATION_ALLIED:
		{
			if (true == relationChecker.IsAlly(pReqActor, pTarActor)) return true;
		}
		break;	
	case CSEffectInfo::RELATION_PARTY:
		{
			if (true == relationChecker.IsParty(pReqActor, pTarActor)) return true;
		}
		break;
	case CSEffectInfo::RELATION_ALLIED_DEAD:
		{
			if (true == relationChecker.IsAllyDead(pReqActor, pTarActor)) return true;
		}
		break;
	case CSEffectInfo::RELATION_SELF:
		{
			if (pReqActor == pTarActor) return true;
		}
		break;
	}

	return false;

}

bool GTalentInfo::IsMoving()
{
	if (m_vExtraPos != vec3::ZERO)
		return true;

	if (m_vExtra2Pos != vec3::ZERO)
		return true;

	if (m_vExtra3Pos != vec3::ZERO)
		return true;

	if (m_fExtraRot != 0.0f)
		return true;

	if (m_fExtra2Rot != 0.0f)
		return true;

	if (m_fExtra3Rot != 0.0f)
		return true;

	return !m_MovingInfoList.empty();
}

bool GTalentInfo::IsMovingWithCasting()
{
	switch (m_nMovableType)
	{
	case MUT_CASTING:
	case MUT_ALL:
		return true;
	}

	return false;
}

bool GTalentInfo::IsNPCTalent()
{
	return (!m_setNPCID.empty());
}

bool GTalentInfo::IsPortalTalent()
{
	return (TEAT_PORTAL == m_nExtraActive || TEAT_PORTAL_SOULBINDING == m_nExtraActive);
}

bool GTalentInfo::HasHeal() const
{
	return m_bHeal;
}
