#include "stdafx.h"
#include "MTrafficMonitor.h"

namespace minet {

MTrafficMonitor::MTrafficMonitor()
{
	Reset();
}

void MTrafficMonitor::Reset()
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	m_nTotalRecvSize = 0;
	m_nTotalSendSize = 0;
	m_nRecvSizePerSec = 0;
	m_nSendSizePerSec = 0;
	m_nLastTrafficCheckTime = 0;
}

void MTrafficMonitor::RecordRecv( uint32 nPacketSIze )
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	uint32 nNowTime = timeGetTime();

	if (nNowTime - m_nLastTrafficCheckTime >= 1000)
	{
		m_nLastTrafficCheckTime = nNowTime;

		m_nRecvSizePerSec = 0;
		m_nSendSizePerSec = 0;
	}

	m_nTotalRecvSize += nPacketSIze;
	m_nRecvSizePerSec += nPacketSIze;
}

void MTrafficMonitor::RecordSend( uint32 nPacketSIze )
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	uint32 nNowTime = timeGetTime();

	if (nNowTime - m_nLastTrafficCheckTime >= 1000)
	{
		m_nLastTrafficCheckTime = nNowTime;

		m_nRecvSizePerSec = 0;
		m_nSendSizePerSec = 0;
	}

	m_nTotalSendSize += nPacketSIze;
	m_nSendSizePerSec += nPacketSIze;
}

void MTrafficMonitor::GetTotalTraffic( int64& noutRecvSize, int64& noutSendSize )
{
	noutRecvSize = m_nTotalRecvSize;
	noutSendSize = m_nTotalSendSize;
}

void MTrafficMonitor::GetSecondTraffic( uint32& noutRecvSize, uint32& noutSendSize )
{
	uint32 nNowTime = timeGetTime();

	if (nNowTime - m_nLastTrafficCheckTime >= 1000) //< 마지막 갱신 시간에서 1초가 경과했다면 0을 돌려줌
	{
		noutRecvSize = 0;
		noutSendSize = 0;
	}
	else
	{
		noutRecvSize = m_nRecvSizePerSec;
		noutSendSize = m_nSendSizePerSec;
	}
}

} // namespace minet