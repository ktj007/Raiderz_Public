#pragma once

#include "SClientConnectionKeeper.h"

class LClientConnectionKeeper : public SClientConnectionKeeper
{
protected:
public:
	LClientConnectionKeeper(MNetClient* pNetClient, wstring strRemoteIP, int nPort, SSystem* pSystem, int nTryTick=0, wstring strNetworkCardIP=L"")
	: SClientConnectionKeeper(pNetClient, strRemoteIP, nPort, pSystem, nTryTick, strNetworkCardIP) {}
	virtual ~LClientConnectionKeeper() {}
};