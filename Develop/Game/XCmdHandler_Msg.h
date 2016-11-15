#ifndef _XCMD_HANDLER_MSG_H
#define _XCMD_HANDLER_MSG_H

#include "MCommandHandler.h"
using namespace minet;

/// 메세지 처리 관련 핸들러
class XCmdHandler_Msg  : public MCommandHandler
{
private:

protected:
	static wstring FilterPvPArea( wchar_t* szPlayerID, wchar_t* szMsg );	

	DECL_CMD_HANDLER(OnNpcSay);
	DECL_CMD_HANDLER(OnNpcBalloon);
	DECL_CMD_HANDLER(OnNpcShout);
	DECL_CMD_HANDLER(OnNpcNarration);	
	DECL_CMD_HANDLER(OnNpcSayAndNarration);	

	DECL_CMD_HANDLER(OnMsgRes);

public:
	XCmdHandler_Msg(MCommandCommunicator* pCC);
};



#endif