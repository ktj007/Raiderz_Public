#pragma once

#include "MCommandHandler.h"
using namespace minet;

#include "CSDef.h"

class GPlayerObject;
class GPlayerAcceptor;

/// 게임 서버에 로그인 관련 커맨드 핸들러
class GCmdHandler_Login : public MCommandHandler, public MTestMemPool<GCmdHandler_Login>
{
public:
	GCmdHandler_Login(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestLoginGameServer);			///< 로그인 서버를 통한 로그인
	DECL_CMD_HANDLER(OnRequestMoveGameServer);			///< 다른 게임서버로부터 이동해온 플레이어 로그인

	DECL_CMD_HANDLER(OnGameStart);						///< 게임 시작

	DECL_CMD_HANDLER(OnRequestLoginGameServerOnPmang);	///< 피망 로그인 시, 로그인 서버를 통한 게임서버 접속
	DECL_CMD_HANDLER(OnRequestMoveGameServerOnPmang);	///< 피망 로그인 시, 다른 게임서버로부터 이동

private:
	static bool CheckServerClosing(MUID uidPlayer);
};
