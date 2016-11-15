#include "stdafx.h"
#include "GRegenUpdater.h"
#include "GEntityActor.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GModuleCombat.h"
#include "GDef.h"
#include "GConst.h"
#include "GNPCLoot.h"
#include "GPlayerDoing.h"
#include "GPlayerSit.h"

GRegenUpdater::GRegenUpdater() : m_fHPRegenModValue(0.0f), m_fENRegenModValue(0.0f), m_fSTARegenModValue(0.0f), m_bActive(true)
{
	float fTick = (float)(GConst::REGEN_TICK);

	m_RegenRegulator.SetTickTime(fTick);
}

void GRegenUpdater::Update(float fDelta, GEntityActor* pActor)
{
	if (!m_bActive) return;
	if (pActor->IsDead()) return;
	if (pActor->IsDisableRegen()) return;

	GModuleCombat* pModuleCombat = pActor->GetModuleCombat();
	if (pModuleCombat == NULL) return;

	if (m_RegenRegulator.IsReady(fDelta) == false) return;

	// HP 리젠
	bool bHPRegenUpdate = UpdateHPRegen(pActor);

	// EN 리젠
	bool bENRegenUpdate = UpdateENRegen(pActor);

	// STA 리젠 - 플레이어만 쓴다.
	bool bSTARegenUpdate = UpdateSTARegen(pActor);


	if (bHPRegenUpdate || bENRegenUpdate || bSTARegenUpdate)
	{
		pActor->OnUpdatedStatus();
	}
}

float GRegenUpdater::CalcAmount( GEntityActor* pActor, float fRegenAmount, float fRegenModValue, float fRegenRateInCombat, float fPlayerSittingRate )
{
	if (pActor->IsNowCombat())
		fRegenAmount *= fRegenRateInCombat;

	fRegenAmount += fRegenModValue;

	if (pActor->IsPlayer())
	{
		GEntityPlayer* pPlayer = static_cast<GEntityPlayer*>(pActor);
		if (pPlayer->GetSit().IsNowSit())
		{
			fRegenAmount = fRegenAmount * fPlayerSittingRate;
		}
	}
	return fRegenAmount;
}

bool GRegenUpdater::UpdateHPRegen( GEntityActor* pActor )
{
	if (pActor->GetHP() >= pActor->GetMaxHP()) return false;

	float fRegenAmount = static_cast<float>(pActor->GetHPRegen());
	if (fRegenAmount <= 0.001f) return false;

	fRegenAmount = CalcAmount(pActor, fRegenAmount, m_fHPRegenModValue, GConst::HP_REGEN_RATE_IN_COMBAT, GConst::SITTING_REGEN_RATE);

	m_fHPRegenModValue = (fRegenAmount - static_cast<int>(fRegenAmount)); // 나머지값 저장
	int nRegenAmount = static_cast<int>(fRegenAmount - m_fHPRegenModValue);

	if (true == pActor->IsNPC())
	{
		int nMaxRecoverableHP = pActor->GetMaxHP() - pActor->GetHP();
		if (nMaxRecoverableHP < nRegenAmount)
		{
			nRegenAmount = nMaxRecoverableHP;
		}
	}


	pActor->SetHP(pActor->GetHP() + nRegenAmount);

	return true;
}

bool GRegenUpdater::UpdateENRegen( GEntityActor* pActor )
{
	if (pActor->GetEN() >= pActor->GetMaxEN()) return false;

	float fRegenAmount = static_cast<float>(pActor->GetENRegen());
	if (fRegenAmount <= 0.001f) return false;

	fRegenAmount = CalcAmount(pActor, fRegenAmount, m_fENRegenModValue, GConst::EN_REGEN_RATE_IN_COMBAT, GConst::SITTING_REGEN_RATE);

	m_fENRegenModValue = (fRegenAmount - static_cast<int>(fRegenAmount)); // 나머지값 저장

	pActor->SetEN(pActor->GetEN()+static_cast<int>(fRegenAmount));

	return true;
}

bool GRegenUpdater::UpdateSTARegen( GEntityActor* pActor )
{
	if (!pActor->IsPlayer()) return false;

	if (pActor->GetSTA() >= pActor->GetMaxSTA()) return false;

	if (pActor->IsGuarding()) return true;

	float fRegenAmount = static_cast<float>(pActor->GetSTARegen());

	fRegenAmount = CalcAmount(pActor, fRegenAmount, m_fSTARegenModValue, GConst::STA_REGEN_RATE_IN_COMBAT, GConst::SITTING_REGEN_RATE);

	m_fSTARegenModValue = (fRegenAmount - static_cast<int>(fRegenAmount)); // 나머지값 저장

	pActor->SetSTA(pActor->GetSTA()+static_cast<int>(fRegenAmount));


	return true;
}

