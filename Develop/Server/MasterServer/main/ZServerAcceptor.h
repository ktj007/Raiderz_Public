#pragma once

#include "MUID.h"
#include "MCommand.h"

class ZServerAcceptor
{
protected:
	MUID				m_uidClient;
	int					m_nServerID;
	SServerType			m_nServerType;

public:
	ZServerAcceptor(SServerType nServerType, MUID uidClient, int nServerID) : m_nServerType(nServerType), m_uidClient(uidClient), m_nServerID(nServerID) {}
	virtual ~ZServerAcceptor() {}


	virtual bool IsAlreadyExist() = 0;
	virtual bool Accept() = 0;
	virtual minet::MCommand* MakeResponseCommand() = 0;
	virtual minet::MCommand* MakeFailCommand(int nCommandResult) = 0;
};
