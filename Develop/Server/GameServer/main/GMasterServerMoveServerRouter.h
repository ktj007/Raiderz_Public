#pragma once

#include "GMoveServerRouter.h"

class GMasterServerMoveServerRouter : public GMoveServerRouter, public MTestMemPool<GMasterServerMoveServerRouter>
{
public:
	GMasterServerMoveServerRouter(void);
	virtual ~GMasterServerMoveServerRouter(void);

	virtual void RouteReserveMoveLoginServerReq(const MUID& uidRequesterPlayer) override;
	virtual void RouteCancelMoveLoginServer(const MUID& uidRequesterPlayer) override;
};
