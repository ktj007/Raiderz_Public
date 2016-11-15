#pragma once

class GMoveServerRouter
{
public:
	virtual ~GMoveServerRouter() {}

	virtual void RouteReserveMoveLoginServerReq(const MUID& uidRequesterPlayer) = 0;
	virtual void RouteCancelMoveLoginServer(const MUID& uidRequesterPlayer) = 0;
};
