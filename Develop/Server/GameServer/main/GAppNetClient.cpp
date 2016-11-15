#include "stdafx.h"
#include "GAppNetClient.h"
#include "GCommandTable.h"
#include "GCmdHandlerGroup.h"
#include "STypes.h"
#include "GConfig.h"

GAppNetClient::GAppNetClient(const MNetClientDesc& desc) : GNetClient(desc)
{
	m_pCmdHandlerGroup = new GCmdHandlerGroup_AppNetClient(this);
}

GAppNetClient::~GAppNetClient()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}

void GAppNetClient::OnReplyConnect( MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp )
{
	GNetClient::OnReplyConnect(uidHost, uidAlloc, nTimeStamp);

	MCommand* pNewCommand = NewCommand(MPC_COMM_REQUEST_LOGIN, GetServerUID());
	pNewCommand->AddParameter(new MCommandParameterWideString(L"GameServer"));	// id는 테스트용
	pNewCommand->AddParameter(new MCommandParameterInt(SERVER_GAME));
	pNewCommand->AddParameter(new MCommandParameterInt(GConfig::m_nMyWorldID));
	pNewCommand->AddParameter(new MCommandParameterInt(GConfig::m_nMyServerID));
	pNewCommand->AddParameter(new MCommandParameterInt(SH_COMMAND_VERSION));
	Post(pNewCommand);
}
