#include "StdAfx.h"
#include "GLootBeginner.h"
#include "GGlobal.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "GInteractionSystem.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GPlayerDoing.h"
#include "GPlayerLoot.h"
#include "GConst.h"
#include "GNPCIEChecker.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GLootSystem.h"
#include "GLootModifyHandler.h"

bool GLootBeginner::KillNPC(GEntityPlayer* pPlayer, const MUID& uidNPC)
{
	VALID_RET(pPlayer, false);

	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	VALID_RET(pNPC, false);

	if (false == KillNPC_Check(pPlayer, pNPC)) return false;		
	KillNPC_Apply(pPlayer, pNPC);
	Route(pPlayer, uidNPC, .0f);

	return true;
}

bool GLootBeginner::KillNPC_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == pNPC->IsDead()) return false;
	if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_TARGET_DISTANCE);

	if (!pNPC->IsBPart())
	{
		if (false == pNPC->GetNPCLoot().GetDropList().IsViewableCID(pPlayer->GetCID())) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_EXIST_DROPITEM);
	}
	if (false == pPlayer->GetDoing().IsEndableBusy()) return false;

	return true;
}

void GLootBeginner::KillNPC_Apply( GEntityPlayer* pPlayer, GEntityNPC* pNPC )
{
	VALID(pPlayer);
	VALID(pNPC);

	pPlayer->GetDoing().EndDoing();
	pPlayer->GetPlayerLoot().BeginByKillNPC(pNPC->GetUID());
	pNPC->GetNPCLoot().Begin(pPlayer->GetCID());
}

bool GLootBeginner::InteractNPC(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nLootID, bool bImmortal)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == InteractNPC_Check(pPlayer, pNPC, bImmortal)) return false;

	if (false == bImmortal)
	{
		if (false == pNPC->GetNPCLoot().GetDropList().DropByInteract_Mortal(pPlayer->GetCID(), nLootID)) return false;
	}
	else
	{
		if (false == pNPC->GetNPCLoot().GetDropList().DropByInteract_Immortal(pPlayer->GetCID(), nLootID)) return false;
	}

	float fLootTime = pNPC->GetNPCInfo()->fInteractionLootTime;
	InteractNPC_Apply(pPlayer, pNPC, fLootTime);
	Route(pPlayer, pNPC->GetUID(), fLootTime);

	return true;
}

bool GLootBeginner::InteractNPC_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, bool bImmortal)
{	
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == pPlayer->GetDoing().IsEndableBusy()) return false;
	if (false == pNPC->GetNPCLoot().IsLootingPlayerEmpty()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_ALREADY_OTHER_INTERACT_LOOTING);

	return true;
}

void GLootBeginner::InteractNPC_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC, float fLootTime)
{
	VALID(pPlayer);
	VALID(pNPC);

	pPlayer->GetDoing().EndDoing();
	pPlayer->GetPlayerLoot().BeginByInteractNPC(pNPC->GetUID(), fLootTime);	
	pNPC->GetNPCLoot().Begin(pPlayer->GetCID());
}

bool GLootBeginner::GatherNPC(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nGatherTier, int nLootID)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	GATHER_TYPE eGatherType = pNPC->GetNPCInfo()->nGatherType;

	if (nGatherTier < 1 || GATHER_TIER_COUNT < nGatherTier) return false;
	if (eGatherType <= GATHER_NONE || MAX_GATHER_TYPE <= eGatherType) return false;
	if (false == GatherNPC_Check(pPlayer, pNPC, nGatherTier)) return false;

	float fGatherTime = GConst::GATHER_TIME[eGatherType][nGatherTier];

	// 보정치 있으면 적용
	CHR_STATUS* pChrStatus = pPlayer->GetChrStatus();
	if (pChrStatus->ActorModifier.fGatherTimeAmp[eGatherType].IsModified())
	{
		fGatherTime = pChrStatus->ActorModifier.fGatherTimeAmp[eGatherType].Calc(fGatherTime);		
	}

	fGatherTime = max(fGatherTime, 0.0f);	// 음수값 방어


	GatherNPC_Apply(pPlayer, pNPC, fGatherTime, nLootID);

	Route(pPlayer, pNPC->GetUID(), fGatherTime);

	return true;
}

bool GLootBeginner::GatherNPC_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nGatherTier)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_TARGET_DISTANCE);
	if (false == pPlayer->GetDoing().IsEndableBusy()) return false;
	if (false == pNPC->GetNPCLoot().IsLootingPlayerEmpty()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_ALREADY_OTHER_GATHERING);

	GNPCIEChecker interactionChecker;
	UNGATHERABLE_REASON nUR;
	if (false == interactionChecker.Gather(pPlayer, pNPC, nGatherTier, nUR))
	{
		switch (nUR)
		{
		case UR_TOOL			: return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_LOOT_GATHER_NEED_GATHERTOOL);
		case UR_TRAINING		: return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_LOOT_GATHER_NEED_TRAINING);
		case UR_HIGHERTRAINING	: return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_LOOT_GATHER_NEED_HIGHER_TRAINING);
		}
	}

	return true;
}

void GLootBeginner::GatherNPC_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC, float fLootTime, int nLootID)
{
	VALID(pPlayer);
	VALID(pNPC);

	pPlayer->GetDoing().EndDoing();
	pPlayer->GetPlayerLoot().BeginByGatherNPC(pNPC->GetUID(), fLootTime, nLootID);
	pNPC->GetNPCLoot().Begin(pPlayer->GetCID());
}

void GLootBeginner::Route(GEntityPlayer* pPlayer, const MUID& uidNPC, float fLootTime)
{
	VALID(pPlayer);	

	MCommand* pCmd = MakeNewCommand(MC_LOOT_BEGIN, 
		3,
		NEW_UID(pPlayer->GetUID()),
		NEW_UID(uidNPC),
		NEW_FLOAT(fLootTime));

	pPlayer->RouteToThisCell(pCmd);
}
