#pragma once

#include "PServer.h"

class MockServer : public PServer
{
private:

protected:
	virtual bool	Create();
	virtual void	Destroy();

	virtual bool	InitInfo();
	virtual void	UpdateForDebug(float fDelta) {}
	virtual bool	CreateNetwork();
public:
	MockServer();
	virtual ~MockServer();
};
