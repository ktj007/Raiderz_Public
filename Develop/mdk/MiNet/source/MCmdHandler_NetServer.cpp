#include "stdafx.h"
#include "MCmdHandler_NetServer.h"
#include "MCommand.h"
#include "MCommandTable.h"
#include "MNetServer.h"
#include "MErrorTable.h"

namespace minet {

MCmdHandler_NetServer::MCmdHandler_NetServer(MCommandCommunicator* pCC)
	: MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOCAL_NET_ONACCEPT,			OnLocal_NetOnAccept);
	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT,		OnLocal_NetOnDisconnect);
	SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT_HARD,	OnLocal_NetOnHardDisconnect);
}

MCmdHandler_NetServer::~MCmdHandler_NetServer()
{

}

MCommandResult MCmdHandler_NetServer::OnLocal_NetOnAccept(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidAlloc;
	pCommand->GetParameter(&uidAlloc, 0, MPT_UID);

	MUID uidHost = NetServer(pHandler)->GetUID();

	unsigned long int nTimeStamp = 0;	// TODO: 나중에 설정해줘야 함.

	MCommand* pNewCommand = NetServer(pHandler)->NewCommand(MC_NET_REPLY_CONNECT, uidAlloc);
	pNewCommand->AddParameter(new MCommandParameterUID(uidHost));
	pNewCommand->AddParameter(new MCommandParameterUID(uidAlloc));
	pNewCommand->AddParameter(new MCommandParameterUInt(nTimeStamp));
	NetServer(pHandler)->Post(pNewCommand);

	return CR_TRUE;
}

MCommandResult MCmdHandler_NetServer::OnLocal_NetOnDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidLink;
	pCommand->GetParameter(&uidLink, 0, MPT_UID);

	NetServer(pHandler)->Disconnect(uidLink);

	return CR_TRUE;
}

MCommandResult MCmdHandler_NetServer::OnLocal_NetOnHardDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidLink;
	pCommand->GetParameter(&uidLink, 0, MPT_UID);

	NetServer(pHandler)->HardDisconnect(uidLink);

	return CR_TRUE;
}

MNetServer* MCmdHandler_NetServer::NetServer( MCommandHandler* pHandler )
{
	return (static_cast<MNetServer*>(pHandler->GetCommandCommunicator()));
}

} // namespace minet