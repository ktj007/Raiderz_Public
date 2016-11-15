#ifndef _XNET_COMMAND_HANDLER_H
#define _XNET_COMMAND_HANDLER_H

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Net : public MCommandHandler
{
friend class XNetClient;

private:
protected:
	static XNetClient*		m_pNetClient;
public:
	XCmdHandler_Net(MCommandCommunicator* pCC);
	virtual ~XCmdHandler_Net();

	DECL_CMD_HANDLER(OnDisConnect);
	DECL_CMD_HANDLER(OnLocalNetOnError);

	DECL_CMD_HANDLER(OnMoveGameServer);
};



#endif