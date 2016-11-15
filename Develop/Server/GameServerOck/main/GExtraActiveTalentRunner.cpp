#include "stdafx.h"
#include "GExtraActiveTalentRunner.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GModuleBuff.h"
#include "GGlobal.h"
#include "GFieldInfoMgr.h"
#include "GPlayerField.h"
#include "GHateTable.h"
#include "GCombatCalculator.h"
#include "GTalentHitRouter.h"
#include "GTalentEffector.h"
#include "GPlayerFieldLazyGateway.h"
#include "GBuffEntity.h"
#include "GEntityNPC.h"
#include "GRebirthSystem.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GPlayerSpawnedNPC.h"

void GExtraActiveTalentRunner::Apply(GEntityActor* pUser, vec3 vPos, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	PFI_BLOCK_THISFUNC(9024);

	VALID(pUser);
	VALID(pTalentInfo);

	switch (pTalentInfo->m_nExtraActive)
	{
	case TEAT_PORTAL:				{ onPortal(pUser, pTalentInfo->m_nExtraActiveParam1, pTalentInfo->m_nExtraActiveParam2); } break;
	case TEAT_PORTAL_SOULBINDING:	{ onPortal_SoulBinding(pUser); } break;
	case TEAT_TOGGLE_PRECISION:		{ onTogglePrecision(pUser); } break;
	case TEAT_TOGGLE_SNEAK:			{ onToggleSneak(pUser, pTalentInfo->m_nExtraActiveParam1); } break;
	case TEAT_ADVENT:				{ onApplyAdvent(pUser); } break;	
	case TEAT_SPAWN:				{ onSpawn(pUser, pTalentInfo->m_nExtraActiveParam1, pTalentInfo->m_nExtraActiveParam2); }	break;
	case TEAT_TAUNT:				{ onTaunt(pUser, pTalentInfo, pVictim); } break;
	case TEAT_BUFF_ENTITY:			{ onBuffEntity(pUser, vPos, pTalentInfo->m_nExtraActiveParam1, pTalentInfo->m_nExtraActiveParam2); } break;
	case TEAT_DISPEL:				{ onDispel(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_CURE_POISON:			{ onCurePoison(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_CURE_DISEASE:			{ onCureDisease(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_CURE_CURSE:			{ onCureCurse(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_DISPEL_MESMERIZE:		{ onDispelMesmerize(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_DISPEL_DEATHPENALTY:	{ onDispelDeathPenalty(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_DISPEL_BUFF:			{ onDispelBuff(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_DISPEL_FOCUS:			{ onDispelFocus(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_DISPEL_BPART:			{ onDispelBPart(pUser, pTalentInfo, pVictim); }	break;
	case TEAT_REBIRTH:				{ onRebirth(pUser, pVictim, pTalentInfo->m_nExtraActiveParam1); }	break;	
	}
}

void GExtraActiveTalentRunner::onTogglePrecision( GEntityActor* pTargetActor )
{
	if (pTargetActor->IsDead())
		return;

	pTargetActor->OnTogglePrecision();
}

void GExtraActiveTalentRunner::onToggleSneak( GEntityActor* pTargetActor, int nSneakBuffType )
{
	if (pTargetActor->IsDead())
		return;

	pTargetActor->OnToggleSneak(nSneakBuffType);
}

void GExtraActiveTalentRunner::onApplyAdvent( GEntityActor* pTargetActor )
{
	if (pTargetActor->IsDead())
		return;

	pTargetActor->OnApplyAdvent();
}

void GExtraActiveTalentRunner::onSpawn( GEntityActor* pUser, int nNPCID, int nDespawnTime )
{
	VALID(pUser);
	if (!pUser->IsPlayer()) return;

	GEntityPlayer* pPlayer = ToEntityPlayer(pUser);
	pPlayer->GetSpawnedNPC().Spawn(nNPCID, static_cast<float>(nDespawnTime));
}

void GExtraActiveTalentRunner::onRebirth(GEntityActor* pUser, GEntityActor* pTarget, int nRecoverRate)
{
	if (!pTarget->IsPlayer()) return;
	if (!pTarget->IsDead()) return;

	float fRecoverRate = GMath::NumberToFloatPercent(nRecoverRate);
	gsys.pRebirthSystem->RebirthToPlace(pUser, ToEntityPlayer(pTarget), fRecoverRate);
}

void GExtraActiveTalentRunner::onPortal(GEntityActor* pTarget, int nFieldID, int nMarkerID)
{
	VALID(pTarget);
	if (pTarget->IsDead())	return;
	if (false == pTarget->IsPlayer())	return;
	GEntityPlayer* pPlayer = ToEntityPlayer(pTarget);

	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	VALID(pFieldInfo);

	const MARKER_INFO* pMarker = pFieldInfo->FindMarker(nMarkerID);
	if (NULL == pMarker) return;

	pPlayer->GetPlayerField().GetLazyGateway().GateToMarker(pMarker);
}

void GExtraActiveTalentRunner::onPortal_SoulBinding(GEntityActor* pTarget)
{
	VALID(pTarget);
	if (pTarget->IsDead())	return;
	if (false == pTarget->IsPlayer())	return;
	GEntityPlayer* pPlayer = ToEntityPlayer(pTarget);

	GSoulBindingInfo* pSoulBindingInfo = gmgr.pSoulBindingInfoMgr->Get(pPlayer->GetPlayerInfo()->nSoulBindingID);
	VALID(pSoulBindingInfo);
	VALID(pSoulBindingInfo->m_pMarkerInfo);

	pPlayer->GetPlayerField().GetLazyGateway().GateToMarker(pSoulBindingInfo->m_pMarkerInfo);
}

void GExtraActiveTalentRunner::onDispel(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->Dispel(nLimitPower);
}

void GExtraActiveTalentRunner::onCurePoison(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->CurePoison(nLimitPower);
}

void GExtraActiveTalentRunner::onCureDisease(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->CureDisease(nLimitPower);
}

void GExtraActiveTalentRunner::onCureCurse(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->CureCurse(nLimitPower);
}

void GExtraActiveTalentRunner::onDispelMesmerize(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->DispelMesmerize(nLimitPower);
}

void GExtraActiveTalentRunner::onDispelDeathPenalty( GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim )
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->DispelDeathPenalty(nLimitPower);
}


void GExtraActiveTalentRunner::onDispelBuff(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	int nLimitPower = pTalentInfo->m_nExtraActiveParam1;
	pmBuff->DispelBuff(nLimitPower);
}

void GExtraActiveTalentRunner::onDispelFocus(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	pmBuff->DispelFocus();
}

void GExtraActiveTalentRunner::onDispelBPart(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim)
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);

	if (pVictim->IsDead())
		return;

	GModuleBuff* pmBuff = pVictim->GetModuleBuff();
	VALID(pmBuff);

	pmBuff->DispelBPart();
}


void GExtraActiveTalentRunner::onTaunt( GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim )
{
	VALID(pUser);
	VALID(pTalentInfo);
	VALID(pVictim);
	VALID(pTalentInfo->m_nExtraActiveParam1 > 0);

	if (pVictim->IsDead())
		return;

	if (!pVictim->IsNPC())
		return;

	// 완전저항 체크
	float fResistRatePercent = 100.0f;
	if (gsys.pCombatCalculator->CheckResist(fResistRatePercent, pUser, pVictim, pTalentInfo->m_Resist))
	{
		if (pTalentInfo->m_Resist.m_nMethod == TRM_BINARY)	
		{
			GTalentHitRouter talentHitRouter;
			talentHitRouter.Route_Resist(pUser, pVictim, pTalentInfo, 0, 0);

			return; // 완전저항 성공
		}
	}
	
	// 목표 고정
	GEntityNPC* pVictimNPC = ToEntityNPC(pVictim);
	pVictimNPC->GetHateTable().Lock(pUser->GetUID(), (float)pTalentInfo->m_nExtraActiveParam1);
}

void GExtraActiveTalentRunner::onBuffEntity(GEntityActor* pUser, vec3 vPos, int nIncludeBuffID, int nLimitQty)
{
	VALID(pUser);
	vec3 vOut;
	MMatrix m;
	m.SetLocalMatrix(pUser->GetPos(), -pUser->GetDir(), vec3::AXISZ);
	m.TransformVect(vPos, vOut);

	GBuffEntity::Spawn(pUser, vOut, nIncludeBuffID, nLimitQty);
}