#include "stdafx.h"
#include "ZCmdHandler_Field.h"
#include "ZCommandTable.h"
#include "ZCommandCenter.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZGlobal.h"
#include "ZFieldCommandRouter.h"
#include "ZFieldLogic.h"

ZCmdHandler_Field::ZCmdHandler_Field(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL,			OnRequestFieldInfoAll);
	SetCmdHandler(MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL,	OnRequestFieldGroupInfoAll);

	SetCmdHandler(MMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP,		OnRequestDelDynamicFieldGroup);

	SetCmdHandler(MMC_GATE_ASK_SHARED_FIELD,						OnGateAskSharedField);
	SetCmdHandler(MMC_GATE_ASK_DYNAMIC_FIELD,						OnGateAskDynamicField);

	SetCmdHandler(MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ,				OnGatePrepareChangeChannelReq);
	SetCmdHandler(MMC_GATE_CANCEL_CHANGE_CHANNEL,					OnGateCancelChangeChannel);
	SetCmdHandler(MMC_GATE_ASK_CHANGE_CHANNEL,						OnGateAskChangeChannel);
}

MCommandResult ZCmdHandler_Field::OnRequestFieldInfoAll(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidClient = pCommand->GetSenderUID();

	SCommObject* pClientObject = gmgr.pGameServerObjectManager->GetClient(uidClient);
	if (pClientObject == NULL) return CR_TRUE;

	ZFieldCommandRouter::SendAllFieldInfo(uidClient);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Field::OnRequestFieldGroupInfoAll(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidClient = pCommand->GetSenderUID();

	SCommObject* pClientObject = gmgr.pGameServerObjectManager->GetClient(uidClient);
	if (pClientObject == NULL) return CR_TRUE;

	ZFieldCommandRouter::SendAllFieldGroupInfo(uidClient);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Field::OnRequestDelDynamicFieldGroup(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestServer = pCommand->GetSenderUID();

	ZGameServerObject* pRequestServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidRequestServer);
	if (pRequestServerObject == NULL)	return CR_TRUE;

	MUID uidFieldGroup;

	if (pCommand->GetParameter(&uidFieldGroup,		0, MPT_UID)==false) return CR_ERROR;


	ZFieldLogic fieldLogic;
	if (!fieldLogic.OnRequestDelDynamicFieldGroup(uidRequestServer, uidFieldGroup))
		return CR_TRUE;

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Field::OnGateAskSharedField(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequestPlayer;
	TD_GATE_INFO* pGateInfo;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pGateInfo, 1))					return CR_ERROR;


	// 요청 서버 확인
	MUID uidRequestGameServer = pCommand->GetSenderUID();

	ZGameServerObject* pServer = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (pServer == NULL)	return CR_TRUE;

	ZPlayer* pPlayer = pServer->GetPlayerMap().FindByUID(uidRequestPlayer);
	if (pPlayer == NULL)	return CR_TRUE;


	// 필드 이동
	ZFieldLogic fieldLogic;
	fieldLogic.OnGateAskSharedField(pServer, pPlayer, pGateInfo);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Field::OnGateAskDynamicField(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	MUID uidRequestPlayer;
	TD_GATE_FIELD_GROUP_INFO* pGateFieldGroupInfo;

	if (!pCommand->GetParameter(&uidRequestPlayer, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pGateFieldGroupInfo, 1))		return CR_ERROR;


	// 요청 서버 확인
	MUID uidRequestGameServer = pCommand->GetSenderUID();

	ZGameServerObject* pServer = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (pServer == NULL)	return CR_TRUE;

	ZPlayer* pPlayer = pServer->GetPlayerMap().FindByUID(uidRequestPlayer);
	if (pPlayer == NULL)	return CR_TRUE;


	// 필드 이동
	ZFieldLogic fieldLogic;
	fieldLogic.OnGateAskDynamicField(pServer, pPlayer, pGateFieldGroupInfo);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Field::OnGatePrepareChangeChannelReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;
	int nChannelID;

	if (!pCommand->GetParameter(&uidRequestPlayer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID,		1, MPT_INT))	return CR_ERROR;

	MUID uidRequestGameServer = pCommand->GetSenderUID();

	ZGameServerObject* pServer = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (pServer == NULL) return CR_TRUE;

	ZPlayer* pPlayer = pServer->GetPlayerMap().FindByUID(uidRequestPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 필드 이동
	ZFieldLogic fieldLogic;
	fieldLogic.OnGatePrepareChangeChannelReq(pServer, pPlayer, nChannelID);

	return CR_TRUE;
}
MCommandResult ZCmdHandler_Field::OnGateCancelChangeChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;

	if (!pCommand->GetParameter(&uidRequestPlayer,	0, MPT_UID))	return CR_ERROR;

	MUID uidRequestGameServer = pCommand->GetSenderUID();

	ZGameServerObject* pServer = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (pServer == NULL) return CR_TRUE;

	ZPlayer* pPlayer = pServer->GetPlayerMap().FindByUID(uidRequestPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 필드 이동
	ZFieldLogic fieldLogic;
	fieldLogic.OnGateCancelChangeChannel(pPlayer);

	return CR_TRUE;
}
MCommandResult ZCmdHandler_Field::OnGateAskChangeChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequestPlayer;
	TD_GATE_INFO* pGateInfo;

	if (!pCommand->GetParameter(&uidRequestPlayer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetSingleBlob(pGateInfo,			1))				return CR_ERROR;

	MUID uidRequestGameServer = pCommand->GetSenderUID();

	ZGameServerObject* pServer = gmgr.pGameServerObjectManager->FindGameServer(uidRequestGameServer);
	if (pServer == NULL) return CR_TRUE;

	ZPlayer* pPlayer = pServer->GetPlayerMap().FindByUID(uidRequestPlayer);
	if (pPlayer == NULL)	return CR_TRUE;

	// 필드 이동
	ZFieldLogic fieldLogic;
	fieldLogic.OnGateAskChangeChannel(pServer, pPlayer, pGateInfo);

	return CR_TRUE;
}
