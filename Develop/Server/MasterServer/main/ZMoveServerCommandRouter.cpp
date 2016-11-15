#include "StdAfx.h"
#include "ZMoveServerCommandRouter.h"
#include "CCommandTable_Master.h"
#include "ZCommandCenter.h"


void ZMoveServerCommandRouter::RouteReserveMoveLoginServerRes(const MUID& uidRequesterGameServer, const MUID& uidPlayer, int nResult, const MUID& uidConnectionKey)
{
	RVALID(uidRequesterGameServer.IsValid());
	RVALID(uidPlayer.IsValid());
	RVALID(uidConnectionKey.IsValid());

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES
		, uidRequesterGameServer
		, 3
		, NEW_UID(uidPlayer)
		, NEW_INT(nResult)
		, NEW_UID(uidConnectionKey)
		);

	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZMoveServerCommandRouter::RouteReadyEnterLoginServerReq(const MUID& uidLoginServer, const MUID& uidRequesterGameServer, const MUID& uidPlayer, const MUID& uidConnectionKey, AID nAID, wstring strUserID)
{
	RVALID(uidLoginServer.IsValid());
	RVALID(uidRequesterGameServer.IsValid());
	RVALID(uidPlayer.IsValid());
	RVALID(uidConnectionKey.IsValid());
	RVALID(nAID > 0);
	RVALID(strUserID.size() > 0);

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_READY_ENTER_LOGINSERVER_REQ
		, uidLoginServer
		, 5
		, NEW_UID(uidRequesterGameServer)
		, NEW_UID(uidPlayer)
		, NEW_UID(uidConnectionKey)
		, NEW_INT(nAID)
		, NEW_WSTR(strUserID.c_str())
		);

	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZMoveServerCommandRouter::RouteCancelMoveLoginServer(const MUID& uidLoginServer, const MUID& uidConnectionKey)
{
	RVALID(uidLoginServer.IsValid());
	RVALID(uidConnectionKey.IsValid());

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_CANCEL_MOVE_LOGINSERVER
		, uidLoginServer
		, 1
		, NEW_UID(uidConnectionKey)
		);

	gsys.pCommandCenter->PostCommand(pCmd);
}