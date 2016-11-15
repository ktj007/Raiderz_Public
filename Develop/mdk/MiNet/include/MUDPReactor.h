#pragma once

#include "MUDPSocket.h"

namespace minet {

class IUDPCommandHandler;

class MUDPReactor
{
public:
	MUDPReactor();
	virtual ~MUDPReactor();

	virtual bool Create(int nPort);
	virtual void Destory(void);
	
	bool Send(int nIP, int nPort, char* pPacket, int nPacketSize);
	void SetCommandHandler(IUDPCommandHandler* pCommandHandler);

private:
	bool OnRecv(DWORD dwIP, WORD wPort, char* pPacket, DWORD dwSize);
	static bool SocketRecvEvent(void* pCallbackContext, DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);

private:	
	MUDPSocket m_UDPSocket;
	IUDPCommandHandler* m_pCommandHandler;
};

}
