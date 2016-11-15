#pragma once

#include "ZServer.h"

class MockServer : public ZServer
{
private:

protected:
	virtual bool	InitInfo();
	virtual void	UpdateForDebug(float fDelta) {}
	virtual bool CreateNetwork();
public:
	MockServer();
	virtual ~MockServer();
};
