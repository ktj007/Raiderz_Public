#include "stdafx.h"
#include "XPost_Local.h"
#include "XBaseApplication.h"
#include "XConnectionManager.h"
#include "XNetwork.h"
#include "MiNetHelper.h"
#include "XStrings.h"
#include "CCommandResultTable.h"
#include "XCmdHandler_Net.h"
#include "CSNet.h"


XNetClient* XCmdHandler_Net::m_pNetClient = NULL;

XCmdHandler_Net::XCmdHandler_Net(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	m_pNetClient = (XNetClient*)pCC;

	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT,	OnDisConnect);
	SetCmdHandler(MC_LOCAL_NET_ONERROR,			OnLocalNetOnError);

	SetCmdHandler(MC_COMM_MOVE_GAME_SERVER,		OnMoveGameServer);
}

XCmdHandler_Net::~XCmdHandler_Net()
{
	m_pNetClient=NULL;
}

MCommandResult XCmdHandler_Net::OnDisConnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.ui->TargetUIEnd();
	global.script->GetGlueGameEvent().OnActTalentEnd();

	if (!gvar.Game.bDisconnectByDuplicatedLogin)
	{		
		gvar.Game.strLastErrorMsg = XCmdResultStr(CR_FAIL_CONNECT);;
	}

	mlog("Socket Disconnected\n");

	if (global.net->GetConnectionManager()->OnDisconnect() == true)
	{
		return CR_TRUE;
	}

	if ( global.app->GetCurrGameFrame() != LOGIN_FRAME_ID)
	{
		XPostLocal_ChangeFrameFrame( LOGIN_FRAME_ID);
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_Net::OnMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
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

MCommandResult XCmdHandler_Net::OnLocalNetOnError(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nErrorCode = 0;

	if( !pCommand->GetParameter(&nErrorCode, 		0, MPT_INT) )								return CR_ERROR;

	if(nErrorCode == WSAECONNREFUSED)
	{
		global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN_ERROR", MLocale::ConvUTF16ToAnsi( XGetStr( L"MSG_FAILCONNECTTOSEVER")).c_str());
	}

	return CR_TRUE;
}
