#include "stdafx.h"
#include "MockLink.h"

minet::MCommandResult MockLink::OnRecvCommand( minet::MCommand& command )
{
	return MMockLink::OnRecvCommand(command);
}

MockLink::MockLink( MUID uidNew, minet::MNetServer* pNetServer, minet::MServerTcpSocket& tcp_socket, minet::MCommandManager* pCM ) 
: MMockLink(uidNew, pNetServer, tcp_socket, pCM)
{

}

MockLink::~MockLink()
{
}
