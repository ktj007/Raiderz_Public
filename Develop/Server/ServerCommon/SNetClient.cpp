#include "stdafx.h"
#include "SNetClient.h"

bool SPostCommand(MCommand* pCmd)
{
//	return global.net->Post(pCmd); 

	return false;
}

MCommand* SNewCmd(int nCmdID)
{
	//MUID uidTarget= global.net->GetServerUID();
	//return global.net->NewCommand(nCmdID, uidTarget);
	return NULL;
}

SNetClient::SNetClient(const MNetClientDesc& desc) : MNetClient(desc)
{
//	m_pCmdHandler = new XCmdHandler_Net(this);
}

SNetClient::~SNetClient()
{

}

void SNetClient::OnReplyConnect( MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp )
{
	MNetClient::OnReplyConnect(uidHost, uidAlloc, nTimeStamp);
}

MCommandResult SNetClient::OnCommand(MCommand* pCommand)
{
	MCommandResult ret = MNetClient::OnCommand(pCommand);
	if (ret != CR_FALSE) return ret;

	return CR_FALSE;
}

void SNetClient::OnPrepareCommand( MCommand* pCommand )
{
//	LogCommand(pCommand);
}

bool SNetClient::Post( MCommand* pCommand )
{
	return MNetClient::Post(pCommand);
}

void SNetClient::SendCommand( MCommand* pCommand )
{
	MNetClient::SendCommand(pCommand);

	//LogCommand(pCommand);
}

void SNetClient::LogCommand( MCommand* pCommand )
{

}