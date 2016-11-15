#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZServerAcceptor;

/// 통신관련 커맨드 핸들러
class ZCmdHandler_Comm : public MCommandHandler
{
private:
	static bool CheckValidLoginReqCommand( ZServerAcceptor* pAcceptor, int nCommandVersion, int nWorldID );
	static bool CommLoginServer( ZServerAcceptor* pAcceptor );

public:
	ZCmdHandler_Comm(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnNetClear);
	DECL_CMD_HANDLER(OnGameServerLogin);
	DECL_CMD_HANDLER(OnLoginServerLogin);
	DECL_CMD_HANDLER(OnAppServerLogin);
	DECL_CMD_HANDLER(OnRequestAppServerList);

	DECL_CMD_HANDLER(OnRequestAddPlayer);
	DECL_CMD_HANDLER(OnDeletePlayer_FromLoginServer);
	DECL_CMD_HANDLER(OnDeletePlayer_FromGameServer);

	DECL_CMD_HANDLER(OnPlayerStateSelectChar);
	DECL_CMD_HANDLER(OnPlayerStateInWorldReq);
	DECL_CMD_HANDLER(OnPlayerInFieldNotify);

	// 피망
	DECL_CMD_HANDLER(OnPmangRequestAddPlayer);

	// Admin
	DECL_CMD_HANDLER(OnAdminChangeServerModeReq);

	// Debug
	DECL_CMD_HANDLER(OnDebugDump);

	// GM
	DECL_CMD_HANDLER(OnGMSummonReq);
};
