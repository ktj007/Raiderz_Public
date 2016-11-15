#include "stdafx.h"
#include "GClientConnectionKeeper.h"

GClientConnectionKeeper::GClientConnectionKeeper(MNetClient* pNetClient, wstring strSeverIP, int nPort, SSystem* pSystem, int nTryTick, wstring strNetworkCardIP)
: SClientConnectionKeeper(pNetClient, strSeverIP, nPort, pSystem, nTryTick, strNetworkCardIP)
, m_connectionType(MASTER_SERVER)
{
	// do nothing
}

GClientConnectionKeeper::~GClientConnectionKeeper()
{
	// do nothing
}

void GClientConnectionKeeper::OnDisconnect()
{
	__super::OnDisconnect();

	// 마스터서버가 아니면, 재접속을 시도한다. (마스터서버와 끊어지면 종료절차를 진행하기 때문에 재접속할 필요가 없다)
	if (m_connectionType != MASTER_SERVER)
	{
		TryToConnect();
	}
}

void GClientConnectionKeeper::SetConnectionType(CONNECTION_TYPE connectionType)
{
	m_connectionType = connectionType;
}
