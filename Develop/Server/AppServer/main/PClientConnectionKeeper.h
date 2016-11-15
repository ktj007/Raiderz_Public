#pragma once

#include "SClientConnectionKeeper.h"

class PClientConnectionKeeper: public SClientConnectionKeeper
{
public:
	PClientConnectionKeeper(MNetClient* pNetClient, wstring strSeverIP, int nPort, SSystem* pSystem, int nTryTick=0, wstring strNetworkCardIP=L"")
		: SClientConnectionKeeper(pNetClient, strSeverIP, nPort, pSystem, nTryTick, strNetworkCardIP){}
	virtual ~PClientConnectionKeeper() {}
};
