#ifndef _MCMD_HANDLER_NETCLIENT_H
#define _MCMD_HANDLER_NETCLIENT_H

#include "MCommandHandler.h"

namespace minet {

class MNetClient;

class MCmdHandler_NetClient : public MCommandHandler
{
private:
	static MNetClient* NetClient(MCommandHandler* pHandler);
public:
	MCmdHandler_NetClient(MCommandCommunicator* pCC);
	virtual ~MCmdHandler_NetClient();

	DECL_CMD_HANDLER( OnNetReplyConnect			);
	
	DECL_CMD_HANDLER( OnLocalNetConnect			);
	DECL_CMD_HANDLER( OnLocalNetConnectWIthNIC	);
	DECL_CMD_HANDLER( OnLocalNetDisconnect		);
};


} // namespace minet

#endif