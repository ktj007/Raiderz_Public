#ifndef _XCMD_HANDLER_FATIGUE_H
#define _XCMD_HANDLER_FATIGUE_H

class XCmdHandler_Fatigue  : public MCommandHandler
{
public:
	XCmdHandler_Fatigue(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnFatigueUpdate);
};

#endif//_XCMD_HANDLER_FATIGUE_H
