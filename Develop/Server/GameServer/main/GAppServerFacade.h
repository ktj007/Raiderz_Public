#pragma once

#include "STransData_M2G.h"
#include "GAppNetClient.h"
#include "MUID.h"
#include "STypes.h"

class GClientConnectionKeeper;
class GProxyChatChannelManager;
class GServerChatChannelRouter;

class GAppServerFacade : public MTestMemPool<GAppServerFacade>
{
public:
	GAppServerFacade();
	GAppServerFacade(MNetClient* pNetClient);
	virtual ~GAppServerFacade();
	
	void Init(void);
	bool CreateNetwork(const wchar_t* szMasterServerIP, int nAppServerPort);
	void DestroyNetwork();
	void SetChatChannelRouter(GServerChatChannelRouter* pChatChannelRouter);

	void RunNetwork(float fDelta);

	void OnConnect();
	void OnDisconnect();
	void OnError(int nErrorCode);

	MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );
	bool Route(MCommand* pCmd);

	void SetNetClient(minet::MNetClient* pNetClient);

	minet::MNetClient* GetNetClient()					{ return m_pClient; }
	GProxyChatChannelManager* GetChatChannelManager()	{ return m_pChatChannelManager; }
	GServerChatChannelRouter* GetChatChannelRouter()	{ _ASSERT(m_pChatChannelRouter != NULL);	return m_pChatChannelRouter; }
private:
	bool ConnectToAppServer(const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP = L"");

private:
	minet::MNetClient*			m_pClient;
	GClientConnectionKeeper*	m_pConnectionKeeper;
	MRegulator					m_appCreateRegulator;
	GProxyChatChannelManager*	m_pChatChannelManager;

	GServerChatChannelRouter*	m_pChatChannelRouter;
};
