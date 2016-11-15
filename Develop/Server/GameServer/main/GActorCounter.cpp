#include "StdAfx.h"
#include "GActorCounter.h"
#include "GEntityActor.h"
#include "GTalentEffector.h"

GActorCounter::GActorCounter( GEntityActor* pOwner )
: m_pOwner(pOwner)
, m_pUsingTalentInfo(NULL)
, m_bImmunedTalent(false)
{
	m_pOwner->AttachObserver(this);
}

GActorCounter::~GActorCounter(void)
{
	m_pOwner->DetachObserver(this);
}

void GActorCounter::Clear()
{
	m_pUsingTalentInfo = NULL;
	m_bImmunedTalent = false;
}

bool GActorCounter::IsCounter() const
{
	if (!m_pUsingTalentInfo)
		return false;

	if (!m_bImmunedTalent)
		return false;

	return true;
}

void GActorCounter::OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
//	DCHECK(!m_pUsingTalentInfo && L"탤런트가 끝나지 않았는데 또 사용됨");
	m_pUsingTalentInfo = pTalentInfo;
}

void GActorCounter::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	DCHECK(m_pUsingTalentInfo == pTalentInfo && L"시작될 때와 다른 탤런트가 완료되었음");
	Clear();
}

void GActorCounter::OnTalentImmuned( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	m_bImmunedTalent = true;
}

void GActorCounter::OnTalentAvoid( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	m_bImmunedTalent = true;
}

void GActorCounter::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	if (!IsCounter())
		return;	// 카운터 준비가 되지 않음

	if (m_pUsingTalentInfo != pTalentInfo)
		return;	// 다른 탤런트로 카운터를 먹일 수 없음

	// 카운터 공격
	GTalentEffector::Info info;
	info.pTalentInfo = m_pUsingTalentInfo;
	info.pUser = m_pOwner;
	info.pTarget = pTarget;
	info.nTalentCondition = TC_COUNTER;
	GTalentEffector effector;
	effector.ApplyTalent(info);
}

