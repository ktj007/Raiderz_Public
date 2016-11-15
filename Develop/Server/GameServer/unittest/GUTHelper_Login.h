#pragma once

#include "MTypes.h"

class GEntityPlayer;

/// 로그인 관련 헬퍼 클래스
class GUTHelper_Login
{
public:
	static MCommand* NewCommand_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER( GEntityPlayer* pEntityPlayer, MUID uidConnectionKey, MUID uidGameServer, MUID uidNetClient );
};