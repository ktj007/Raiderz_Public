#include "StdAfx.h"
#include "GClientMoveServerRouter.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"
#include "GCommand.h"
#include "CCommandResultTable.h"
#include "GGlobal.h"
#include "GCommandCenter.h"


GClientMoveServerRouter::GClientMoveServerRouter(void)
{
}

GClientMoveServerRouter::~GClientMoveServerRouter(void)
{
}

void GClientMoveServerRouter::RouteResponseLoginGameServer(const MUID& uidTargetPlayer, int nResult, const MUID& uidPlayer/*=MUID::ZERO*/, UIID nUIID/*=0*/)
{
	RVALID(uidTargetPlayer.IsValid());
	if (nResult == CR_SUCCESS)
		RVALID(uidPlayer.IsValid() && nUIID != 0);

	MCommand* pNewCommand = MakeNewCommand(MC_COMM_RESPONSE_LOGIN_GAME_SERVER
		, uidTargetPlayer
		, 3
		, NEW_INT(nResult)
		, NEW_UID(uidPlayer)
		, NEW_USHORT(nUIID)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GClientMoveServerRouter::RouteResponseMoveGameServer(const MUID& uidTargetPlayer, int nResult, const MUID& uidPlayer/*=MUID::ZERO*/, UIID nUIID/*=0*/)
{
	RVALID(uidTargetPlayer.IsValid());
	if (nResult == CR_SUCCESS)
		RVALID(uidPlayer.IsValid() && nUIID != 0);

	MCommand* pNewCommand = MakeNewCommand(MC_COMM_RESPONSE_MOVE_GAME_SERVER
		, uidTargetPlayer
		, 3
		, NEW_INT(nResult)
		, NEW_UID(uidPlayer)
		, NEW_USHORT(nUIID)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GClientMoveServerRouter::RouteStartMoveLoginServerRes(const MUID& uidTargetPlayer, int nResult, const MUID& uidConnectionKey)
{
	MCommand* pNewCommand = MakeNewCommand(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES
		, uidTargetPlayer
		, 2
		, NEW_INT(nResult)
		, NEW_UID(uidConnectionKey)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}
