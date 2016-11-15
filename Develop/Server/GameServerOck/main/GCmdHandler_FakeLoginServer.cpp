#include "stdafx.h"
#include "GCmdHandler_FakeLoginServer.h"
#include "MCommand.h"
#include "GCommandTable.h"
#include "GServer.h"
#include "GCommand.h"
#include "GDBManager.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "SCmdRouter_Login.h"
#include "GCommandCenter.h"
#include "CCommandResultTable.h"
#include "GCalculator.h"
#include "GFakeLoginLogic.h"
#include "GConfig.h"
#include "GDBTaskDataAccount.h"
#include "GFakeLoginPlayerAcceptor_FromGameServerToLoginServer.h"
#include "GPlayerAcceptManager.h"


GCmdHandler_FakeLoginServer::GCmdHandler_FakeLoginServer(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_REQUEST_LOGIN,				OnRequestLogin);
	SetCmdHandler(MC_COMM_REQUEST_ACCOUNT_CHAR_LIST,	OnRequestAccountCharList);
	SetCmdHandler(MC_CHAR_REQUEST_INSERT_CHAR,			OnRequestInsertCharacter);
	SetCmdHandler(MC_CHAR_REQUEST_DELETE_CHAR,			OnRequestDeleteCharacter);
	SetCmdHandler(MC_CHAR_REQUEST_SEL_MYCHAR,			OnRequestSelMyChar);

	SetCmdHandler(MC_COMM_REQUEST_MOVE_LOGIN_SERVER,	OnRequestMoveLoginServer);
}

MCommandResult GCmdHandler_FakeLoginServer::OnRequestLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strUserID;
	wstring strPassword;
	int nCommandVersion;	

	if (pCommand->GetParameter(strUserID,		0, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(strPassword,		1, MPT_WSTR)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,2, MPT_INT)==false) return CR_FALSE;


	MUID uidPlayer = pCommand->GetSenderUID();
	if (uidPlayer.IsInvalid()) return CR_FALSE;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN_COMMAND_INVALID_VERSION);
		return CR_TRUE;
	}

	if (!GConfig::m_bStandAlone)
	{
		cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN_NOT_STAND_ALONE_SERVER);
		return CR_TRUE;
	}

	GPlayerObject* pNewPlayer = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddPlayer(pNewPlayer);
	pNewPlayer->OnBeginLogin();

	GDBT_ACC_LOGIN data(uidPlayer, strUserID, L"", strUserID, strPassword, "testing");

	// 로그인 요청
	if (!gsys.pDBManager->LoginGetInfo(data))
	{
		gmgr.pPlayerObjectManager->DeletePlayer(uidPlayer);

		cmdRouter.ResponseLogin(uidPlayer, CR_FAIL_LOGIN_POST_DB);
		return CR_TRUE;
	}


	if (GConfig::m_bNoDB)
	{
		//GFakeLoginLogic logic;
		//logic.RequestAccountCharList(uidPlayer);
		//logic.SelMyChar(uidPlayer, 0);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_FakeLoginServer::OnRequestAccountCharList(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	GFakeLoginLogic logic;
	logic.RequestAccountCharList(uidPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_FakeLoginServer::OnRequestInsertCharacter(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	TD_INSERT_CHARACTER_INFO* pInsertCharInfo;

	if (pCommand->GetSingleBlob(pInsertCharInfo,		0)==false) return CR_FALSE;


	GFakeLoginLogic logic;
	logic.InsertCharacter(uidPlayer, pInsertCharInfo);

	return CR_TRUE;
}

MCommandResult GCmdHandler_FakeLoginServer::OnRequestDeleteCharacter(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCommand->GetSenderUID();

	int8 nCharIndex;
	if (pCommand->GetParameter(&nCharIndex,		0, MPT_CHAR)==false) return CR_FALSE;

	GFakeLoginLogic logic;
	logic.DeleteCharacter(uidPlayer, nCharIndex);

	return CR_TRUE;
}


MCommandResult GCmdHandler_FakeLoginServer::OnRequestSelMyChar(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCharIndex=0;
	if (pCommand->GetParameter(&nCharIndex,	0, MPT_INT)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();

	GFakeLoginLogic logic;
	logic.SelMyChar(uidPlayer, nCharIndex);

	return CR_TRUE;
}

MCommandResult GCmdHandler_FakeLoginServer::OnRequestMoveLoginServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConnectionKey;
	int nCommandVersion;	

	if (pCommand->GetParameter(&uidConnectionKey,	0, MPT_UID)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nCommandVersion,	1, MPT_INT)==false) return CR_FALSE;


	MUID uidPlayer = pCommand->GetSenderUID();
	if (uidPlayer.IsInvalid()) return CR_FALSE;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);

	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		RouteResponseMoveLoginServer(CR_FAIL_LOGIN_COMMAND_INVALID_VERSION, MUID::ZERO);
		return CR_TRUE;
	}

	GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	GFakeLoginPlayerAcceptor_FromGameServerToLoginServer* pAcceptor =  pAcceptManager->FindFakeLoginPlayerAcceptor_FromGameServerToLoginServer(uidConnectionKey);
	if (pAcceptor == NULL)
	{
		RouteResponseMoveLoginServer(CR_FAIL_LOGIN, MUID::ZERO);
		return CR_TRUE;
	}

	GPlayerObject* pNewPlayer = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddPlayer(pNewPlayer);

	AID nAID = pAcceptor->GetAID();
	wstring strUserID = pAcceptor->GetUserID();

	pNewPlayer->InitAccountInfo(nAID, strUserID, false);

	pAcceptManager->Del(uidConnectionKey, GPlayerAcceptor::FAKE_LOGIN_FROM_GAME_SERVER_TO_LOGIN_SERVER);
	pAcceptManager->Update();

	RouteResponseMoveLoginServer(CR_SUCCESS, uidPlayer);

	return CR_TRUE;
}
void GCmdHandler_FakeLoginServer::RouteResponseMoveLoginServer(int nResult, MUID uidPlayer)
{
	MCommand* pNewCommand = MakeNewCommand(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER
		, uidPlayer
		, 2
		, NEW_INT(nResult)
		, NEW_UID(uidPlayer)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}