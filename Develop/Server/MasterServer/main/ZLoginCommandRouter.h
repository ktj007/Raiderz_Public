#pragma once

struct TD_LOGIN_ACCOUNT_INFO;
struct TD_LOGIN_GAME_SERVER_INFO;

class ZLoginCommandRouter
{
public:
	static void RouteRequestLoginGameServer(MUID uidTarget, MUID uidRequestServer, MUID uidRequestPlayer, MUID uidConnectionKey, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo);		
	static void RouteResponseLoginGameServer(MUID uidTarget, MUID uidRequestPlayer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo);
};
