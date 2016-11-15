#pragma once


#include "MNetClient.h"
using namespace minet;

#include "ServerCommonLib.h"
#include "MTstring.h"

class SSystem;

class SCOMMON_API SClientConnectionKeeper
{
protected:
	MNetClient*				m_pNetClient;
	SSystem*				m_pSystem;

	wstring					m_strNetworkCardIP;
	wstring					m_strIP;
	int						m_nPort;
	bool					m_bConnected;
	bool					m_bActive;
	uint32					m_nTryTick;
	uint32					m_nLastTime;
	bool					m_bTryReserved;
	void TryToConnect();
	bool Post_Connect( const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP = L"");
public:
	SClientConnectionKeeper(MNetClient* pNetClient, wstring strSeverIP, int nPort, SSystem* pSystem, int nTryTick=0, wstring strNetworkCardIP = L"");
	virtual ~SClientConnectionKeeper() {}

	void Run();
	void OnConnect();	
	void OnNetError(int nErrorCode);

	virtual void OnDisconnect();

	void SetActive(bool bActive) { m_bActive = bActive; }
	bool IsActive() { return m_bActive; }
};