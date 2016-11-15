#include "StdAfx.h"
#include "GBuffModifierApplier.h"
#include "GEntityPlayer.h"
#include "GBuff.h"
#include "GBuffInfo.h"
#include "GBuffRequirement.h"
#include "CSTalentInfoEnum.h"

GBuffModifierApplier::GBuffModifierApplier(GBuff* pOwner)
: m_pOwner(pOwner)
{
}

GBuffModifierApplier::~GBuffModifierApplier(void)
{
	RestoreEffect();
}

bool GBuffModifierApplier::OnEvent(TALENT_CONDITION nTiming)
{
	switch (nTiming )
	{
	case TC_BUFF_DUPLICATED:
		OnDuplicated();
		break;
	case TC_BUFF_STACKED:
		OnStacked();
		break;
	}

	return ApplyEffect();
}

void GBuffModifierApplier::OnDuplicated()
{
	RestoreEffect();
}

void GBuffModifierApplier::OnStacked()
{
	RestoreEffect();
}

bool GBuffModifierApplier::ApplyEffect()
{
	if (!m_pOwner->GetOwner()->IsActor())
		return false;

	GBuffInfo* pBuffInfo = m_pOwner->GetInfo();
	GEntityActor* pOwnerActor = ToEntityActor(m_pOwner->GetOwner());

	GBuffRequirement buff_requirement;
	if (!buff_requirement.ProcessRequirement(pOwnerActor, pBuffInfo->m_RequireBuff))
		return false; // 버프 필요조건 불충족

	GModEffector				m_ModEffector;
	for (int i=0; i<m_pOwner->GetStackCount(); ++i)
	{
		// 되받아치기 속성
		pOwnerActor->AddRiposte(pBuffInfo->m_Riposte);
		m_InfoRestore.vecRiposte.push_back(pBuffInfo->m_Riposte);

		// 모디파이어
		m_ModEffector.AddActorModifier(pOwnerActor->GetChrStatus()->ActorModifier, pBuffInfo->m_ActorModifier);
		m_ModEffector.AddActorModifier(m_InfoRestore.ActorModifier, pBuffInfo->m_ActorModifier);

		if (pOwnerActor->IsPlayer())
		{
			GEntityPlayer* pPlayer = ToEntityPlayer(pOwnerActor);
			m_ModEffector.AddPlayerModifier(pPlayer->GetPlayerModifier(), pBuffInfo->m_PlayerModifier);
			m_ModEffector.AddPlayerModifier(m_InfoRestore.PlayerModifier, pBuffInfo->m_PlayerModifier);
		}

		m_ModEffector.RefreshStatus(pOwnerActor);
	}
		
	return false;
}

bool GBuffModifierApplier::RestoreEffect()
{
	if (!m_pOwner->GetOwner()->IsActor())	return false;

	GEntityActor* pOwnerActor = ToEntityActor(m_pOwner->GetOwner());	
		
	// 되받아치기 속성
	for each (const CSRiposte& each in m_InfoRestore.vecRiposte)
		pOwnerActor->EraseRiposte(each);
	
	// 모디파이어
	GModEffector m_ModEffector;
	m_ModEffector.EraseActorModifier(pOwnerActor->GetChrStatus()->ActorModifier, m_InfoRestore.ActorModifier);

	if (pOwnerActor->IsPlayer())
	{
		GEntityPlayer* pPlayer = ToEntityPlayer(pOwnerActor);
		m_ModEffector.ErasePlayerModifier(pPlayer->GetPlayerModifier(), m_InfoRestore.PlayerModifier);
	}

	m_ModEffector.RefreshStatus(pOwnerActor);

	ClearResotreInfo();

	return false;
}

void GBuffModifierApplier::ClearResotreInfo()
{
	m_InfoRestore.ActorModifier.Clear();
	m_InfoRestore.PlayerModifier.Clear();
	m_InfoRestore.vecRiposte.clear();
}
