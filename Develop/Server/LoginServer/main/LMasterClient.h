#pragma once

#include "MUID.h"
#include "STransData.h"
#include "MNetClient.h"

class LMasterNetClient;
class LClientConnectionKeeper;
class LCmdHandler_Master_Net;

using namespace minet;

/// 마스터 서버에 접속하기 위한 클라이언트 퍼사드
class LMasterClient
{
	friend class LCmdHandler_Master_Net;
private:
	MNetClient*					m_pNetClient;
	LClientConnectionKeeper*	m_pConnectionKeeper;

	void OnConnect();
	void OnDisconnect();
	void OnError(int nErrorCode);
public:
	LMasterClient();
	~LMasterClient();

	bool Create();
	bool Create(MNetClient* pNetClient);
	void Destroy();
	void Run();

	MNetClient* GetNetClient() { return m_pNetClient; }

	MCommand* MakeNewSendCommand( int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );

	// post
	bool Post_ConnectToMasterServer( const wchar_t* szIP, int nPort, const wchar_t *szNetworkCardIP = L"" );
	bool Post_Disconnect(bool bConnectionKeeperActive = false);
	bool Post_PlayerLoginToGameServer(MUID uidPlayer, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo);
};