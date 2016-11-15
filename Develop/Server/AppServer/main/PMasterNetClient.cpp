#include "stdafx.h"
#include "PMasterNetClient.h"
#include "PCommandTable.h"
#include "PCmdHandler_Master_Global.h"
#include "PClientConnectionKeeper.h"
#include "STypes.h"
#include "PGlobal.h"
#include "PSystem.h"
#include "PConfig.h"
#include "MiNetHelper.h"
#include "STransData_M2G.h"

PMasterNetClient::PMasterNetClient(const MNetClientDesc& desc)
: SNetClient(desc)
{
	m_pGlobalCmdHandler = new PCmdHandler_Master_Global(this);
}

PMasterNetClient::~PMasterNetClient()
{
	SAFE_DELETE(m_pGlobalCmdHandler);
}

void PMasterNetClient::OnReplyConnect( MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp )
{
	SNetClient::OnReplyConnect(uidHost, uidAlloc, nTimeStamp);

	// AppServerMode
	string strIP = MiNetHelper::GetHostIPString();

	TD_APP_SERVER_INFO appServerInfo;
	appServerInfo.nIP = MiNetHelper::ConvertIPAddress(strIP.c_str());
	appServerInfo.nPort = PConfig::m_nPort;
	appServerInfo.nAppServerMode = APP_SERVER_CHAT;
	vector<TD_APP_SERVER_INFO> vecAppServerInfo(1, appServerInfo);	

	// 서버정보 전달
	MCommand* pNewCommand = NewCommand(MAC_COMM_REQUEST_LOGIN, GetServerUID());
	pNewCommand->AddParameter(new MCommandParameterWideString(L"AppServer"));
	pNewCommand->AddParameter(new MCommandParameterInt(SERVER_APPLICATION));
	pNewCommand->AddParameter(new MCommandParameterInt(PConfig::m_nMyWorldID));
	pNewCommand->AddParameter(new MCommandParameterInt(PConfig::m_nMyServerID));
	pNewCommand->AddParameter(new MCommandParameterInt(SH_COMMAND_VERSION));
	pNewCommand->AddParameter(new MCommandParameterBlob(vecAppServerInfo));
	Post(pNewCommand);
}
