#include "StdAfx.h"
#include "GNPCInteraction.h"
#include "GDef.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GJobMgr.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GGlobal.h"
#include "GInteractionSystem.h"
#include "GNPCTarget.h"
#include "GPlayerNPCIconSender.h"
#include "GNPCInteractor.h"
#include "GPlayerObjectManager.h"

GNPCInteraction::GNPCInteraction(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_bEnable(true)
, m_bEnabledBeforeCombat(true)
{

}

void GNPCInteraction::Begin(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	if (true == m_setInteractingPlayer.empty())
	{
		if (m_pOwner->GetModuleAI())
		{
			GJobMgr* pJobMgr = m_pOwner->GetModuleAI()->GetJobMgr();
			pJobMgr->Suspend();
		}
	}

	m_setInteractingPlayer.insert(pPlayer->GetUID());

	m_nLastInteractPlayerUID = pPlayer->GetUID();
	GetScriptNPC()->OnInteract(m_pOwner->GetDelegator(), pPlayer->GetDelegator());

	// 인터랙션된 상태를 타게팅하기
	m_pOwner->GetNPCTarget().SetTargetUID(pPlayer->GetUID());
}

void GNPCInteraction::End(GEntityPlayer* pPlayer)
{
	VALID(pPlayer);

	m_setInteractingPlayer.erase(pPlayer->GetUID());

	if (true == m_setInteractingPlayer.empty())
	{
		if (m_pOwner->GetModuleAI())
		{
			GJobMgr* pJobMgr = m_pOwner->GetModuleAI()->GetJobMgr();
			pJobMgr->Resume();
		}
	}	
}

set<GEntityPlayer*> GNPCInteraction::GetInteractingPlayer()
{	
	set<GEntityPlayer*> setInteractingPlayer;

	for each (const MUID& uidInteractigPlayer in m_setInteractingPlayer)
	{
		GEntityPlayer* pInteractingPlayer = m_pOwner->FindPlayer(uidInteractigPlayer);
		if (NULL == pInteractingPlayer) continue;

		setInteractingPlayer.insert(pInteractingPlayer);		
	}

	return setInteractingPlayer;
}

bool GNPCInteraction::IsInteractingPlayer( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	if (m_setInteractingPlayer.end() == m_setInteractingPlayer.find(pPlayer->GetUID())) return false;

	return true;
}

GEntityPlayer* GNPCInteraction::GetLastInteractPlayer()
{
	return m_pOwner->FindPlayer(m_nLastInteractPlayerUID);
}

void GNPCInteraction::Enable()
{
	m_bEnable = true;

	m_pOwner->SendNPCIcon();
}

void GNPCInteraction::Disable()
{
	m_bEnable = false;

	if (false == m_pOwner->IsNowCombat())
	{
		GVectorMUID vecPlayerUID;
		
		m_pOwner->GetNeighborSectorPlayer(vecPlayerUID.Vector());
		
		for each (const MUID& nPlayerUID in vecPlayerUID.Vector())
		{
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nPlayerUID);
			if (NULL == pPlayer) continue;

			pPlayer->GetNPCIconSender().SendByDisableNPCInteraction(m_pOwner);
		}
	}	

	gsys.pInteractionSystem->GetNPCInteractor().End(m_pOwner);
}

void GNPCInteraction::DisableByCombatEnter()
{
	if (true == IsEnable())
	{
		m_bEnabledBeforeCombat = true;
		Disable();		
	}
	else
	{
		m_bEnabledBeforeCombat = false;
	}
}

void GNPCInteraction::EnableByCombatExit()
{
	if (true == m_bEnabledBeforeCombat)
	{
		Enable();
	}
}