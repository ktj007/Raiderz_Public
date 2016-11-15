#pragma once

#include "GServer.h"

class NetworkMockServer : public GServer
{
public:
	NetworkMockServer();
private:
	virtual bool CreateNetwork();
};
