#include "stdafx.h"
#include "MReceiver.h"
#include "MServerTcpSocket.h"

namespace minet {

MReceiver::MReceiver() : m_pListener(NULL)
{

}

void MReceiver::OnRecv(MServerTcpSocket& tcpsocket, MUID uidLink, BYTE* pData, DWORD dwPacketLen)
{
	if (m_pListener) m_pListener->OnRecv(tcpsocket, uidLink, pData, dwPacketLen);
}

void MReceiver::ProcEvent( MAct* act, DWORD bytes_transferred )
{
	MTcpAct* pTcpAct = static_cast<MTcpAct*>(act);

	assert(act);
	assert(pTcpAct->m_pTcpSocket);

	MServerTcpSocket& tcpsocket = *(pTcpAct->m_pTcpSocket);

	// passive 연결끊김
	if( bytes_transferred == 0 )
	{
		if (m_pListener) m_pListener->OnDisconnect(pTcpAct->m_uidLink);
	}
	else
	{
		OnRecv(tcpsocket, pTcpAct->m_uidLink, (BYTE*)(&tcpsocket.m_RecvBuffer[0]), bytes_transferred);

		// Async Recv 포스트
		tcpsocket.Recv();
	}
}

void MReceiver::ProcError( MAct* act, DWORD bytes_transferred, DWORD error )
{
	MTcpAct* pTcpAct = static_cast<MTcpAct*>(act);

	assert(pTcpAct);
	assert(pTcpAct->m_pTcpSocket);

	MServerTcpSocket* pTcpSocket = pTcpAct->m_pTcpSocket;

	// 로그가 너무 많이 남아서 ERROR_NETNAME_DELETED는 로그 안남김 - birdkr
	if (error != ERROR_NETNAME_DELETED)
	{
		minet_logd("MReceiver ProcError socket(%d) err(%d)\n", pTcpSocket->GetSocket(), error );
	}

	// 이미 닫혀진 소켓이 아닌한 Recv할 때 오류가 났으면 끊는다.
	if (bytes_transferred == 0)
	{
		if (m_pListener) m_pListener->OnHardDisconnect(pTcpAct->m_uidLink);
	}

}

void MReceiver::Init( MProactor* proactor )
{
	m_pProactor = proactor;
}


} // namespace minet