#include "StdAfx.h"
#include "XCmdHandler_GameStart.h"
#include "XNetwork.h"
#include "XConnectionManager.h"
#include "XStrings.h"
#include "XMyPlayer.h"
#include "CCommandResultTable.h"

XCmdHandler_GameStart::XCmdHandler_GameStart(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_RESPONSE_LOGIN_GAME_SERVER,				OnResponseLoginGameServer);
	SetCmdHandler(MC_COMM_RESPONSE_MOVE_GAME_SERVER,				OnResponseMoveGameServer);
}

bool XCmdHandler_GameStart::CommonResponseLogin( MCommand* pCommand, MCommandHandler* pHandler )
{
	int nResult = 0;
	MUID uidAlloc;
	unsigned short nUIID = 0;

	if( !pCommand->GetParameter(&nResult,	0, MPT_INT) )	return false;
	if( !pCommand->GetParameter(&uidAlloc,	1, MPT_UID) )	return false;
	if( !pCommand->GetParameter(&nUIID,		2, MPT_USHORT) )	return false;

	if (nResult != CR_SUCCESS)
	{
		global.ui->ShowMessageBox(nResult);
		global.net->PostDisconnect();
		return false;
	}

	global.net->GetConnectionManager()->OnConnectedToGameServer();

	gvar.MyInfo.MyUID = uidAlloc;
	gvar.MyInfo.SetUIID(nUIID);

	global.net->GetConnectionManager()->PostGameStart();

	return true;
}

MCommandResult XCmdHandler_GameStart::OnResponseLoginGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (!CommonResponseLogin(pCommand, pHandler))
		return CR_TRUE;

	global.script->GetGlueGameEvent().OnCharacterEvent( "WORLD_ENTER", "PROGRESS_MSG", CR_ENTER_WORLD_SUCCESS_LOGIN_GAMESERVER );
	mlog("EnterWorld! (%d)SUCCESS_LOGIN_GAMESERVER\n", CR_ENTER_WORLD_SUCCESS_LOGIN_GAMESERVER);

	return CR_TRUE;
}

MCommandResult XCmdHandler_GameStart::OnResponseMoveGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	CommonResponseLogin(pCommand, pHandler);

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if (pMyPlayer)
	{
		pMyPlayer->ChangeUIID(XGetMyUIID());
	}

	gvar.MyInfo.ResetByMoveGameServer();

	return CR_TRUE;
}

