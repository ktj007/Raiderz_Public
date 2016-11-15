#include "stdafx.h"
#include "GCmdHandler_Master_Common.h"
#include "MCommandTable.h"
#include "CCommandTable_Master.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "CCommandResultTable.h"
#include "GAppServerFacade.h"
#include "GPlayerObjectManager.h"
#include "GServer.h"
#include "GCommandCenter.h"
#include "MCommandProfiler.h"
#include "GMsgCommandFacade.h"
#include "GPlayerObject.h"
#include "GConfig.h"
#include "GLoginLogic.h"
#include "GServerModeChanger.h"


GCmdHandler_Master_Common::GCmdHandler_Master_Common(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_ONCONNECT,				OnConnect);
	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT,			OnDisconnect);
	SetCmdHandler(MC_LOCAL_NET_ONERROR,					OnError);

	SetCmdHandler(MMC_COMM_RESPONSE_LOGIN_M2G,			OnResponseLogin);
	SetCmdHandler(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN,	OnDuplicatedPlayerLogin);

	SetCmdHandler(MMC_COMM_RESPONSE_APP_SERVER_LIST,	OnResponseAppServerList);	

	SetCmdHandler(MMC_MSG_ANNOUNCE,						OnMsgAnnounce);

	SetCmdHandler(MMC_ADMIN_CHANGE_SERVER_MODE,			OnAdminChangeServerMode);
	SetCmdHandler(MMC_GM_SUMMON,						OnGMSummon);
}

MCommandResult GCmdHandler_Master_Common::OnConnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pMasterServerFacade->OnConnect();

	return CR_TRUE;
}


MCommandResult GCmdHandler_Master_Common::OnDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	gsys.pMasterServerFacade->OnDisconnect();
	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnError(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult = 0;
	if (!pCommand->GetParameter(&nResult, 0, MPT_INT))		return CR_ERROR;

	gsys.pMasterServerFacade->OnError(nResult);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnResponseLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult = 0;
	MUID uidAlloc;
	uint8 nServerMode;

	if( !pCommand->GetParameter(&nResult,		0, MPT_INT) )	return CR_ERROR;
	if( !pCommand->GetParameter(&uidAlloc,		1, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&nServerMode,	2, MPT_UCHAR) )	return CR_ERROR;

	if (nResult != CR_SUCCESS)
	{
		mlog("MasterServer Login Failed(CommandResult = %d)\n", nResult);
		gsys.pMasterServerFacade->GetNetClient()->Disconnect();
		return CR_ERROR;
	}

	///< MasterServer 의 서버 모드로 셋팅
	GConfig::m_nServerMode = nServerMode;
	
	MCommandParameter* pParam = pCommand->GetParameter(3);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;

	TDMG_MANAGED_SHARED_FIELD* pAssignedSharedFields = (TDMG_MANAGED_SHARED_FIELD*)pParam->GetPointer();
	if (false == gsys.pMasterServerFacade->CreateManagedSharedFields(pAssignedSharedFields, pParam->GetBlobCount()))
	{
		mlog("MasterServer Login Failed : Failed Create Managed Shared Fields\n");
	}

	mlog("MasterServer Login Success : Alloc UID = (%u:%u), ServerMode = %s\n", uidAlloc.High, uidAlloc.Low, GConfig::GetServerModeANSIString().c_str());

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnDuplicatedPlayerLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	MUID uidConnectionKey;
	
	if (!pCommand->GetParameter(&uidPlayer,	0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidConnectionKey, 1, MPT_UID))	return CR_ERROR;
	

	GLoginLogic logic;
	logic.DuplicatedLogin(uidPlayer, uidConnectionKey);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnResponseAppServerList(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	vector<TD_APP_SERVER_INFO> vecAppServerList;
	if (!pCommand->GetBlob(vecAppServerList, 0))		return CR_ERROR;

	// 채팅 서버 등록
	for each(const TD_APP_SERVER_INFO& info in vecAppServerList)
	{
		if (info.nAppServerMode == APP_SERVER_CHAT)
		{
			wstring strIP = MLocale::ConvAnsiToUCS2(MiNetHelper::ConvertIPAddress(info.nIP)).c_str();
			gsys.pAppServerFacade->CreateNetwork(strIP.c_str(), info.nPort);
			gsys.pAppServerFacade->GetNetClient()->AttachCommandProfiler(new MCommandProfiler());

			gsys.pServer->CreateCommandProfileForAppServer(gsys.pAppServerFacade->GetNetClient());
		}
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnMsgAnnounce(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strMsg;

	if (!pCommand->GetParameter(strMsg, 0, MPT_WSTR))	return CR_FALSE;


	gsys.pMsgCommandFacade->RouteNoticeMsgToAll(strMsg);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnAdminChangeServerMode(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nServerMode;

	if (!pCommand->GetParameter(&nServerMode, 0, MPT_INT))	return CR_FALSE;

	GServerModeChanger serverModeChanger;
	serverModeChanger.ChangeServerModeImpl(nServerMode);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Common::OnGMSummon(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidTarPalyer;
	MUID uidFieldGroup;
	int nFieldID;
	CHANNELID nChannelID;
	vec3 vPos;

	if (!pCommand->GetParameter(&uidTarPalyer,		0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidFieldGroup,		1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID,			2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID,		3, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&vPos,				4, MPT_VEC))	return CR_ERROR;

	GEntityPlayer* pTarPlayer = gmgr.pPlayerObjectManager->GetEntity(uidTarPalyer);
	if (NULL == pTarPlayer) return CR_FALSE;
	if (true == pTarPlayer->GetPlayerField().GetFieldEntry().IsReserved()) return CR_FALSE;

	pTarPlayer->GetPlayerField().SetDynamicFieldGroupUID(uidFieldGroup);
	pTarPlayer->GetPlayerField().Gate(nFieldID, vPos, pTarPlayer->GetDir(), true, nChannelID);

	return CR_TRUE;
}