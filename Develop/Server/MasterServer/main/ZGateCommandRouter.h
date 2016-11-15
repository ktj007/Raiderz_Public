#pragma once

namespace minet
{
	class MCommand;
}
struct TD_LOGIN_GAME_SERVER_INFO;
struct TD_GATE_INFO;

class ZGateCommandRouter
{
public:	
	static void RouteResponseReserveMoveGameServer(MUID uidTarget, MUID uidRequestPlayer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo);
	static void RouteRequestReadyEnterMovePlayer(MUID uidTarget, MUID uidConnectionKey, MUID uidRequestPlayer, MUID uidRequestServer, const minet::MCommand* pCommand);
	static void RouteGateMoveServer(MUID uidTarget, MUID uidRequestPlayer, int nMoveTargetServerID, const TD_GATE_INFO* pGateInfo);
	static void RouteCancelReservedMovePlayer(MUID uidTarget, MUID uidConnectionKey);
	static void RouteGateRes(MUID uidTarget, MUID uidRequestPlayer, RESPONSE_GATE_TYPE nResponseGateType, const TD_GATE_INFO* pGateInfo);
	static void RouteGatePrepareChangeChannelRes(MUID uidTarget, MUID uidRequestPlayer, int nResult, int nChannelID);
};
