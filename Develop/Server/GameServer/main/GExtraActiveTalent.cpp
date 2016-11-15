#include "stdafx.h"
#include "GExtraActiveTalent.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "CCommandTable.h"
#include "GTalentHitter.h"
#include "GTalentRouter.h"
#include "GModuleCombat.h"
#include "GNPCSwallow.h"
#include "GuardEffector.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// GExtraActiveTalent ////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
// GExtraActiveTalent_Grapple ////////////////////////////////////////////////////////////////

GExtraActiveTalent_Grapple::GExtraActiveTalent_Grapple( GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress/*=true*/ ) 
: GTalent(pOwner, pTalentInfo, Target, bGainStress)
{

}

void GExtraActiveTalent_Grapple::OnStart()
{
	// 히트캡슐 타겟만 유효합니다. (강제 고정)
	m_pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_HITCAPSULE;

	GTalent::OnStart();
}

void GExtraActiveTalent_Grapple::OnHitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex )
{
	if (pVictim->IsNPC())
		return; // NPC는 잡을 수 없음

	// 이 탤런트는 딱 1명만 판정될 수 있다.
	if (m_TalentHit.HasVictim()) return;

	GTalentHitter_Instant* pNewHitter = GTalentHitter::NewTalentHitter< GTalentHitter_Instant >();
	pNewHitter->Setup(m_pOwner->GetUID(), m_pTalentInfo, pVictim, 0, nCapsuleGroupIndex, nCapsuleIndex);
	if (m_pOwner->GetModuleCombat()) m_pOwner->GetModuleCombat()->AddTargetHitter(pNewHitter);

	if ( pVictim->IsNowInvincibility())
		return;

	GuardEffector guard_effector;
	if (GuardEffector::GUARD_SUCCESS == guard_effector.CheckGuard(pVictim, m_pOwner, m_pTalentInfo))
		return; // 가드 성공

	if (pVictim->IsHittable(m_pOwner, m_pTalentInfo) == false) return;

	if (pVictim->GetCurrentMotionFactor() == MF_STUN) return;
	if (pVictim->GetActionState() == AS_SWIMMING) return;

	// 잡기에 걸리면 사용중인 탤런트 취소
	pVictim->doCancelTalentForce(false);

	ChangePhase(TALENT_PHASE_EXTRA);
}

void GExtraActiveTalent_Grapple::OnEnterPhaseExtra()
{
	__super::OnEnterPhaseExtra();

	// 탤런트 만료시간을 ExtraPhaseTime만큼 시간 연장
	m_TalentExpirer.SetExpiredTime(m_fElapsedTime+m_pTalentInfo->m_fExtraPhaseTime);

	m_pRouter->RouteExtraActTalent(m_pTalentInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GExtraActiveTalent_Swallowed ////////////////////////////////////////////////////////////////

GExtraActiveTalent_Swallowed::GExtraActiveTalent_Swallowed( GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress/*=true*/ ) 
: GTalent(pOwner, pTalentInfo, Target, bGainStress)
{

}

void GExtraActiveTalent_Swallowed::OnStart()
{
	// 히트캡슐 타겟만 유효합니다. (강제 고정)
	m_pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_HITCAPSULE;

	GTalent::OnStart();
}

void GExtraActiveTalent_Swallowed::OnHitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex )
{
	if (!m_pOwner->IsNPC())
		return;	// NPC만 먹을 수 있음

	if (pVictim->IsNPC())
		return; // NPC는 먹을 수 없음

	// 이 탤런트는 딱 1명만 판정될 수 있다.
	if (m_TalentHit.HasVictim()) return;

	GTalentHitter_Instant* pNewHitter = GTalentHitter::NewTalentHitter< GTalentHitter_Instant >();
	pNewHitter->Setup(m_pOwner->GetUID(), m_pTalentInfo, pVictim, 0, nCapsuleGroupIndex, nCapsuleIndex);

	if (m_pOwner->GetModuleCombat()) 
		m_pOwner->GetModuleCombat()->AddTargetHitter(pNewHitter);

	if ( pVictim->IsNowInvincibility())
		return;

	GuardEffector guard_effector;
	if (GuardEffector::GUARD_SUCCESS == guard_effector.CheckGuard(pVictim, m_pOwner, m_pTalentInfo))
		return; // 가드 성공

	if (!pVictim->IsHittable(m_pOwner, m_pTalentInfo)) 
		return;

	if (pVictim->GetCurrentMotionFactor() == MF_STUN) 
		return;

	if (pVictim->GetActionState() == AS_SWIMMING) 
		return;

	// 먹기 처리
	GEntityNPC* pOwnerNPC = ToEntityNPC(m_pOwner);
	if (!pOwnerNPC->GetNPCSwallow().Swallow(pVictim, m_pTalentInfo))
		return;	// 먹을 수 없는 상태

	// 잡기에 걸리면 사용중인 탤런트 취소
	pVictim->doCancelTalentForce(false);

	ChangePhase(TALENT_PHASE_EXTRA);
}

void GExtraActiveTalent_Swallowed::OnEnterPhaseExtra()
{
	__super::OnEnterPhaseExtra();

	m_fActTime = m_fElapsedTime; // 탤런트 판정시간을 맞추기 위해 액트시간으로 조정

	// 탤런트 만료시간을 ExtraPhaseTime만큼 시간 연장
	m_TalentExpirer.SetExpiredTime(	m_fElapsedTime+
									m_pTalentInfo->m_fExtraPhaseTime+	// 먹는 애니메이션 
									(float)m_pTalentInfo->m_nExtraActiveParam1+	// 먹고 있는 시간
									max(m_pTalentInfo->m_fExtraPhaseTime2, m_pTalentInfo->m_fExtraPhaseTime3));	// 뱉는 애니메이션

	m_rgrPhaseExtra2.SetTickTime(m_pTalentInfo->m_fExtraPhaseTime+(float)m_pTalentInfo->m_nExtraActiveParam1);

	m_pRouter->RouteExtraActTalent(m_pTalentInfo);
}

void GExtraActiveTalent_Swallowed::OnUpdatePhaseExtra( float fDelta )
{
	__super::OnUpdatePhaseExtra(fDelta);

	m_fActTime = m_fElapsedTime; // 탤런트 판정시간을 맞추기 위해 액트시간으로 조정

	GEntityNPC* pOwnerNPC = ToEntityNPC(m_pOwner);
	if (pOwnerNPC->GetNPCSwallow().IsSwallowable())
	{
		// 먹을 수 있는 상태가 되면 뱉기로 변경
		ChangePhase(TALENT_PHASE_EXTRA3);
		return;
	}

	if (m_rgrPhaseExtra2.IsReady(fDelta))
	{
		ChangePhase(TALENT_PHASE_EXTRA2);
		return;
	}
}

// ==> 시간이 다 되어 뱉기
void GExtraActiveTalent_Swallowed::OnEnterPhaseExtra2()
{
	__super::OnEnterPhaseExtra2();

	m_fActTime = m_fElapsedTime; // 탤런트 판정시간을 맞추기 위해 액트시간으로 조정

	// 탤런트 만료시간을 ExtraPhaseTime2만큼 시간 연장
	m_TalentExpirer.SetExpiredTime(m_fElapsedTime+m_pTalentInfo->m_fExtraPhaseTime2);

	// 뱉기 처리
	GEntityNPC* pOwnerNPC = ToEntityNPC(m_pOwner);
	if (!pOwnerNPC->GetNPCSwallow().Spewup(false))
		return;	// 뱉을 수 없는 상태
}

// ==> 먹는 도중에 공격 받아 뱉기
void GExtraActiveTalent_Swallowed::OnEnterPhaseExtra3()
{
	__super::OnEnterPhaseExtra3();

	// 탤런트 만료시간을 ExtraPhaseTime3만큼 시간 연장
	m_TalentExpirer.SetExpiredTime(m_fElapsedTime+m_pTalentInfo->m_fExtraPhaseTime3);
}
