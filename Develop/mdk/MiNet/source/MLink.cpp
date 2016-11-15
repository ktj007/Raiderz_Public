#include "stdafx.h"
#include "MLink.h"
#include "MCommandStream.h"

namespace minet {


//////////////////////////////////////////////////////////////////////////

bool MLink::Disconnect()
{
	return m_TCPSocket.Disconnect();
}

void MLink::Send( BYTE* buf, int buflen )
{
	m_TCPSocket.Send(buf, buflen);
}

MLink::MLink( MUID uidServer, MUID uidNew, MServerTcpSocket& tcp_socket, minet::MCommandManager* pCM) 
: m_UID(uidNew), m_TCPSocket(tcp_socket), m_bAllowed(false)
{
	m_pCommandStream = new MCommandStream(pCM);
	InitCrypt(*m_pCommandStream, uidServer, uidNew);

	SetAllowed(true);

	SetRecvTime(timeGetTime());
}

MLink::~MLink()
{
	SAFE_DELETE(m_pCommandStream);
}

void MLink::InitCrypt( MCommandStream& commandStream, MUID uidServer, MUID uidNew )
{
	const int CRYPT_KEY_LEN = 32;
	char szKey[CRYPT_KEY_LEN] = {0, };
	MMakeCryptKey((unsigned char*)szKey, CRYPT_KEY_LEN, uidServer, uidNew, 0);
	m_PacketCrypter.InitKey(szKey, CRYPT_KEY_LEN);

	commandStream.InitCrypt(&m_PacketCrypter, true);	// 중복 패킷 체크도 활성화
}

void MLink::ReAllocUID( MUID uidAlloc )
{
	m_UID = uidAlloc;
	m_TCPSocket.SetLink(m_UID);
}

void MLink::HardDisconnect()
{
	m_TCPSocket.HardDisconnect(m_UID);
}



} // namespace minet {