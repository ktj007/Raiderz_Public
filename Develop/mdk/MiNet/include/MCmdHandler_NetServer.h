#ifndef _MCMD_HANDLER_NETSERVER_H
#define _MCMD_HANDLER_NETSERVER_H

#include "MCommandHandler.h"

namespace minet {

class MNetServer;

class MCmdHandler_NetServer : public MCommandHandler
{
private:
	static MNetServer* NetServer(MCommandHandler* pHandler);
public:
	MCmdHandler_NetServer(MCommandCommunicator* pCC);
	virtual ~MCmdHandler_NetServer();
	DECL_CMD_HANDLER(OnLocal_NetOnAccept);
	DECL_CMD_HANDLER(OnLocal_NetOnDisconnect);
	DECL_CMD_HANDLER(OnLocal_NetOnHardDisconnect);
};

} // namespace minet

#endif