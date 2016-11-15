#include "StdAfx.h"
#include "GBuffTriggerApplier.h"
#include "GBuff.h"
#include "GEntityActor.h"
#include "GGlobal.h"
#include "GBuffInfo.h"
#include "GModuleBuff.h"
#include "GBuffEntity.h"
#include "GField.h"

GBuffTriggerApplier::GBuffTriggerApplier(GBuff* pOwner)
: m_pOwner(pOwner)
{
}

GBuffTriggerApplier::~GBuffTriggerApplier(void)
{
}

bool GBuffTriggerApplier::OnEvent( TALENT_CONDITION nTiming )
{
	return ApplyEffect();
}

bool GBuffTriggerApplier::ApplyEffect()
{
	VALID_RET(m_pOwner, false);
	GBuffInfo* pBuffInfo = m_pOwner->GetInfo();
	VALID_RET(pBuffInfo, false);
	VALID_RET(m_pOwner->GetOwner(), false);
	GField* pField = m_pOwner->GetOwner()->GetField();
	if (!pField)
		return false;
	
	const GBuffInfo::TriggeredBuff& EnchantTriggeredInfo = pBuffInfo->m_TriggeredBuff;
	if (EnchantTriggeredInfo.nBuffID == INVALID_BUFF_ID)
		return false; // Triggerd 설정이 없음

	GBuffInfo* pTriggeredBuff = gmgr.pBuffInfoMgr->Get(EnchantTriggeredInfo.nBuffID);
	if (!pTriggeredBuff)
		return false; // 존재 하지 않는 BuffID

	GPercentDice dice;
	if (!dice.Dice(EnchantTriggeredInfo.fRate))	
		return false; // 선택확률 실패

	GEntitySync* pOwnerEntity = m_pOwner->GetOwner();
	GEntitySync* pCaster = NULL;
	MUID uidUser = pOwnerEntity->GetUID();
	if (pOwnerEntity->IsBuffEntity())
	{
		GEntityActor* pOwnerActor = static_cast<GBuffEntity*>(pOwnerEntity)->GetOwner();
		if (!pOwnerActor)
			return true; // 사용자 액터가 존재하지 않음

		uidUser = pOwnerActor->GetUID();
	}
	
	pCaster = pField->FindActor(m_pOwner->GetUserUID());

	GEntityActor* pLastHitOrHittedEnemy = NULL;

	if (pOwnerEntity->IsActor())
	{
		GEntityActor* pActor = ToEntityActor(pOwnerEntity);
		MUID uidLastHitOrHittedEnemy = pActor->GetLastHitOrHittedEnemy();
		pLastHitOrHittedEnemy = pActor->FindActor(uidLastHitOrHittedEnemy);
	}
	

	GEffectTargetSelector m_EffecteeTargetSelector;
	GEffectTargetSelector::Desc desc(EnchantTriggeredInfo.infoEffect);
	desc.pSelf = pOwnerEntity;
	desc.pTarget = pLastHitOrHittedEnemy;
	desc.pCaster = pCaster;
	set<GEntityActor*> vecEffecteeTarget;
	m_EffecteeTargetSelector.Select(desc, vecEffecteeTarget);

	for each (GEntityActor* pEffecteeTarget in vecEffecteeTarget)
	{
		if (pEffecteeTarget->IsDead() ||
			pEffecteeTarget->IsDie())
			continue;

		pEffecteeTarget->GetModuleBuff()->LazyGainBuff(pTriggeredBuff, pTriggeredBuff->m_fDuration, pTriggeredBuff->m_fPeriod, uidUser);
	}

	if (!vecEffecteeTarget.empty())
	{
		m_pOwner->OnTriggeredBuffAffected();
	}
	return false;
}