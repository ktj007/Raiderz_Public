#ifndef MINET_COMMON_H
#define MINET_COMMON_H

#include "MTrafficLog.h"
#include "MTypes.h"

#pragma comment(lib, "ws2_32.lib")


namespace minet {

class MSendRecvTrafficLog
{
private:
	unsigned long			m_nTotalSendBytes;
	unsigned long			m_nTotalRecvBytes;
	MTrafficLog				m_SendTrafficLog;
	MTrafficLog				m_RecvTrafficLog;
public:
	MSendRecvTrafficLog()
	{
		m_nTotalSendBytes = m_nTotalRecvBytes = 0;
	}
	~MSendRecvTrafficLog() {}
	void OnSend(int nSendSize)
	{
		m_nTotalSendBytes += nSendSize;
		m_SendTrafficLog.Record(m_nTotalSendBytes);
	}
	void OnRecv(int nRecvSize)
	{
		m_nTotalRecvBytes += nRecvSize;
		m_RecvTrafficLog.Record(m_nTotalRecvBytes);
	}

	int GetSendTraffic()	{ return m_SendTrafficLog.GetTrafficSpeed(); }
	int GetRecvTraffic()	{ return m_RecvTrafficLog.GetTrafficSpeed(); }

	uint32 GetTotalSendBytes() { return m_nTotalSendBytes; }
	uint32 GetTotalRecvBytes() { return m_nTotalRecvBytes; }
};


/// 커맨드 처리 결과값
enum MCommandResult
{
	CR_FALSE		= 0,			///< 처리안됨
	CR_TRUE			= 1,			///< 처리됨
	CR_ERROR		= 2,			///< 에러
	CR_MOVED		= 3,			///< 다른 큐로 이동됨
};

void SplitIAddress(char* szIP, int nIPBufferSize, int* pPort, const char* szAddress);

class MPingData
{
private:
	uint32	m_nPing;
	uint32	m_nRecevedTime;
	uint32	m_nSentTime;
	uint32	m_nSendCount;
	uint32	m_nRecvCount;
public:
	MPingData() : m_nSentTime(0), m_nRecevedTime(0), m_nPing(0), m_nSendCount(0), m_nRecvCount(0)	{}

	void Sent(uint32 nNowTime)
	{
		m_nSentTime = nNowTime;
		++m_nSendCount;
	}
	void Received(uint32 nNowTime, uint32 nTimeStamp)
	{
		++m_nRecvCount;
		m_nRecevedTime = nNowTime;

		if (m_nSentTime == nTimeStamp)
		{
			m_nPing = nNowTime - nTimeStamp;
		}
	}
	uint32 GetPing() const { return m_nPing; }
	uint32 GetSendCount() const { return m_nSendCount; }
	uint32 GetRecvCount() const { return m_nRecvCount; }
	bool CheckHeartBeat(uint32 nNowTime, uint32 nCheckTime)
	{
		if (nNowTime - m_nSentTime >= nCheckTime)
		{
			if (m_nSentTime == 0) 
			{
				m_nSentTime = nNowTime;
				return false;
			}

			return true;
		}
		return false;
	}
	bool CheckTimeOut(uint32 nNowTime, uint32 nCheckTime)
	{
		if (m_nSentTime != 0 && m_nSentTime > m_nRecevedTime && nNowTime - m_nSentTime > nCheckTime)
		{
			return true;
		}
		return false;
	}
};



} // namespace minet



#endif