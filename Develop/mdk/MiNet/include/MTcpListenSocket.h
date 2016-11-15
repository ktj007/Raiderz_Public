#pragma once

#include "MSocket.h"
#include "MProactor.h"

namespace minet {

class MTcpListenSocket : public MSocket
{
public:
	MTcpListenSocket();
	bool Init(int addressFamily,WORD port, bool reuse=false, const char* szMyNetworkCardIP="");
	bool Listen(MProactor* pProactor);
};









} // namespace minet {