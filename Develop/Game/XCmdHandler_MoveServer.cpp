#include "StdAfx.h"
#include "XCmdHandler_MoveServer.h"
#include "MiNetHelper.h"
#include "XNetwork.h"
#include "XConnectionManager.h"
#include "CSNet.h"
#include "CCommandResultTable.h"
#include "XPost_Comm.h"


XCmdHandler_MoveServer::XCmdHandler_MoveServer(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_MOVE_GAME_SERVER,		OnMoveGameServer);	///< From GS To GS
	
	SetCmdHandler(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES,	OnStartMoveToLoginServerRes);	///< LoginServer 로 이동 시작.
	SetCmdHandler(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER,		OnResponseMoveLoginServer);		///< LoginServer 로 이동해서 인증 요청.
}

MCommandResult XCmdHandler_MoveServer::OnMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;

	int nBlobCount = pParam->GetBlobCount();
	if (nBlobCount <= 0) return CR_TRUE;

	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo = (TD_LOGIN_GAME_SERVER_INFO*)pParam->GetPointer();
	if (pLoginGameServerInfo == NULL) return CR_TRUE;


	wstring strIP = pLoginGameServerInfo->strHostName;
	int nPort = (int)pLoginGameServerInfo->nPort;

	global.net->GetConnectionManager()->MoveFromGameServerToGameServer(strIP, nPort, pLoginGameServerInfo->uidConnectionKey);

	return CR_TRUE;
}

MCommandResult XCmdHandler_MoveServer::OnStartMoveToLoginServerRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult;
	MUID uidConnectionKey;

	if ( !pCommand->GetParameter(&nResult,			0, MPT_INT) )	return CR_ERROR;
	if ( !pCommand->GetParameter(&uidConnectionKey,	1, MPT_UID) )	return CR_ERROR;

	int nPort = DEFAULT_LOGIN_SERVER_PORT;
	if (gvar.Network.bLoginToGameServer)
		nPort = DEFAULT_GAME_SERVER_PORT;

	// 서버 이동처리 시작. GameServer Disconnect.
	global.net->GetConnectionManager()->MoveFromGameServerToLoginServer(gvar.Network.szIP, nPort, uidConnectionKey);

	/// TODO : 게임에서 사용하던 데이터들 초기화 필요. - aibeast (20110223)

	gvar.MyInfo.Clear();

	return CR_TRUE;
}

MCommandResult XCmdHandler_MoveServer::OnResponseMoveLoginServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult;
	MUID uidAllocUID;

	if ( !pCommand->GetParameter(&nResult,		0, MPT_INT) )	return CR_ERROR;
	if ( !pCommand->GetParameter(&uidAllocUID,	1, MPT_UID) )	return CR_ERROR;

	if (nResult != CR_SUCCESS)
	{
		/// TODO : 에러메시지 출력. 현재 프레임에 맞게 적절한 메시지 박스 출력 필요. - aibeast (20110223)
		/// (XCmdHandler_GameStart.cpp) XCmdHandler_GameStart::CommonResponseLogin() 함수의 실패처리와 동일하게 처리합니다.
		//global.ui->ShowMessageBox(nResult);
		global.net->PostDisconnect();		///< 여기서 실패하면 접속 종료.
		return CR_TRUE;
	}

	global.net->GetConnectionManager()->OnConnectedToLoginServer();
	gvar.MyInfo.MyUID = uidAllocUID;

	// 캐릭터 목록 요청.
	XPostChar_RequestAccountCharList();

	return CR_TRUE;
}
