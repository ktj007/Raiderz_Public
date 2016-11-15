#pragma once

#include "MThread2.h"

namespace minet {

class MTrafficMonitor
{
private:
	uint32					m_nRecvSizePerSec;
	uint32					m_nSendSizePerSec;
	int64					m_nTotalRecvSize;
	int64					m_nTotalSendSize;
	uint32					m_nLastTrafficCheckTime;
	cml2::MCriticalSection	m_csLock;

public:
	MTrafficMonitor();
	~MTrafficMonitor() {}
	
	void	Reset();

	void	RecordRecv(uint32 nPacketSize);
	void	RecordSend(uint32 nPacketSize);

	void	GetTotalTraffic(int64& noutRecvSize, int64& noutSendSize);
	void	GetSecondTraffic(uint32& noutRecvSize, uint32& noutSendSize);
};


}