#include "stdafx.h"
#include "MMockNetServer.h"
#include "MMockLink.h"

namespace minet {

MMockLink* MMockNetServer::AddMockLink( MUID uidLink )
{
	MMockLink* pNewLink = NewMockLink(uidLink);
	m_LinkMap.Add(pNewLink);
	return pNewLink;
}

void MMockNetServer::DelMockLink( MUID uidLink )
{
	m_LinkMap.Delete(uidLink);
}

void MMockNetServer::ClearMockLinks()
{
	m_LinkMap.Clear();
}

bool MMockNetServer::SendCommandToLink( MUID uidReceiver, MCommand* pCommand, MCommandMsg* pMsg, int nPacketSize )
{
	MLink* pLink = m_LinkMap.GetLink(uidReceiver);
	if (NULL == pLink) return false;
	
	MMockLink* pMockLink = dynamic_cast<MMockLink*>(pLink);
	pMockLink->OnSendCommand(*pCommand);

	pLink->Drop();
	free(pMsg);
	
	return true;
}

bool MMockNetServer::Create(int nPort, const bool bReuse/*=false*/, MUID uidSeed/*=MUID::ZERO*/, int nSocketPoolSize/*=2000*/, int nSendPendingLimitCount /*= INT_MAX*/, const char* szMyNetworkCardIP/*=""*/)
{
	MUID new_server_uid = NewUID();
	SetUID(new_server_uid);

	return true;
}

void MMockNetServer::Destroy()
{
	MNetServer::Destroy();
}
} // namespace minet