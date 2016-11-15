#include "stdafx.h"
#include "ZGateCommandRouter.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "CTransData.h"
#include "ZServer.h"
#include "STransData_M2G.h"

void ZGateCommandRouter::RouteResponseReserveMoveGameServer(MUID uidTarget, MUID uidRequestPlayer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
{
	int nCount = 1;
	if (nResult != CR_SUCCESS || pLoginGameServerInfo == NULL)
	{
		nCount = 0;
	}

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER
						, uidTarget
						, 3
						, NEW_UID(uidRequestPlayer)
						, NEW_INT(nResult)
						, NEW_BLOB(pLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), nCount));
	
	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZGateCommandRouter::RouteRequestReadyEnterMovePlayer(MUID uidTarget, MUID uidConnectionKey, MUID uidRequestPlayer, MUID uidRequestServer, const MCommand* pCommand)
{
	_ASSERT(pCommand != NULL);

	MCommand* pCmd = gsys.pCommandCenter->NewCommand(MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER, uidTarget);

	// 해더 파라미터 설정
	pCmd->AddParameter(NEW_UID(uidConnectionKey));
	pCmd->AddParameter(NEW_UID(uidRequestServer));
	pCmd->AddParameter(NEW_UID(uidRequestPlayer));
	
	// Relay 파라미터 복사
	for (int i = 2; i < pCommand->GetParameterCount(); i++)
	{
		MCommandParameter* pParameter = pCommand->GetParameter(i);
		pCmd->AddParameter(pParameter->Clone());
	}

	// 전달
	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZGateCommandRouter::RouteGateMoveServer(MUID uidTarget, MUID uidRequestPlayer, int nMoveTargetServerID, const TD_GATE_INFO* pGateInfo)
{
	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_GATE_MOVE_SERVER,
							uidTarget,
							3,
							NEW_UID(uidRequestPlayer),
							NEW_INT(nMoveTargetServerID),
							NEW_SINGLE_BLOB(pGateInfo, sizeof(TD_GATE_INFO)));

	gsys.pCommandCenter->PostCommand(pCmd);	
}

void ZGateCommandRouter::RouteCancelReservedMovePlayer(MUID uidTarget, MUID uidConnectionKey)
{
	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_CANCEL_RESERVED_MOVE_PLAYER,
															uidTarget,
															1,
															NEW_UID(uidConnectionKey)
															);

	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZGateCommandRouter::RouteGateRes(MUID uidTarget, MUID uidRequestPlayer, RESPONSE_GATE_TYPE nResponseGateType, const TD_GATE_INFO* pGateInfo)
{
	int nCount = 1;
	if (pGateInfo == NULL)
	{
		nCount = 0;
	}

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_GATE_RES,
							uidTarget,
							3,
							NEW_UID(uidRequestPlayer),
							NEW_INT(nResponseGateType),
							NEW_BLOB(pGateInfo, sizeof(TD_GATE_INFO), nCount));

	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZGateCommandRouter::RouteGatePrepareChangeChannelRes(MUID uidTarget, MUID uidRequestPlayer, int nResult, int nChannelID)
{
	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_GATE_PREPARE_CHANGE_CHANNEL_RES,
		uidTarget,
		3,
		NEW_UID(uidRequestPlayer),
		NEW_INT(nResult),
		NEW_INT(nChannelID));

	gsys.pCommandCenter->PostCommand(pCmd);
}