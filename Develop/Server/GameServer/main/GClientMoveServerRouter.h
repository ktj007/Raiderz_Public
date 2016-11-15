#pragma once

class GClientMoveServerRouter
{
public:
	GClientMoveServerRouter(void);
	virtual ~GClientMoveServerRouter(void);

	void RouteResponseLoginGameServer(const MUID& uidTargetPlayer, int nResult, const MUID& uidPlayer=MUID::ZERO, UIID nUIID=0);
	void RouteResponseMoveGameServer(const MUID& uidTargetPlayer, int nResult, const MUID& uidPlayer=MUID::ZERO, UIID nUIID=0);

	void RouteStartMoveLoginServerRes(const MUID& uidTargetPlayer, int nResult, const MUID& uidConnectionKey);
};
