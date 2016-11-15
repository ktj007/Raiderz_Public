#pragma once

#include "MiNetLib.h"
#include "MiNetPrerequisites.h"

namespace minet {

class MINET_API MSocket
{
protected:
	SOCKET CreateSocket( int addressFamily, int socketType, int protocol );
public:
	SOCKET				m_hSocket;
	SOCKADDR_IN			m_Addr;
	SOCKADDR_STORAGE	m_remoteAddr;
	int					m_remoteAddrLen;

	MSocket();
	virtual ~MSocket() {}

	void CloseSocket();

	SOCKET GetSocket() const { return m_hSocket; }
};

} // namespace minet {