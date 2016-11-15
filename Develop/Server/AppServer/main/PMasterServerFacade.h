#pragma once

#include "PMasterNetClient.h"

class PClientConnectionKeeper;

class PMasterServerFacade
{
public:
	PMasterServerFacade(MNetClient* pMasterNetClient);
	virtual ~PMasterServerFacade();

	// Network
	bool CreateNetwork(const wchar_t* szMasterServerIP, int nMasterServerPort);
	void DestroyNetwork();
	void RunNetwork();
	void OnConnect();
	void OnDisconnect();
	void OnError(int nErrorCode);
	
	MNetClient* GetNetClient() { return m_pMasterNetClient; }

private:
	MNetClient*					m_pMasterNetClient;
	PClientConnectionKeeper*	m_pConnectionKeeper;

	bool ConnectToMasterServer(const wchar_t* szMasterServerIP, int nPort, const wchar_t* szNetworkCardIP=L"");

};
