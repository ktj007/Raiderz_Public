#include "StdAfx.h"
#include "GRebirthSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerBattleArena.h"
#include "GPlayerFieldDynamic.h"
#include "CCommandResultTable.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GGlobal.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBManager.h"
#include "GBattleArenaMgr.h"
#include "GFieldInfoMgr.h"
#include "GConst.h"
#include "GPlayerPVPArea.h"
#include "GModuleBuff.h"
#include "CSDef_QuestPVP.h"
#include "CSFactionInfo.h"
#include "GPlayerFactions.h"
#include "GPlayerSystem.h"
#include "GBuffInfo.h"

bool GRebirthSystem::RebirthToPlace(GEntityActor* pUser, GEntityPlayer* pTarget, float fRecoverRate)
{
	VALID_RET(pUser, false);
	VALID_RET(pTarget, false);

	return Rebirth(pUser, pTarget, pTarget->GetFieldID(), pTarget->GetPos(), pTarget->GetDir(), fRecoverRate);
}

bool GRebirthSystem::RebirthToPlaceInBattleArena(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	// 투기장이 아닌 경우 무시
	if (false == pPlayer->GetPlayerBattleArena().IsEntered())	return false;

	// 스팟위치가 따로 없으면 현재 위치에서 부활
	vec3 vSpotPos = gmgr.pBattleArenaMgr->GetSpotPos(pPlayer);
	if (vec3::ZERO == vSpotPos)
	{
		vSpotPos = pPlayer->GetPos();
	}

	return Rebirth(pPlayer, pPlayer, pPlayer->GetFieldID(), vSpotPos, pPlayer->GetDir(), GConst::REBIRTH_BATTLEARENA_RECOVER);
}

bool GRebirthSystem::AutoRebirthInBattleArena(GEntityPlayer* pPlayer, vec3 vPos, vec3 vDir)
{
	VALID_RET(pPlayer, false);

	return Rebirth(pPlayer, pPlayer, pPlayer->GetFieldID(), vPos, vDir, GConst::REBIRTH_BATTLEARENA_RECOVER);
}

bool GRebirthSystem::RebirthToSoulBinding(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	// 투기장에 들어와있을때는 부활 명령을 무시
	if (pPlayer->GetPlayerBattleArena().IsEntered()) return false;

	const MARKER_INFO* pMarkerInfo = gsys.pPlayerSystem->GetSoulBindingMarker(pPlayer->GetSoulBindingID());
	if (NULL == pMarkerInfo)
	{
		pMarkerInfo = GetNearSoulBindingMarker(pPlayer);
		if (NULL == pMarkerInfo) return false;
	}	

	return Rebirth(pPlayer, pPlayer, pMarkerInfo->nFieldID, pMarkerInfo->vPoint, pMarkerInfo->vDir, GConst::REBIRTH_SOULBINDING_RECOVER, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
}

bool GRebirthSystem::RebirthToNearSoulBinding(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);
	// 다이나믹 필드 일때는 무시
	if (true == pPlayer->IsStayDynamicField()) return false;

	const MARKER_INFO* pMarkerInfo = GetNearSoulBindingMarker(pPlayer);
	if (NULL == pMarkerInfo) return false;

	return Rebirth(pPlayer, pPlayer, pMarkerInfo->nFieldID, pMarkerInfo->vPoint, pMarkerInfo->vDir, GConst::REBIRTH_SOULBINDING_RECOVER, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
}

bool GRebirthSystem::RebirthToCheckPoint(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	// 다이나믹 필드가 아닐때는 무시
	if (false == pPlayer->IsStayDynamicField()) return false;

	MARKER_INFO makrerInfo = pPlayer->GetPlayerField().GetFieldDynamic().GetCheckpoint();

	// 안전지대에 등록이 되어있지 않으면 소울바인딩 위치에서 부활
	if (true == makrerInfo.IsInvalid())
	{
		return RebirthToSoulBinding(pPlayer);
	}

	return Rebirth(pPlayer, pPlayer, makrerInfo.nFieldID, makrerInfo.vPoint, makrerInfo.vDir, GConst::REBIRTH_CHECKPOINT_RECOVER, GConst::REBIRTH_CHECKPOINT_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
}

bool GRebirthSystem::RebirthToQPVP( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	if (false == pPlayer->GetPlayerPVPArea().IsLocatedPvPArea()) return false;
	const GFieldInfo* pFieldInfo = pPlayer->GetFieldInfo();
	VALID_RET(pFieldInfo, false);

	int nMarkerID = 0;
	switch (pPlayer->GetPlayerPVPArea().GetTeam())
	{
	case QT_ONE :	nMarkerID = pFieldInfo->m_nQPVPRebirthMarkerID1; break;
	case QT_TWO :	nMarkerID = pFieldInfo->m_nQPVPRebirthMarkerID2; break;
	default:		nMarkerID = pFieldInfo->m_nQPVPRebirthMarkerID1; break;	// 팀이없더라도 어디로든 부활을 시켜준다.
	}
	
	const MARKER_INFO* pMarkerInfo = pFieldInfo->FindMarker(nMarkerID);
	VALID_RET(pMarkerInfo, false);

	if (false == Rebirth(pPlayer, pPlayer, pMarkerInfo->nFieldID, pMarkerInfo->vPoint, pMarkerInfo->vDir, GConst::REBIRTH_QUESTPVP_RECOVER, GConst::REBIRTH_QUESTPVP_BUFF, GConst::REBIRTH_QUESTPVP_DEBUFF)) return false;

	return true;
}


bool GRebirthSystem::RebirthForGM(GEntityPlayer* pPlayer)
{
	if (pPlayer->GetPlayerBattleArena().IsEntered())
	{
		return RebirthToPlaceInBattleArena(pPlayer);
	}
	else
	{
		return RebirthToPlace(pPlayer, pPlayer, 1.0f);
	}
}


bool GRebirthSystem::Rebirth(GEntityActor* pUser, GEntityPlayer* pTarget, int nFieldID, vec3 vPos, vec3 vDir, float fRecoverRate, int nBuffID1, int nBuffID2, int nBuffTargetLevel)
{
	VALID_RET(pUser, false);
	VALID_RET(pTarget, false);

	if (false == pTarget->IsDead()) return false;

	pTarget->GetPlayerField().Gate(nFieldID, vPos, vDir, false);

	if (pUser == pTarget)
	{
		pTarget->doRebirthBySelf();
	}                                    
	else
	{
		pTarget->doRebirthByOther();
	}	

	ApplyRebirthRecoverRate(pTarget, fRecoverRate);
	ApplyRebirthBuff(pTarget, nBuffID1, nBuffID2, nBuffTargetLevel);
	Route(pTarget, nFieldID, vPos, vDir);

	return true;
}

void GRebirthSystem::ApplyRebirthRecoverRate(GEntityPlayer* pPlayer, float fRecoverRate)
{
	VALID(pPlayer);

	pPlayer->SetHP(static_cast<int>(pPlayer->GetMaxHP() * fRecoverRate));
	pPlayer->SetEN(static_cast<int>(pPlayer->GetMaxEN() * fRecoverRate));
	pPlayer->SetSTA(static_cast<int>(pPlayer->GetMaxSTA() * fRecoverRate));
}

void GRebirthSystem::ApplyRebirthBuff(GEntityPlayer* pPlayer, int nBuffID1, int nBuffID2, int nBuffTargetLevel)
{
	VALID(pPlayer);
	if (nBuffTargetLevel >= pPlayer->GetLevel())
		return;

	GBuffInfo* pBuffInfo1 = gmgr.pBuffInfoMgr->Get(nBuffID1);
	if (NULL != pBuffInfo1)
	{
		pPlayer->GetModuleBuff()->GainBuffForced(pBuffInfo1, pBuffInfo1->m_fDuration, 0.0f);
	}
	GBuffInfo* pBuffInfo2 = gmgr.pBuffInfoMgr->Get(nBuffID2);
	if (NULL != pBuffInfo2)
	{
		pPlayer->GetModuleBuff()->GainBuffForced(pBuffInfo2, pBuffInfo2->m_fDuration, 0.0f);
	}
}

void GRebirthSystem::Route(GEntityPlayer* pPlayer, int nFieldID, vec3 vPos, vec3 vDir)
{
	VALID(pPlayer);

	TD_UPDATE_SIMPLE_STATUS td_simple_status;
	td_simple_status.nHP = (uint16)pPlayer->GetHP();
	td_simple_status.nEN = (uint16)pPlayer->GetEN();
	td_simple_status.nSTA = (uint16)pPlayer->GetSTA();

	MCommand* pNewMyCmd = MakeNewCommand(MC_CHAR_REBIRTH_MYPLAYER, 
		4,
		NEW_INT(nFieldID),
		NEW_VEC(vPos),
		NEW_SVEC2(vDir),
		NEW_SINGLE_BLOB(&td_simple_status, sizeof(TD_UPDATE_SIMPLE_STATUS)));
	pPlayer->RouteToMe(pNewMyCmd);	

	MCommand* pNewCommand = MakeNewCommand(MC_CHAR_REBIRTH_NETPLAYER, 1, NEW_UID(pPlayer->GetUID()));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);	
}

const MARKER_INFO* GRebirthSystem::GetNearSoulBindingMarker(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, NULL);

	vector<GSoulBindingInfo*> vecSoulBindingInfo = gmgr.pSoulBindingInfoMgr->CollectSoulBindingInfo(pPlayer->GetFieldID());

	const MARKER_INFO* pNearSoulBindingMarker = NULL;
	float fNearSoulBindingPosDiff = FLT_MAX;
	for each (GSoulBindingInfo* pSoulBindingInfo in vecSoulBindingInfo)
	{
		if (NULL == pSoulBindingInfo) continue;
		const MARKER_INFO* pMarkerInfo = pSoulBindingInfo->m_pMarkerInfo;
		if (NULL == pMarkerInfo) continue;
		if (0 != pSoulBindingInfo->m_nFactionID &&
			FRT_BAD >= pPlayer->GetPlayerFactions().GetRelation(pSoulBindingInfo->m_nFactionID)) continue;
		float fDiff = pPlayer->GetPos().DistanceTo(pMarkerInfo->vPoint);
		if (fDiff >= fNearSoulBindingPosDiff) continue;

		pNearSoulBindingMarker = pMarkerInfo;
		fNearSoulBindingPosDiff = fDiff;
	}

	return pNearSoulBindingMarker;
}


