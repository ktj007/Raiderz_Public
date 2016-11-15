#pragma once

#include "MMockLink.h"

class MockPlayerObject;

class MockLink : public minet::MMockLink
{
protected:
private:
protected:
	MockPlayerObject*			m_pMockPlayerObject;
public:
	MockLink(MUID uidNew, minet::MNetServer* pNetServer, minet::MServerTcpSocket& tcp_socket, minet::MCommandManager* pCM);
	virtual ~MockLink();

	void AttachMockPlayerObject(MockPlayerObject* pMockPlayerObject);
	virtual minet::MCommandResult OnRecvCommand(minet::MCommand& command);
};
