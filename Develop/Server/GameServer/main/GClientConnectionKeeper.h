#pragma once

#include "SClientConnectionKeeper.h"

class GClientConnectionKeeper : public SClientConnectionKeeper, public MTestMemPool<GClientConnectionKeeper>
{
public:
	enum CONNECTION_TYPE
	{
		MASTER_SERVER = 0,
		CHAT_SERVER
	};

public:
	GClientConnectionKeeper(MNetClient* pNetClient, wstring strSeverIP, int nPort, SSystem* pSystem, int nTryTick=0, wstring strNetworkCardIP=L"");
	virtual ~GClientConnectionKeeper();

	virtual void OnDisconnect() override;

	void SetConnectionType(CONNECTION_TYPE connectionType);

private:
	CONNECTION_TYPE	m_connectionType;
};
