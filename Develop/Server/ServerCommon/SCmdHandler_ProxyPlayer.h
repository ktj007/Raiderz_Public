#pragma once

#include "MCommandHandler.h"

using namespace minet;

class SProxyPlayerManager;

class SCmdHandler_ProxyPlayer: public MCommandHandler
{
public:
	SCmdHandler_ProxyPlayer(MCommandCommunicator* pCC, SProxyPlayerManager* pManager);
	virtual ~SCmdHandler_ProxyPlayer();

	DECL_CMD_HANDLER(OnProxyPlayer);
	DECL_CMD_HANDLER(OnProxyPlayerAdd);
	DECL_CMD_HANDLER(OnProxyPlayerRemove);
	DECL_CMD_HANDLER(OnProxyPlayerUpdate);
	DECL_CMD_HANDLER(OnProxyPlayerRefresh);

private:
	static SProxyPlayerManager* sm_pManager;
};
