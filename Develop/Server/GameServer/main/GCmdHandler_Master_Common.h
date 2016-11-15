#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Common : public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Common>
{
private:

public:
	GCmdHandler_Master_Common(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnConnect);
	DECL_CMD_HANDLER(OnDisconnect);
	DECL_CMD_HANDLER(OnError);

	DECL_CMD_HANDLER(OnResponseLogin);
	DECL_CMD_HANDLER(OnDuplicatedPlayerLogin);

	DECL_CMD_HANDLER(OnResponseAppServerList);
	DECL_CMD_HANDLER(OnMsgAnnounce);

	DECL_CMD_HANDLER(OnAdminChangeServerMode);
	DECL_CMD_HANDLER(OnGMSummon);
};
