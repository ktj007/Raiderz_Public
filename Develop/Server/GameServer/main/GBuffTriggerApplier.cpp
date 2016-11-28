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
		return true; // Triggerd 설정이 없음

	GBuffInfo* pTriggeredBuff = gmgr.pBuffInfoMgr->Get(EnchantTriggeredInfo.nBuffID);
	if (!pTriggeredBuff)
		return true; // 존재 하지 않는 BuffID

	GPercentDice dice;
	if (!dice.Dice(EnchantTriggeredInfo.fRate))	
		return true; // 선택확률 실패

	// triggered buff duration. this will be modified in just below.
	float fDuration = pTriggeredBuff->m_fDuration;

	if (pTriggeredBuff->IsAuraEffectBuff())	// aura effect duration is always 1 second (refer GBuffInfo::Cooking()).
	{
		// for aura buffs, add trigerring period from parent buff. this will prevent from repeating short-time buff gain/lost.
		fDuration += pBuffInfo->m_fPeriod;
	}
	else if (pTriggeredBuff->IsInstantEffect() && !pTriggeredBuff->IsExistModValue())	// instant effect = 0 seconds duration (refer GBuffInfo::IsInstantEffect()).
	{
		// change to non-instant effect (duration > 0) and let players gain like normal buff. so client just can cause visual effect for this buff.
		fDuration = 0.001f;
	}

	GEntitySync* pOwnerEntity = m_pOwner->GetOwner();
	MUID uidOwner = pOwnerEntity->GetUID();
	if (pOwnerEntity->IsBuffEntity())
	{
		GEntityActor* pOwnerActor = static_cast<GBuffEntity*>(pOwnerEntity)->GetOwner();
		if (!pOwnerActor)
			return false; // 사용자 액터가 존재하지 않음

		uidOwner = pOwnerActor->GetUID();
	}

	MUID uidCaster = m_pOwner->GetUserUID();
	GEntitySync* pCaster = pField->FindActor(uidCaster);

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
	vector<GEntityActor*> vecEffecteeTarget = 
		m_EffecteeTargetSelector.Select(desc);

	// MUID uidUser = uidOwner;
	GEntitySync* pUser = pOwnerEntity;
	if (EnchantTriggeredInfo.infoEffect.m_nRelation == CSEffectInfo::RELATION_CASTER_ENEMY)
	{
		// uidUser = uidCaster;
		pUser = pCaster;
	}

	for each (GEntityActor* pEffecteeTarget in vecEffecteeTarget)
	{
		if (pEffecteeTarget->IsDead() ||
			pEffecteeTarget->IsDie())
			continue;

		pEffecteeTarget->GetModuleBuff()->LazyGainBuff(pTriggeredBuff, fDuration, pTriggeredBuff->m_fPeriod, GBuffUser(pUser, pEffecteeTarget, pTriggeredBuff));
	}

	if (!vecEffecteeTarget.empty())
	{
		m_pOwner->OnTriggeredBuffAffected();
	}
	return true;
}