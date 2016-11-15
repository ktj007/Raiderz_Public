#include "StdAfx.h"
#include "GMasterServerMoveServerRouter.h"
#include "GCommand.h"
#include "CCommandTable_Master.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"


GMasterServerMoveServerRouter::GMasterServerMoveServerRouter(void)
{
}

GMasterServerMoveServerRouter::~GMasterServerMoveServerRouter(void)
{
}

void GMasterServerMoveServerRouter::RouteReserveMoveLoginServerReq(const MUID& uidRequesterPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_COMM_RESERVE_MOVE_LOGINSERVER_REQ
		, 1
		, NEW_UID(uidRequesterPlayer)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerMoveServerRouter::RouteCancelMoveLoginServer(const MUID& uidRequesterPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_COMM_CANCEL_MOVE_LOGINSERVER
		, 1
		, NEW_UID(uidRequesterPlayer)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}
