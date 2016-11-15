#include "stdafx.h"
#include "MAct.h"
#include "MActor.h"
#include "MServerTcpSocket.h"

namespace minet {

void MTcpAct::Complete( DWORD bytes_transferred )
{
	m_pActor->ProcEvent(this, bytes_transferred);
}

void MTcpAct::Error( DWORD bytes_transferred, DWORD error )
{
	m_pActor->ProcError(this, bytes_transferred, error);
}



MTcpActSend::~MTcpActSend()
{
	if (m_pData)
	{
		free(m_pData);
		m_pData = NULL;
	}

	if (m_pTcpSocket)
	{
		m_pTcpSocket->DecreaseSendPendingCounter();
	}
}

bool MTcpActSend::Init(MActor* pActor, MServerTcpSocket* pTcpSocket, MUID uidLink, char* pData, int nDataLen)
{
	m_pActor = pActor;
	m_pTcpSocket = pTcpSocket;
	m_uidLink = uidLink;

	m_pData = pData;
	m_nTotalBytes = nDataLen;

	if (m_pTcpSocket == NULL)
	{
		return false;			
	}

	bool isAllowedPending = m_pTcpSocket->IncreaseSendPendingCounter();
	return isAllowedPending;
}

} // namespace minet {
