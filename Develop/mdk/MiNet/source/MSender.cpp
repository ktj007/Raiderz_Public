#include "stdafx.h"
#include "MSender.h"
#include "MTcpSocket.h"
#include "MServerTcpSocket.h"

namespace minet {

MSender::MSender() : m_pListener(NULL)
{

}

void MSender::OnSend(MTcpAct* const pTcpAct,DWORD bytes_transferred)
{
	assert(NULL!=pTcpAct);
	if (m_pListener) m_pListener->OnSend(pTcpAct->m_uidLink, bytes_transferred);
}

void MSender::ProcEvent( MAct* act, DWORD bytes_transferred )
{
	MTcpAct* pTcpAct = static_cast<MTcpAct*>(act);

	assert(pTcpAct);
	assert(pTcpAct->m_pTcpSocket);

	OnSend(pTcpAct, bytes_transferred);
}

void MSender::ProcError( MAct* act, DWORD bytes_transferred, DWORD error )
{
	MTcpAct* pTcpAct = static_cast<MTcpAct*>(act);

	assert(pTcpAct);
	assert(pTcpAct->m_pTcpSocket);

	// 로그가 너무 많이 남아서 ERROR_NETNAME_DELETED는 로그 안남김 - birdkr test
	if (error != ERROR_NETNAME_DELETED)
	{
		MServerTcpSocket& tcpsocket = *(pTcpAct->m_pTcpSocket);

		minet_logd("MSender ProcError socket(%d) err(%d)\n", tcpsocket.GetSocket(), error );
	}
}

void MSender::Init( MProactor* proactor )
{
	m_pProactor = proactor;
}


} // namespace minet {