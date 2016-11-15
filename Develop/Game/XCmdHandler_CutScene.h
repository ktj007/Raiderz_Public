#ifndef _XCMD_HANDLER_CUTSCENE_H
#define _XCMD_HANDLER_CUTSCENE_H

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_CutScene : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnCutSceneBeginMe);
	DECL_CMD_HANDLER(OnCutSceneBeginOther);
	DECL_CMD_HANDLER(OnCutSceneEndMe);
	DECL_CMD_HANDLER(OnCutSceneEndOther);

public:
	XCmdHandler_CutScene(MCommandCommunicator* pCC);

};

#endif //_XCMD_HANDLER_CUTSCENE_H
