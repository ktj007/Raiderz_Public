#include "stdafx.h"
#include "MockLink.h"
#include "GCommandCenter.h"
#include "MockServer.h"
#include "MockNetServer.h"
#include "MockPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GSTLUtil.h"

minet::MCommandResult MockLink::OnRecvCommand( minet::MCommand& command )
{
	if (gsys.pServer == NULL || !gsys.pServer->IsCreated())
	{
		return CR_ERROR;
	}

	return MMockLink::OnRecvCommand(command);
}

MockLink::MockLink( MUID uidNew, minet::MNetServer* pNetServer, minet::MServerTcpSocket& tcp_socket, minet::MCommandManager* pCM ) 
: MMockLink(uidNew, pNetServer, tcp_socket, pCM)
, m_pMockPlayerObject(NULL)
{
	
}

MockLink::~MockLink()
{
	if (m_pMockPlayerObject)
	{
		gmgr.pPlayerObjectManager->DeletePlayer(m_UID);
	}
}

void MockLink::AttachMockPlayerObject( MockPlayerObject* pMockPlayerObject )
{
	m_pMockPlayerObject = pMockPlayerObject;
}
