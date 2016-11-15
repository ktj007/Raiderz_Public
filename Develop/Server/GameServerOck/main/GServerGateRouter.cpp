#include "stdafx.h"
#include "GServerGateRouter.h"
#include "GPlayerGameData.h"
#include "GCommandTable.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "GGateInfo.h"
#include "CTransData.h"
#include "GConfig.h"


GServerGateRouter::GServerGateRouter()
{
	// do nothing
}

GServerGateRouter::~GServerGateRouter()
{
	// do nothing
}

void GServerGateRouter::ResponseLoginGameServer(MUID uidRequestServer, MUID uidRequestID, int nResult, MUID uidConnectionKey)
{
	TD_LOGIN_GAME_SERVER_INFO loginGameServerInfoParam;

	wstring strIP = GConfig::m_strPublicIP;
	wcsncpy_s(loginGameServerInfoParam.strHostName, strIP.c_str(), _TRUNCATE);
	loginGameServerInfoParam.nPort = (uint32)GConfig::m_nPort;
	loginGameServerInfoParam.uidConnectionKey = uidConnectionKey;

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_COMM_RESPONSE_LOGIN_GAME_SERVER,
													4, 
													NEW_UID(uidRequestServer),
													NEW_UID(uidRequestID),
													NEW_INT(nResult),
													NEW_SINGLE_BLOB(&loginGameServerInfoParam, sizeof(loginGameServerInfoParam))
													);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::RequestReserveMoveGameServer(MUID uidRequestPlayer, int nMoveTargetServerID, const GPlayerGameData* pPlayerGameData)
{	
	_ASSERT(pPlayerGameData != NULL);

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER,
													17,
													NEW_INT(nMoveTargetServerID),
													NEW_UID(uidRequestPlayer),
													NEW_BLOB(pPlayerGameData->GetGateInfo(), sizeof(TD_GATE_INFO), 1),
													NEW_BLOB(pPlayerGameData->GetDynamicFieldEnterPosInfo(), sizeof(TD_DYNAMIC_FIELD_ENTER_POS_INFO), 1),
													NEW_BLOB(pPlayerGameData->GetMovePlayerInfo(), sizeof(TD_MOVE_PLAYER_INFO), 1),
													NEW_BLOB(pPlayerGameData->GetCharaInfo(), sizeof(TD_PLAYER_GAME_DATA_CHARAINFO), 1),
													NEW_BLOB(pPlayerGameData->GetEquip()),
													NEW_BLOB(pPlayerGameData->GetItemInstance()),
													NEW_BLOB(pPlayerGameData->GetTalent()),
													NEW_BLOB(pPlayerGameData->GetPalette()),
													NEW_BLOB(pPlayerGameData->GetTalentCollTime()),
													NEW_BLOB(pPlayerGameData->GetBuffRemain()),
													NEW_BLOB(pPlayerGameData->GetQuestInstance()),
													NEW_BLOB(pPlayerGameData->GetQuestInstanceDontDoing()),
													NEW_BLOB(pPlayerGameData->GetFactoin()),
													NEW_BLOB(pPlayerGameData->GetSawnCutscene()),
													NEW_BLOB(pPlayerGameData->GetRecipe())
													);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::RequestCancelMoveGameServer(MUID uidRequestPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_COMM_CANCEL_MOVE_GAME_SERVER,
																	1,
																	NEW_UID(uidRequestPlayer)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::ResponseReadyEnterMovePlayer(MUID uidRequestGameServer, MUID uidRequestPlayer, int nResult, MUID uidConnectionKey)
{
	TD_LOGIN_GAME_SERVER_INFO loginGameServerInfoParam;

	wstring strIP = GConfig::m_strPublicIP;
	wcsncpy_s(loginGameServerInfoParam.strHostName, strIP.c_str(), _TRUNCATE);
	loginGameServerInfoParam.nPort = (uint32)GConfig::m_nPort;
	loginGameServerInfoParam.uidConnectionKey = uidConnectionKey;

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER,
													4,
													NEW_UID(uidRequestGameServer),
													NEW_UID(uidRequestPlayer),
													NEW_INT(nResult),
													NEW_BLOB(&loginGameServerInfoParam, sizeof(loginGameServerInfoParam), 1)
													);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::AskGateSharedField(MUID uidRequestPlayer, const GGateInfo* pGateInfo)
{
	_ASSERT(pGateInfo != NULL);

	TD_GATE_INFO tdGateInfo;
	pGateInfo->Export(tdGateInfo);

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GATE_ASK_SHARED_FIELD, 
													2,
													NEW_UID(uidRequestPlayer),
													NEW_SINGLE_BLOB(&tdGateInfo, sizeof(tdGateInfo)));
	
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::AskGateDynamicField(MUID uidRequestPlayer, const GGateInfo* pGateInfo, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType, bool isSingleType)
{
	_ASSERT(pGateInfo != NULL);

	TD_GATE_FIELD_GROUP_INFO tdGateFieldGroupInfo;
	pGateInfo->Export(tdGateFieldGroupInfo.GateInfo);
	tdGateFieldGroupInfo.nFieldGroupID = nFieldGroupID;
	tdGateFieldGroupInfo.nType = (int8)eType;
	tdGateFieldGroupInfo.isSingleType = isSingleType;

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GATE_ASK_DYNAMIC_FIELD, 
													2,
													NEW_UID(uidRequestPlayer),
													NEW_SINGLE_BLOB(&tdGateFieldGroupInfo, sizeof(tdGateFieldGroupInfo)));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::PrepareChangeChannelReq(MUID uidRequestPlayer, int nChannelID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GATE_PREPARE_CHANGE_CHANNEL_REQ,
		2,
		NEW_UID(uidRequestPlayer),
		NEW_INT(nChannelID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::CancelChangeChannel(MUID uidRequestPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GATE_CANCEL_CHANGE_CHANNEL,
		1,
		NEW_UID(uidRequestPlayer));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerGateRouter::AskGateChangeChannel(MUID uidRequestPlayer, const GGateInfo* pGateInfo)
{
	_ASSERT(pGateInfo != NULL);

	TD_GATE_INFO tdGateInfo;
	pGateInfo->Export(tdGateInfo);

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GATE_ASK_CHANGE_CHANNEL,
		2,
		NEW_UID(uidRequestPlayer),
		NEW_SINGLE_BLOB(&tdGateInfo, sizeof(tdGateInfo)));

	gsys.pMasterServerFacade->Route(pNewCmd);
}