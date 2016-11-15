#include "StdAfx.h"
#include "GStandAloneModeMoveServerRouter.h"
#include "GFakeLoginLogic.h"


GStandAloneModeMoveServerRouter::GStandAloneModeMoveServerRouter(void)
{
}

GStandAloneModeMoveServerRouter::~GStandAloneModeMoveServerRouter(void)
{
}

void GStandAloneModeMoveServerRouter::RouteReserveMoveLoginServerReq(const MUID& uidRequesterPlayer)
{
	//
	GFakeLoginLogic logic;
	logic.MoveToLoginServer(uidRequesterPlayer);
}

void GStandAloneModeMoveServerRouter::RouteCancelMoveLoginServer(const MUID& uidRequesterPlayer)
{

}
