#include "stdafx.h"
#include "GCmdHandler_Master_Gate.h"
#include "CCommandTable_Master.h"
#include "STransData_M2G.h"
#include "GPlayerObject.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "CCommandResultTable.h"
#include "GFieldSystem.h"
#include "GPlayerFieldShared.h"

GCmdHandler_Master_Gate::GCmdHandler_Master_Gate(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_GATE_MOVE_SERVER,					OnGateMoveServer);
	SetCmdHandler(MMC_GATE_RES,							OnGateRes);
	SetCmdHandler(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,	OnGatePrepareChangeChannelRes);	
}

MCommandResult GCmdHandler_Master_Gate::OnGateMoveServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequestPlayer;
	int nTargetGameServerID;
	TD_GATE_INFO* pGateInfo;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nTargetGameServerID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pGateInfo, 2))					return CR_ERROR;

	// 요청 플레이어 확인
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidRequestPlayer);
	if (NULL == pPlayerObject) return CR_TRUE;

	GGateInfo gateInfo;
	gateInfo.Assign(*pGateInfo);
	pPlayerObject->ReadyMoveGameServer(nTargetGameServerID, gateInfo);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Gate::OnGateRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequestPlayer;
	int nResponseGateType;
	TD_GATE_INFO* pGateInfo;
	int nCount;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResponseGateType, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetBlob(pGateInfo, nCount, 2))				return CR_ERROR;
	if (nResponseGateType == RGT_EXECUTE && pGateInfo == NULL)		return CR_ERROR;


	// 요청 플레이어 확인
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidRequestPlayer);
	if (pPlayerObject == NULL) return CR_TRUE;


	// 재시도 형식 확인
	GGateInfo gateInfo;
	if (pGateInfo != NULL)
	{
		gateInfo.Assign(*pGateInfo);
	}

	pPlayerObject->Gate((RESPONSE_GATE_TYPE)nResponseGateType, &gateInfo);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Gate::OnGatePrepareChangeChannelRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;
	int nResult;
	int nChannelID;

	if (!pCommand->GetParameter(&uidRequestPlayer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,			1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID,		2, MPT_INT))	return CR_ERROR;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (NULL == pPlayer) return CR_TRUE;

	if (CR_SUCCESS != nResult)
	{
		gsys.pFieldSystem->CancelChangeChannel(pPlayer);

		pPlayer->RouteSystemMsg((CCommandResultTable)nResult);
		return CR_TRUE;
	}

	// 예약 시작 시간 기록
	pPlayer->GetPlayerField().GetFieldShared().PrepareChangeChannel(nChannelID);

	return CR_TRUE;
}
