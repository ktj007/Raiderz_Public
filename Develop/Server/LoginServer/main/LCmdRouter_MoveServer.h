#pragma once

class LCmdRouter_MoveServer
{
public:
	/// For MasterServer
	static void RouteReadyEnterLoginServerRes(const MUID& uidRequesterGameServer, const MUID& uidRequesterPlayer, int nResult, MUID uidConnectionKey);

	/// For Cient
	static void RouteResponseMoveLoginServer(const MUID& uidPlayer, int nResult, const MUID& uidAllocUID);

};
