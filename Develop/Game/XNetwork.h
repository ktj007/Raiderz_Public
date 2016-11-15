#pragma once

#include "XNetClient.h"

class XConnectionManager;
class XWorldLocatorClient;

/// 네트워크 정보
struct XNetworkDesc
{
	wstring		strServerIP;		///< 접속한 서버 ID
	int			nServerPort;		///< 접속한 서버 Port
};

/// 네트워크 관련 퍼사드 클래스
class XNetwork
{
protected:
	minet::MNetClient*		m_pNetClient;
	XWorldLocatorClient*	m_pWorldLocatorClient;	
	XConnectionManager*		m_pConnectionManager;
	XNetworkDesc			m_Desc;

	virtual minet::MNetClient* NewClient(minet::MNetClientDesc desc);
	void Destroy();
	void PostConnect(const wchar_t* szIP, const int nPort);

public:
	XNetwork();
	virtual ~XNetwork();
	void Create();

	void Update(float fDelta);
	bool Post(MCommand* pCommand);
	MCommand* NewCommand(int nCmdID, MUID TargetUID);

	void PostConnectToLocatorServer(const vector<wstring>& vecIP);
	void PostConnectToGameServer(const wchar_t* szIP, int nPort);
	void PostConnectToLoginServer(const wchar_t* szIP);

	void PostDisconnect();	


	void StartCommandProfile();
	void FinishCommandProfile();


	MUID GetServerUID(void) { return m_pNetClient->GetServerUID(); }
	uint32 GetPing() { return m_pNetClient->GetPing(); }
	minet::MNetClient* GetClient() { return m_pNetClient; }
	virtual XNetClient* GetNetClient() { return static_cast<XNetClient*>(m_pNetClient); }
	XConnectionManager* GetConnectionManager() { return m_pConnectionManager; }
	const XNetworkDesc& GetDesc() { return m_Desc; }
	XWorldLocatorClient* GetWorldLocatorClient()	{ return m_pWorldLocatorClient;	}
};



class MockNetwork : public XNetwork
{
protected:
	virtual minet::MNetClient* NewClient(minet::MNetClientDesc desc);
public:
	MockNetwork();
	virtual ~MockNetwork() {}
	virtual XNetClient* GetNetClient() { return NULL; }
};