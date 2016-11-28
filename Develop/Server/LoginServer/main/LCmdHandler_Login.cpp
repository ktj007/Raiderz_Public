#include "stdafx.h"
#include "LCmdHandler_Login.h"
#include "MCommand.h"
#include "LCommandTable.h"
#include "LPlayerObject.h"
#include "LCommandCenter.h"
#include "LDBManager.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LMasterServerMgr.h"
#include "LTimeoutManager.h"
#include "LGlobal.h"
#include "LConfig.h"
#include "LCmdRouter_Player.h"
#include "LPlayerLogic.h"
#include "LLoginLogic.h"
#include "SCmdRouter_Login.h"


LCmdHandler_Login::LCmdHandler_Login(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{	
	SetCmdHandler(MC_COMM_REQUEST_LOGIN,				OnLogin);
	SetCmdHandler(MC_COMM_REQUEST_LOGIN_ON_PMANG,		OnPmangLogin);

	SetCmdHandler(MC_LOCAL_NET_CLEAR,					OnNetClear);

	SetCmdHandler(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST,	OnRequestAccountCharList);
	SetCmdHandler(MC_CHAR_REQUEST_INSERT_CHAR,			OnRequestInsertCharacter);
	SetCmdHandler(MC_CHAR_REQUEST_DELETE_CHAR,			OnRequestDeleteCharacter);

	SetCmdHandler(MC_CHAR_REQUEST_SEL_MYCHAR,			OnRequestSelMyChar);
}

MCommandResult LCmdHandler_Login::OnLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	wstring strUserID;
	wstring strPassword;
	int nCommandVersion;	

	if (!pCommand->GetParameter(strUserID,			0, MPT_WSTR))	return CR_FALSE;
	if (!pCommand->GetParameter(strPassword,		1, MPT_WSTR))	return CR_FALSE;
	if (!pCommand->GetParameter(&nCommandVersion,	2, MPT_INT))	return CR_FALSE;

	
	// 요청자 확인
	MUID uidPlayer = pCommand->GetSenderUID();
	if (uidPlayer.IsInvalid()) return CR_FALSE;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 커맨드 버전 확인	
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);
		return CR_TRUE;
	}

	if (!LConfig::IsInGameLoginAllowed())
	{
		cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN);
		return CR_TRUE;
	}
	
	// 로그인 요청
	LLoginLogic loginLogic;

	if (!loginLogic.Login(uidPlayer, strUserID, strPassword))
	{
		cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN_POST_DB);
		return CR_TRUE;
	}

	return CR_TRUE;
}

MCommandResult LCmdHandler_Login::OnPmangLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nCommandVersion;
	wstring strLoginStream;

	if (!pCommand->GetParameter(&nCommandVersion, 0, MPT_INT))	return CR_FALSE;
	if (!pCommand->GetParameter(strLoginStream,	1,	MPT_WSTR))	return CR_FALSE;

	
	// 요청자 확인
	MUID uidPlayer = pCommand->GetSenderUID();
	if (uidPlayer.IsInvalid()) return CR_FALSE;


	// 커맨드 버전 확인
	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		cmdRouter.ResponsePmangLogin(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);		
		return CR_TRUE;
	}


	// 로그인 요청
	LLoginLogic loginLogic;	
	
	if (!loginLogic.PmangLogin(uidPlayer, strLoginStream))
	{
		cmdRouter.ResponsePmangLogin(uidPlayer, CR_FAIL_LOGIN_POST_DB);
		return CR_TRUE;		
	}


	return CR_TRUE;
}

MCommandResult LCmdHandler_Login::OnNetClear(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;

	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_FALSE;

	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayer == NULL) return CR_FALSE;

	LPlayerLogic playerLogic;
	playerLogic.OnDisconnectPlayer(uidPlayer);

	return CR_TRUE;
}

MCommandResult LCmdHandler_Login::OnRequestAccountCharList(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	LPlayerLogic logic;
	logic.RequestAccountCharList(uidPlayer);
	
	return CR_TRUE;
}

MCommandResult LCmdHandler_Login::OnRequestInsertCharacter(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	TD_INSERT_CHARACTER_INFO* pInsertCharInfo;

	if (pCommand->GetSingleBlob(pInsertCharInfo,		0)==false) return CR_FALSE;

	LPlayerLogic logic;
	logic.InsertCharacter(uidPlayer, pInsertCharInfo);
	return CR_TRUE;
}

MCommandResult LCmdHandler_Login::OnRequestDeleteCharacter(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	int8 nCharIndex;
	if (pCommand->GetParameter(&nCharIndex,		0, MPT_CHAR)==false) return CR_FALSE;

	LPlayerLogic logic;
	logic.DeleteCharacter(uidPlayer, nCharIndex);
	return CR_TRUE;
}

MCommandResult LCmdHandler_Login::OnRequestSelMyChar(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	int nCharIndex;
	if (pCommand->GetParameter(&nCharIndex,		0, MPT_INT)==false) return CR_FALSE;

	LPlayerLogic logic;
	logic.SelMyChar(uidPlayer, nCharIndex);
	return CR_TRUE;
}
