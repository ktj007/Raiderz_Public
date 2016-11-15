#include "stdafx.h"
#include "LCmdHandler_Master_Global.h"
#include "MCommand.h"
#include "LCommandTable.h"
#include "LGlobal.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "LCmdRouter_Player.h"
#include "SCmdRouter_Login.h"
#include "LCommandCenter.h"
#include "LServableChecker.h"
#include "LPlayerLogic.h"
#include "LConfig.h"


LCmdHandler_Master_Global::LCmdHandler_Master_Global(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MLC_COMM_RESPONSE_LOGIN,				OnResponseLogin);
	SetCmdHandler(MLC_PLAYER_ADD_PLAYER_RES,			OnResponseAddPlayer);
	SetCmdHandler(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN,	OnDuplicatedPlayerLogin);

	SetCmdHandler(MLC_PLAYER_PMANG_ADD_PLAYER_RES,		OnPmangResponseAddPlayer);

	SetCmdHandler(MLC_ADMIN_CHANGE_SERVER_MODE,			OnAdminChangeServerMode);
}

MCommandResult LCmdHandler_Master_Global::OnResponseLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult = 0;
	MUID uidAlloc;
	uint8 nServerMode;
	std::vector<TD_SERVER_LIST_NODE> vecServerList;

	if( !pCommand->GetParameter(&nResult,		0, MPT_INT) )	return CR_ERROR;
	if( !pCommand->GetParameter(&uidAlloc,		1, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&nServerMode,	2, MPT_UCHAR) )	return CR_ERROR;
	if( !pCommand->GetBlob(vecServerList, 3))	return CR_ERROR;

	if (nResult != CR_SUCCESS)
	{
		mlog("MasterServer Login Failed - (CommandResult = %d)\n", nResult);
		gsys.pServer->GetMasterClient()->Post_Disconnect();

		return CR_ERROR;
	}

	///< MasterServer 의 서버 모드로 셋팅
	LConfig::m_nServerMode = nServerMode;

	gsys.pServer->GetServableChecker().Start(vecServerList);

	mlog("MasterServer Login Success : Alloc UID = (%u:%u), ServerMode = %s\n", uidAlloc.High, uidAlloc.Low, LConfig::GetServerModeANSIString().c_str());

	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_Global::OnResponseAddPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	int nResult;

	if( !pCommand->GetParameter(&uidPlayer,	0, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&nResult,	1, MPT_INT) )	return CR_ERROR;

	/// 플레이어 체크
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayer) return CR_TRUE;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseLogin(uidPlayer, (CCommandResultTable)nResult, LConfig::m_nServerMode);

	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_Global::OnDuplicatedPlayerLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	MUID uidConnectionKey;

	if( !pCommand->GetParameter(&uidPlayer,			0, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&uidConnectionKey,	1, MPT_UID) )	return CR_ERROR;

	LPlayerLogic playerLogic;
	playerLogic.DuplicatedPlayerLogin(uidPlayer, uidConnectionKey);

	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_Global::OnPmangResponseAddPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	int nResult;

	if( !pCommand->GetParameter(&uidPlayer,	0, MPT_UID) )	return CR_ERROR;
	if( !pCommand->GetParameter(&nResult,	1, MPT_INT) )	return CR_ERROR;

	/// 플레이어 체크
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayer) return CR_TRUE;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponsePmangLogin(uidPlayer, (CCommandResultTable)nResult, LConfig::m_nServerMode);

	return CR_TRUE;
}

MCommandResult LCmdHandler_Master_Global::OnAdminChangeServerMode(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nServerMode;

	if (!pCommand->GetParameter(&nServerMode,	0, MPT_INT))		return CR_ERROR;

	LConfig::m_nServerMode = nServerMode;

	return CR_TRUE;
}
