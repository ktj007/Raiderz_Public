#include "stdafx.h"
#include "GEntityNPCUpdater.h"
#include "GEntityNPC.h"
#include "GHateTable.h"
#include "GNPCDecay.h"
#include "GNPCSensorChecker.h"
#include "GNPCStress.h"
#include "GNPCBParts.h"
#include "GNPCAssist.h"
#include "GNPCFirstStrike.h"
#include "GActorCooltimeChecker.h"
#include "GActorInvisibility.h"
#include "GConst.h"
#include "GModuleAI.h"
#include "GField.h"
#include "GGlobal.h"
#include "GTestSystem.h"
#include "GQPNPCPosSender.h"

AI_LOD_LEVEL GEntityNPCUpdater::GetAILodLevel( GEntityNPC* pOwner )
{
	if (pOwner->GetField() && 
		pOwner->GetField()->GetInfo() &&
		!pOwner->GetField()->GetInfo()->m_bAILODEnable) 
	{
		return AI_LOD_LEVEL_1;
	}

	if (!pOwner->GetNPCInfo()->m_bEnableAILod)
	{
		return AI_LOD_LEVEL_1;
	}

	return pOwner->GetModuleAI()->GetLodLevel();
}

void GEntityNPCUpdater::UpdatePre( GEntityNPC* pOwner, float fDelta )
{
	PFC_THISFUNC;

	AI_LOD_LEVEL nLODLevel = GetAILodLevel(pOwner);

	ResetTickVars(pOwner);
	Update_HateTable(pOwner, fDelta, nLODLevel);
	Update_NPCDecay(pOwner, fDelta);
	Update_ScriptTimer(pOwner, fDelta);
}

void GEntityNPCUpdater::UpdatePost( GEntityNPC* pOwner, float fDelta )
{
	PFC_THISFUNC;

	AI_LOD_LEVEL nLODLevel = GetAILodLevel(pOwner);

	Update_Invisibility(pOwner, fDelta, nLODLevel);
	Update_CooltimeChecker(pOwner, fDelta, nLODLevel);

	Update_NPCSensorChecker(pOwner, fDelta);
	Update_NPCStress(pOwner, fDelta);
	Update_NPCBParts(pOwner, fDelta);
	Update_NPCAssist(pOwner, fDelta, nLODLevel);
	Update_NPCFirstStrike(pOwner, fDelta, nLODLevel);
	Update_NPCMarkPos(pOwner, fDelta);
}

void GEntityNPCUpdater::Update_NPCAssist( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel )
{
	if (nLODLevel >= AI_LOD_LEVEL_3) return;

	if (pOwner->m_pNPCAssist) 
	{
		pOwner->m_pNPCAssist->Update(fDelta);
	}
}

void GEntityNPCUpdater::Update_NPCBParts( GEntityNPC* pOwner, float fDelta )
{
	if (pOwner->m_pNPCBParts) 
		pOwner->m_pNPCBParts->Update(fDelta);
}

void GEntityNPCUpdater::Update_NPCMarkPos( GEntityNPC* pOwner, float fDelta )
{
	if (pOwner->m_pQPNPCPosSender) 
		pOwner->m_pQPNPCPosSender->Update(fDelta);
}

void GEntityNPCUpdater::Update_NPCFirstStrike( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel )
{
	if (nLODLevel >= AI_LOD_LEVEL_4) return;

	pOwner->m_pNPCFirstStrike->Update(fDelta);
}

void GEntityNPCUpdater::Update_NPCStress( GEntityNPC* pOwner, float fDelta )
{
	pOwner->m_pNPCStress->Update(fDelta);
}

void GEntityNPCUpdater::Update_NPCSensorChecker( GEntityNPC* pOwner, float fDelta )
{
	if (pOwner->m_pNPCSensorChecker)
		pOwner->m_pNPCSensorChecker->Update(fDelta);
}

void GEntityNPCUpdater::Update_ScriptTimer( GEntityNPC* pOwner, float fDelta )
{
	pOwner->UpdateScriptTimer(fDelta);
}

void GEntityNPCUpdater::Update_NPCDecay( GEntityNPC* pOwner, float fDelta )
{
	pOwner->m_pNPCDecay->Update(fDelta);
}

void GEntityNPCUpdater::Update_HateTable( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel )
{
	if (nLODLevel >= AI_LOD_LEVEL_5) return;

	pOwner->m_pHateTable->Update(fDelta);
}

void GEntityNPCUpdater::ResetTickVars( GEntityNPC* pOwner )
{
	if (pOwner->GetTickVars().LazyAttack.uidTarget.IsValid())
	{
		pOwner->doAttack(pOwner->GetTickVars().LazyAttack.uidTarget, pOwner->GetTickVars().LazyAttack.nDamage);
	}
	pOwner->ResetTickVars();
}

void GEntityNPCUpdater::Update_CooltimeChecker( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel )
{
	if (pOwner->GetNPCInfo()->nNpcType == NPC_TYPE_OBJECT ||
		pOwner->GetNPCInfo()->nNpcType == NPC_TYPE_GATHER)
	{
		return;
	}

	if (nLODLevel >= AI_LOD_LEVEL_3) return;


	if (pOwner->m_pCoolTimeChecker)
		pOwner->m_pCoolTimeChecker->Update(fDelta);
}

void GEntityNPCUpdater::Update_Invisibility( GEntityNPC* pOwner, float fDelta, AI_LOD_LEVEL nLODLevel )
{
	if (pOwner->GetNPCInfo()->nNpcType == NPC_TYPE_OBJECT ||
		pOwner->GetNPCInfo()->nNpcType == NPC_TYPE_GATHER)
	{
		return;
	}

	if (nLODLevel >= AI_LOD_LEVEL_3) return;


	pOwner->m_pInvisibility->OnUpdate(fDelta);
}