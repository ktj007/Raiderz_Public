#pragma once

#include "MMockLink.h"

class MockLink : public minet::MMockLink
{
protected:
public:
	MockLink(MUID uidNew, minet::MNetServer* pNetServer, minet::MServerTcpSocket& tcp_socket, minet::MCommandManager* pCM);
	virtual ~MockLink();
	virtual minet::MCommandResult OnRecvCommand(minet::MCommand& command);
};
