#include "stdafx.h"
#include "GMasterNetClient.h"
#include "GCommandTable.h"
#include "GCmdHandlerGroup.h"
#include "STypes.h"
#include "GConfig.h"

GMasterNetClient::GMasterNetClient(const MNetClientDesc& desc) : GNetClient(desc)
{
	m_pCmdHandlerGroup = new GCmdHandlerGroup_MasterNetClient(this);
}

GMasterNetClient::~GMasterNetClient()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}

void GMasterNetClient::OnReplyConnect( MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp )
{
	GNetClient::OnReplyConnect(uidHost, uidAlloc, nTimeStamp);

	MCommand* pNewCommand = NewCommand(MMC_COMM_REQUEST_LOGIN_G2M, GetServerUID());
	pNewCommand->AddParameter(new MCommandParameterWideString(L"GameServer"));	// id는 테스트용
	pNewCommand->AddParameter(new MCommandParameterInt(SERVER_GAME));
	pNewCommand->AddParameter(new MCommandParameterInt(GConfig::m_nMyWorldID));
	pNewCommand->AddParameter(new MCommandParameterInt(GConfig::m_nMyServerID));
	pNewCommand->AddParameter(new MCommandParameterInt(SH_COMMAND_VERSION));
	Post(pNewCommand);
}