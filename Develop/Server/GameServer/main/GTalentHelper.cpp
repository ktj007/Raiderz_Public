#include "stdafx.h"
#include "GTalentHelper.h"
#include "GEntityPlayer.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GTalent.h"
#include "GTalentHitter.h"
#include "GModuleCombat.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"

GTalentInfo* GTalentHelper::FindTalentInfo( int nTalentID, GEntity* pEntity/*=NULL*/ )
{
	if (pEntity && pEntity->IsPlayer())
	{
		GEntityPlayer* pEntityPlayer = static_cast<GEntityPlayer*>(pEntity);
		GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

		int nMode = (int)pItemHolder->GetEquipment().GetWeaponType();
		return gmgr.pTalentInfoMgr->Find(nTalentID, nMode);
	}
	else
	{
		return gmgr.pTalentInfoMgr->Find(nTalentID);
	}

	return NULL;
}

void GTalentHelper::AddHitterForTargetAndNonProjTalent( GTalent* pTalent )
{
	VALID(pTalent);

	if (pTalent->GetInfo()->m_EffectInfo.m_nPoint == CSEffectInfo::POINT_TARGET && 
		!pTalent->GetInfo()->IsProjectileType())
	{
		GEntityActor* pTarget = pTalent->GetTargetActor();
		if (!pTarget)	return;

		GTalentHitter_Instant* pNewHitter = GTalentHitter::NewTalentHitter< GTalentHitter_Instant >();
		pNewHitter->Setup(pTalent->GetOwner()->GetUID(), pTalent->GetInfo(), pTarget, 0, pTalent->GetTarget().nCapsuleGroupIndex, pTalent->GetTarget().nCapsuleIndex);

		VALID(pTalent->GetOwner()->GetModuleCombat());
		pTalent->GetOwner()->GetModuleCombat()->AddTargetHitter(pNewHitter);
	}
}