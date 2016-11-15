#include "stdafx.h"
#include "GCmdHandler_Comm.h"
#include "MCommand.h"
#include "GCommandTable.h"
#include "GPlayerObject.h"
#include "GField.h"
#include "GItemManager.h"
#include "GFieldInfo.h"
#include "GConst.h"
#include "GFieldSystem.h"
#include "GSharedFieldMaster.h"
#include "GCommandCenter.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicField.h"
#include "GModuleCombat.h"
#include "GInteractionSystem.h"
#include "GCommand.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GFieldMgr.h"
#include "GEnvManager.h"
#include "GDialogSystem.h"
#include "GPlayerInteraction.h"
#include "GDBTaskLogin.h"
#include "GQuestSystem.h"
#include "GPlayerSystem.h"
#include "GFieldInfoMgr.h"
#include "GMasterServerFacade.h"
#include "GServer.h"
#include "GPlayerBattleArena.h"
#include "GServerPlayerRouter.h"
#include "GNPCInteractor.h"
#include "GEntityNPC.h"
#include "GMsgCommandFacade.h"
#include "GPlayerTargetInfoSender.h"
#include "GModuleBuff.h"
#include "GBuffInfo.h"
#include "GConfig.h"
#include "GCalculator.h"
#include "GPlayerFieldDynamic.h"
#include "PmRegionStat.h"
#include "GPMSSystem.h"
#include "GProxyFieldManager.h"
#include "GAsyncTaskFactory.h"
#include "CCommandResultTable.h"
#include "GProxySharedFieldMaster.h"
#include "GProxyField.h"
#include "GFieldEnteringMgr.h"
#include "GPlayerGameGuard.h"
#include "GGameGuard.h"
#include "GGMExpoHandler.h"


GCmdHandler_Comm::GCmdHandler_Comm(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_FIELD_PREPARE_CHANGE_CHANNEL_REQ,	OnRequestPrepareChangeChannel);
	SetCmdHandler(MC_FIELD_CANCEL_CHANGE_CHANNEL,		OnRequestCancelChangeChannel);
	SetCmdHandler(MC_FIELD_REQUEST_CHANGE_CHANNEL,		OnRequestChangeChannel);
	SetCmdHandler(MC_FIELD_REQUEST_CHANNEL_LIST,		OnRequestChannelList);

	SetCmdHandler(MC_TRIGGER_TOUCH_SENSOR_ENTER,		OnTriggerTouchSensorEnter);
	SetCmdHandler(MC_TRIGGER_TOUCH_SENSOR_LEAVE,		OnTriggerTouchSensorLeave);
	SetCmdHandler(MC_TRIGGER_SENSOR_DESTINATION_SEL,	OnTriggerSensorDestinationSelection);
	SetCmdHandler(MC_TRIGGER_SENSOR_INTERACTION_REQ,	OnTriggerSensorInteraction);
	SetCmdHandler(MC_TRIGGER_SENSOR_FOLLOW_PARTY_RES,	OnTriggerSensorFollowPartyRes);
	
	SetCmdHandler(MC_DYNAMIC_FIELD_ENTER_REQ,		OnRequestEnterDynamicField);
	SetCmdHandler(MC_DYNAMIC_FIELD_LEAVE_REQ,		OnRequestLeaveDynamicField);
	
	SetCmdHandler(MC_DIALOG_SELECT_REQ,				OnRequestDialogSelect);
	SetCmdHandler(MC_BUFF_LOST_REQ,					OnRequestBuffLost);	

	SetCmdHandler(MC_FIELD_WHO_IS_REQ,				OnRequestWhoIs);
	SetCmdHandler(MC_FIELD_WHO_IS_UIID_REQ,			OnRequestWhoIsUIID);
	SetCmdHandler(MC_FIELD_PLAYER_EXTRA_INFO_REQ,	OnRequestPlayerExtraInfo);

	// 다른 핸들러로 빼야함 - ingame
	SetCmdHandler(MC_FIELD_LOADING_COMPLETE,		OnFieldLoadingComplete);

	// 캐릭터 선택화면으로 이동
	SetCmdHandler(MC_COMM_START_MOVE_TO_LOGIN_SERVER_REQ,	OnStartMoveToLoginServerReq);

	// 게임가드
	SetCmdHandler(MC_GG_AUTH_RESPOND,				OnGameGuardAuthRes);
	SetCmdHandler(MC_GG_AUTH_ENABLE,				OnGameGuardEnable);

	// Network
	SetCmdHandler(MC_LOCAL_NET_CLEAR,				OnNetClear);
}




MCommandResult GCmdHandler_Comm::OnTriggerTouchSensorEnter(MCommand* pCommand, MCommandHandler* pHandler)
{
	unsigned long	nSensorID;
	vec3			vPos; 
	if (pCommand->GetParameter(&nSensorID,	0, MPT_UINT)==false) return CR_FALSE;
	if (pCommand->GetParameter(&vPos,		1, MPT_VEC)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();

	GEntityPlayer* pEntity = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pEntity == NULL) return CR_FALSE;

	if (pEntity->IsValidSensor(nSensorID, vPos))
	{
		pEntity->EnterSensor(nSensorID);
	}	

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnTriggerTouchSensorLeave(MCommand* pCommand, MCommandHandler* pHandler)
{
	unsigned long	nSensorID;
	vec3			vPos;
	if (pCommand->GetParameter(&nSensorID,	0, MPT_UINT)==false) return CR_FALSE;
	if (pCommand->GetParameter(&vPos,		1, MPT_VEC)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();

	GEntityPlayer* pEntity = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pEntity == NULL) return CR_FALSE;
	
	if (pEntity->IsValidSensor(nSensorID, vPos))
	{
		pEntity->LeaveSensor(nSensorID);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnTriggerSensorDestinationSelection(MCommand* pCommand, MCommandHandler* pHandler)
{
	int	nSelection;
	if (pCommand->GetParameter(&nSelection,	0, MPT_INT)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();

	GEntityPlayer* pEntity = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pEntity == NULL) return CR_FALSE;

	pEntity->SelectWarpTrigger(nSelection);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnTriggerSensorInteraction(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<uint32> vecSensorID;
	
	if (!pCommand->GetBlob(vecSensorID, 0))		return CR_FALSE;
	

	MUID uidPlayer = pCommand->GetSenderUID();
	GEntityPlayer* pEntity = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pEntity == NULL) return CR_FALSE;

	pEntity->SensorInteraction(vecSensorID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnTriggerSensorFollowPartyRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	// no parameter command

	// 플레이어 확인
	MUID uidPlayer = pCommand->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (!pPlayer) return CR_FALSE;

	// 파티가 있을 경우만 이동
	if (!pPlayer->HasParty())	return CR_TRUE;
	
	int nFieldGroupID = pPlayer->GetFollowFieldGroupID();
	if (!pPlayer->GetPlayerField().GateToTrial(nFieldGroupID))	return CR_FALSE;

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestEnterDynamicField(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (IsRunForTest())
	{
		int nFieldGroupID;
		if (pCommand->GetParameter(&nFieldGroupID,	0, MPT_INT)==false) return CR_FALSE;

		MUID uidPlayer = pCommand->GetSenderUID();
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
		if (!pPlayer) return CR_FALSE;

		if (!pPlayer->GetPlayerField().GateToTrial(nFieldGroupID)) return CR_FALSE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestLeaveDynamicField(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (!pPlayer) return CR_FALSE;

	if (pPlayer->IsNowCombat())
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_EXIT_FIELD_PROCEED_COMBAT);
		return CR_TRUE;
	}

	pPlayer->GetPlayerField().GetFieldDynamic().GateToEnterPos(pPlayer->GetFieldID());

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestChangeChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCommand->GetSenderUID());
	if (pEntityPlayer == NULL) return CR_FALSE;

	gsys.pFieldSystem->ChangeChannel(pEntityPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestPrepareChangeChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCommand->GetSenderUID());
	if (pEntityPlayer == NULL) return CR_FALSE;

	int nChannelID;
	if (pCommand->GetParameter(&nChannelID,	0, MPT_INT)==false) return CR_FALSE;

	gsys.pFieldSystem->PrepareChangeChannel(pEntityPlayer, nChannelID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestCancelChangeChannel(MCommand* pCommand, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCommand->GetSenderUID());
	if (pEntityPlayer == NULL) return CR_FALSE;

	gsys.pFieldSystem->CancelChangeChannel(pEntityPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestChannelList(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pEntityPlayer == NULL) return CR_FALSE;

	GField* pField = pEntityPlayer->GetField();
	if (pField == NULL) return CR_FALSE;


	vector<CHANNELID> vecChannelID;
	vector<CHANNEL_STATUS> vecChannelStatuses;
	if (GConfig::m_bStandAlone)
	{
		GSharedFieldMaster* pSharedFieldMaster = gmgr.pFieldMgr->GetSharedFieldMaster(pField->GetID());
		if (NULL == pSharedFieldMaster) return CR_FALSE;

		pSharedFieldMaster->GetChannels(vecChannelID);

		for each (CHANNELID each in vecChannelID)
		{
			GSharedField* pChannelField = pSharedFieldMaster->GetChannel(each);
			if (NULL == pChannelField) return CR_FALSE;
			vecChannelStatuses.push_back(GCalculator::CalcChannelStatus(pChannelField->GetPlayerQty(), pChannelField->GetCapacity()));
		}
	}
	else
	{
		// 현재 필드의 서버군 전체 채널목록을 구한다.
		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		_ASSERT(pProxyFieldManager != NULL);

		GProxySharedFieldMaster* pSharedFieldMaster = pProxyFieldManager->FindSharedFieldMaster(pField->GetID());
		if (NULL == pSharedFieldMaster) return CR_FALSE;

		pSharedFieldMaster->GetChannelIDList(vecChannelID);

		for each (CHANNELID each in vecChannelID)
		{
			GProxyField* pChannelField = pSharedFieldMaster->GetSharedField(each);
			if (NULL == pChannelField) return CR_FALSE;
			vecChannelStatuses.push_back(GCalculator::CalcChannelStatus(pChannelField->GetPlayerCount(), pChannelField->GetCapacity()));
		}
	}

	if (vecChannelID.empty())	return CR_FALSE;


	// 커맨드 전송
	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_CHANNEL_LIST, uidPlayer, 2, 
		NEW_BLOB(vecChannelID),
		NEW_BLOB(vecChannelStatuses));
	gsys.pCommandCenter->PostCommand(pNewCmd);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestWhoIs(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidObject;
	if (pCmd->GetParameter(&uidObject,		0, MPT_UID) == false) return CR_FALSE;

	GEntityActor* pTarObject = pPlayer->FindActor(uidObject);
	if (pTarObject == NULL) return CR_FALSE;

	if (pTarObject->IsNowInvisibility())
		return CR_TRUE;	// 안보이는 상태

	MCommand* pNewCmd = pTarObject->MakeCmd_Whois(pPlayer);
	pPlayer->RouteToMe(pNewCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestWhoIsUIID(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	UIID nObjectUIID;
	if (pCmd->GetParameter(&nObjectUIID,		0, MPT_USHORT) == false) return CR_FALSE;

	GEntityActor* pTarObject = NULL;

	GEntityPlayer* pTarEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(nObjectUIID);
	if (pTarEntityPlayer != NULL)
	{
		pTarObject = pTarEntityPlayer;
	}
	else
	{
		GEntityNPC* pTarEntityNPC = pPlayer->FindNPC(nObjectUIID);
		pTarObject = pTarEntityNPC;
	}

	if (pTarObject == NULL) return CR_FALSE;

	if (pTarObject->IsNowInvisibility())
		return CR_TRUE;	// 안보이는 상태

	MCommand* pNewCmd = pTarObject->MakeCmd_Whois(pPlayer);
	pPlayer->RouteToMe(pNewCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestPlayerExtraInfo(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidPlayer;
	if (pCmd->GetParameter(&uidPlayer,		0, MPT_UID) == false) return CR_FALSE;

	GEntityPlayer* pTarPlayer = pPlayer->FindPlayer(uidPlayer);
	if (pTarPlayer == NULL) return CR_FALSE;


	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestDialogSelect(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nSelectID;
	if (pCmd->GetParameter(&nSelectID,	0, MPT_INT) == false) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckITarget(pPlayer)) return CR_FALSE;

	if (false == gsys.pDialogSystem->Select(pPlayer, nSelectID))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnRequestBuffLost(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nBuffID;
	if (pCmd->GetParameter(&nBuffID,		0, MPT_INT) == false) return CR_FALSE;

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (NULL == pBuffInfo) return CR_FALSE;
	if (BUFT_DEBUFF == pBuffInfo->m_nType) return CR_FALSE;
	if (BUFT_QUEST == pBuffInfo->m_nType) return CR_FALSE;

	pPlayer->GetModuleBuff()->CancelBuff(nBuffID);

	return CR_TRUE;
}

//////////////////////////////////////////////////////////////////////////

MCommandResult GCmdHandler_Comm::OnFieldLoadingComplete(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	/// 아직 상태값이 POS_INWORLD 가 아닙니다. GPlayerObjectManager::GetEntityInWorld() 메소드를 사용하지 않습니다.
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return CR_FALSE;

	gmgr.pFieldMgr->GetFieldEnteringQueue()->Add(uidPlayer);
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnStartMoveToLoginServerReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(pCmd->GetSenderUID());
	if (NULL == pPlayerObject) return CR_FALSE;

	GGMExpoHandler expoHandler;
	if (expoHandler.IsExpoPublicBoothAccount(pPlayerObject->GetAccountInfo().szUserID))
	{
		expoHandler.ResetPublicBoothCharacter(pPlayerObject);
	}
	else
	{
		gsys.pPlayerSystem->MoveToSelectChar(pPlayerObject);
	}	

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnNetClear(MCommand* pCommand, MCommandHandler* pHandler)
{
	PFI_BLOCK_THISFUNC(5643);

	gsys.pServer->GetServerInfo().nDisconnCountAll++;

	//////////////////////////////////////////////////////////////////////////
	MUID uidPlayer;

	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_FALSE;

	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayerObject == NULL) return CR_FALSE;

	// 로그아웃 처리
	GAsyncTaskManager* pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();	
	GAsyncTaskFactory::NewLogoutTask(pAsyncTaskManager, uidPlayer);	// Mgr->Add(pNewTask);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnGameGuardAuthRes(MCommand* pCmd, MCommandHandler* pHandler)
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(pCmd->GetSenderUID());
	if (!pPlayer) 
		return CR_FALSE;

	MCommandParameter* pParam = pCmd->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;

	GG_AUTH_DATA* pggData = (GG_AUTH_DATA*)pParam->GetPointer();
	GPlayerGameGuard* pGameGuard = pPlayer->GetGameGuard();
	if (!pGameGuard)
		return CR_FALSE;

	pGameGuard->AuthAnswer(pggData);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Comm::OnGameGuardEnable(MCommand* pCmd, MCommandHandler* pHandler)
{
	bool bEnable = true;
	if (pCmd->GetParameter(&bEnable,		0, MPT_BOOL) == false) return CR_FALSE;


	// GM 이상만 사용할 수 있습니다.	
	MUID uidPlayer = pCmd->GetSenderUID();
	VALID_RET(uidPlayer.IsValid(), CR_FALSE);

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	VALID_RET(pEntityPlayer, CR_FALSE);
	
	if (!pEntityPlayer->IsGM()) return CR_FALSE;


	mlog("GameGuard %s.\n", bEnable?"Enabled":"Disabled");
	gsys.pGameGuard->Enable(bEnable);
	
	return CR_TRUE;
}
