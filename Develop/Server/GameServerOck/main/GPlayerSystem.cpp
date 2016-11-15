#include "StdAfx.h"
#include "GPlayerSystem.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GConst.h"
#include "CCommandResultTable.h"
#include "GDBManager.h"
#include "GPlayerTalent.h"
#include "GPlayerDoing.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GSoulBindingInfoMgr.h"
#include "GSoulBindingInfo.h"
#include "GConfig.h"
#include "GDBTaskCharData.h"
#include "GPlayerField.h"
#include "GPlayerFieldShared.h"
#include "GPlayerSit.h"


bool GPlayerSystem::AddTP(GEntityPlayer* pPlayer, int nTP)
{
	if (NULL == pPlayer) return false;
	if (0 >= nTP) return false;

	gsys.pDBManager->GM_CharUpdateTP(pPlayer->GetAID()
		, pPlayer->GetCID()
		, pPlayer->GetTalent().GetTP() + nTP
		, pPlayer->GetTalent().GetTotalTP() + nTP
		, pPlayer->GetPlayerInfo()->nPlayTimeSec);

	pPlayer->GetTalent().AddTP(nTP);

	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_UPDATE_TP, 
		1,
		NEW_INT(pPlayer->GetTalent().GetTP()));

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GPlayerSystem::WriteDieLogToDB( GEntityPlayer* pPlayer, int nCode, int KillerNpcID, const MUID& uidKillerPlayer, int nKillerCode )
{
	if (NULL == pPlayer)
		return false;

	GDBT_CHAR_KILL die(
		pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nCID
		, nCode
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetLevel()
		, pPlayer->GetPlayerInfo()->nXP
		, pPlayer->GetFieldID()
		, pPlayer->GetPos());

	if (pPlayer->GetUID() != uidKillerPlayer)
	{
		GEntityPlayer* pKiller = gmgr.pPlayerObjectManager->GetEntity(uidKillerPlayer);
		if (NULL != pKiller)
		{
			GDBT_CHAR_KILL killer(
				pKiller->GetAID()
				, pKiller->GetUID()
				, pKiller->GetPlayerInfo()->nCID
				, nKillerCode
				, pKiller->GetPlayerInfo()->nPlayTimeSec
				, pKiller->GetPlayerInfo()->GetDeltaPlayTime()
				, pKiller->GetLevel()
				, pKiller->GetPlayerInfo()->nXP
				, pKiller->GetFieldID()
				, pKiller->GetPos());

			return gsys.pDBManager->CharDie(die, killer, KillerNpcID);
		}
	}

	return gsys.pDBManager->CharDie(die, GDBT_CHAR_KILL(), KillerNpcID);
}

bool GPlayerSystem::Sit(GEntityPlayer* pPlayer)
{
	if (true == pPlayer->GetSit().IsNowSit()) return CR_FALSE;

	pPlayer->GetSit().Sit();

	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_SIT, 
		1,
		NEW_USHORT(pPlayer->GetUIID()));

	pPlayer->RouteToThisCell(pNewCmd);

	return CR_TRUE;
}

bool GPlayerSystem::SitRise(GEntityPlayer* pPlayer)
{
	if (false == pPlayer->GetSit().IsNowSit()) return CR_FALSE;

	pPlayer->GetSit().Rise();	
	pPlayer->OnDoSomething();

	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_SIT_RISE, 
		1,
		NEW_USHORT(pPlayer->GetUIID()));

	pPlayer->RouteToThisCell(pNewCmd);

	return CR_TRUE;
}

bool GPlayerSystem::SaveSoulBinding( GEntityPlayer* pEntityPlayer, SBID nSoulBindingID )
{
	if (NULL == pEntityPlayer) return CR_FAIL;

	GSoulBindingInfo* pSoulBindingInfo = gmgr.pSoulBindingInfoMgr->Get(nSoulBindingID);
	if (NULL == pSoulBindingInfo) return pEntityPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_SOULBINDING_ID);
	const MARKER_INFO* pMarkerInfo = pSoulBindingInfo->m_pMarkerInfo;
	if (NULL == pMarkerInfo) return false;

	if (false == SaveSoulBinding_Apply(pEntityPlayer, nSoulBindingID)) return false;
	SaveSoulBinding_Route(pEntityPlayer, pMarkerInfo->nFieldID, pMarkerInfo->nID);

	return true;
}

bool GPlayerSystem::SaveSoulBinding_Apply( GEntityPlayer* pEntityPlayer, SBID nSoulBindingID)
{
	if (NULL == pEntityPlayer) return false;

	GDBT_CHAR_SAVE_SOUL_BINDING data(pEntityPlayer->GetAID()
		, pEntityPlayer->GetUID()
		, pEntityPlayer->GetCID()
		, nSoulBindingID
		, pEntityPlayer->GetPlayerInfo()->nPlayTimeSec);

	if (false == gsys.pDBManager->SoulBindingSave(data)) return false;
	pEntityPlayer->GetPlayerInfo()->nSoulBindingID = nSoulBindingID;

	return true;
}

void GPlayerSystem::SaveSoulBinding_Route(GEntityPlayer* pEntityPlayer, int nFieldID, int nMarkerID)
{
	if (NULL == pEntityPlayer) return;

	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_SAVE_SOULBINDING, 
		1, 
		NEW_INT(nFieldID));

	pEntityPlayer->RouteToMe(pNewCmd);
}

const MARKER_INFO* GPlayerSystem::GetSoulBindingMarker(SBID nSoulBindingID)
{
	GSoulBindingInfo* pSoulBindingInfo = gmgr.pSoulBindingInfoMgr->Get(nSoulBindingID);
	if (NULL == pSoulBindingInfo) return NULL;
	const MARKER_INFO* pMakrInfo = pSoulBindingInfo->m_pMarkerInfo;
	if (NULL == pMakrInfo) return NULL;

	return pMakrInfo;
}

SBID GPlayerSystem::GetSoulBindingFieldID(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, 0);

	GSoulBindingInfo* pSoulBindingInfo = gmgr.pSoulBindingInfoMgr->Get(pPlayer->GetSoulBindingID());
	if (NULL == pSoulBindingInfo) return 0;
	const MARKER_INFO* pMarkerInfo = pSoulBindingInfo->m_pMarkerInfo;
	VALID_RET(pMarkerInfo, 0);

	return pMarkerInfo->nFieldID;
}

bool GPlayerSystem::MoveToSelectChar(GPlayerObject* pPlayerObject, bool bForce/*=false*/)
{
	RVALID_RET(pPlayerObject != NULL, false);
	
	if (!bForce)
	{
		if (!MoveToSelectChar_Check(pPlayerObject)) return false;
	}

	MoveToSelectChar_Apply(pPlayerObject);

	return true;
}

bool GPlayerSystem::MoveToSelectChar_Check(GPlayerObject* pPlayerObject)
{
	RVALID_RET(pPlayerObject != NULL, false);
	GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();
	RVALID_RET(pEntityPlayer != NULL, false);

	if (!pPlayerObject->IsInWorld())					return pEntityPlayer->FailAndRouteSystemMsg(CR_MOVE_TO_SELECT_CHAR_INVALID_STATE);
	if (pEntityPlayer->GetDoing().IsBusy())				return pEntityPlayer->FailAndRouteAdviceMsg(CR_MOVE_TO_SELECT_CHAR_BUSY);
	if (pEntityPlayer->IsNowCombat())					return pEntityPlayer->FailAndRouteAdviceMsg(CR_MOVE_TO_SELECT_CHAR_DONT_DURING_COMBAT);

	GPlayerFieldShared& refPlayerFieldShared = pEntityPlayer->GetPlayerField().GetFieldShared();
	if (refPlayerFieldShared.IsPreparedChangeChannel())	return pEntityPlayer->FailAndRouteSystemMsg(CR_MOVE_TO_SELECT_CHAR_INVALID_STATE);

	return true;
}
void GPlayerSystem::MoveToSelectChar_Apply(GPlayerObject* pPlayerObject)
{
	RVALID(pPlayerObject != NULL);
	
	pPlayerObject->ReadyMoveLoginServer();
}
