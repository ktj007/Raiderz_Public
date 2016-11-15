#ifndef _GCMD_HANDLER_GAME_H
#define _GCMD_HANDLER_GAME_H

#include "MCommandHandler.h"
using namespace minet;

/// 게임관련 커맨드 핸들러 - 일단 이곳에 만들어두다가 너무 많아지면 분리하자.
class GCmdHandler_Game : public MCommandHandler, public MTestMemPool<GCmdHandler_Game>
{
private:

public:
	GCmdHandler_Game(MCommandCommunicator* pCC);

	
};


#endif