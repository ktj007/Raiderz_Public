#include "stdafx.h"
#include "LMasterNetClient.h"
#include "LCommandTable.h"
#include "LCmdHandlerGroup.h"
#include "STypes.h"
#include "LConfig.h"

LMasterNetClient::LMasterNetClient(const MNetClientDesc& desc) : SNetClient(desc)
{
	m_pHandlerGroup = new LCmdHandlerGroup_MasterNetClient(this);
}

LMasterNetClient::~LMasterNetClient()
{
	SAFE_DELETE(m_pHandlerGroup);
}

void LMasterNetClient::OnReplyConnect( MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp )
{
	SNetClient::OnReplyConnect(uidHost, uidAlloc, nTimeStamp);

	MCommand* pNewCommand = NewCommand(MLC_COMM_REQUEST_LOGIN, GetServerUID());
	pNewCommand->AddParameter(new MCommandParameterWideString(LConfig::m_strServerName.c_str()));
	pNewCommand->AddParameter(new MCommandParameterInt(SERVER_LOGIN));
	pNewCommand->AddParameter(new MCommandParameterInt(LConfig::m_nWorldID));
	pNewCommand->AddParameter(new MCommandParameterInt(LConfig::m_nServerID));
	pNewCommand->AddParameter(new MCommandParameterInt(SH_COMMAND_VERSION));
	Post(pNewCommand);
}

