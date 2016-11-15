#pragma once

#include "GMoveServerRouter.h"

class GStandAloneModeMoveServerRouter :public GMoveServerRouter, public MTestMemPool<GStandAloneModeMoveServerRouter>
{
public:
	GStandAloneModeMoveServerRouter(void);
	virtual ~GStandAloneModeMoveServerRouter(void);

	virtual void RouteReserveMoveLoginServerReq(const MUID& uidRequesterPlayer) override;
	virtual void RouteCancelMoveLoginServer(const MUID& uidRequesterPlayer) override;
};
