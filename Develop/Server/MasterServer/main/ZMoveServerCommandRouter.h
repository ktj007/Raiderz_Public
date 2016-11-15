#pragma once

class ZMoveServerCommandRouter
{
public:
	static void RouteReserveMoveLoginServerRes(const MUID& uidRequesterGameServer, const MUID& uidPlayer, int nResult, const MUID& uidConnectionKey);
	static void RouteReadyEnterLoginServerReq(const MUID& uidLoginServer, const MUID& uidRequesterGameServer, const MUID& uidPlayer, const MUID& uidConnectionkey, AID nAID, wstring strUserID);
	static void RouteCancelMoveLoginServer(const MUID& uidLoginServer, const MUID& uidConnectionkey);

};
