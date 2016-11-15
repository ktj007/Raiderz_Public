#pragma once

#include "MTcpServer.h"

class XNet : public minet::MTcpServer
{
public:
	XNet();
	virtual ~XNet();

	static XNet& Instance() {
		return singleton;
	}

private:
	static XNet singleton;
};

__inline XNet& Net()
{
	return XNet::Instance();
}