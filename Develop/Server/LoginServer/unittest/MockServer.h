#pragma once

#include "LServer.h"

class MockServer : public LServer
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
