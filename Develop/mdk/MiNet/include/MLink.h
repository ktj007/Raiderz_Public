#pragma once

#include "MiNetLib.h"
#include "MiNetPrerequisites.h"
#include "MServerTCPSocket.h"
#include "MCommandStream.h"
#include "MPacketCrypter.h"

class minet::MCommandManager;

namespace minet {

struct MKeepingAliveData
{
	uint32		nLastRecvTime;

	MKeepingAliveData() : nLastRecvTime(0) {}
	bool CheckAlive(uint32 nNowTime, uint32 nAllowedTime)
	{
		if (nLastRecvTime > nNowTime)
		{
			return true;
		}
		if (nLastRecvTime == 0 || nNowTime - nLastRecvTime < nAllowedTime)
		{
			return true;
		}
		return false;
	}
	void SetRecvTime(uint32 nTime)
	{
		nLastRecvTime = nTime;
	}
};

/// Link용 레퍼런스 카운터
/// - 레퍼런스 카운터 기능이 필요한 클래스는 이 클래스를 상속받으면 된다.
template <class T>
class MINET_API MRefObject
{
private:
	static MCriticalSectionSpinLock		m_csLock;
	mutable long m_nReferenceCounter;
public:
	MRefObject() : m_nReferenceCounter(1)
	{

	}

	virtual ~MRefObject()
	{

	}

	void AddRef() const
	{
		MCriticalSectionSpinLock::Guard guard(m_csLock);

		m_nReferenceCounter++;
	}

	bool Drop() const
	{
		m_csLock.Lock();

		assert(m_nReferenceCounter > 0);

		m_nReferenceCounter--;

		if (0 == m_nReferenceCounter)
		{
			delete this;

			m_csLock.Unlock();

			return true;
		}

		m_csLock.Unlock();

		return false;
	}

	long GetReferenceCount() const
	{
		return m_nReferenceCounter;
	}
};

template <class T>
MCriticalSectionSpinLock MRefObject<T>::m_csLock;


class MINET_API MLink : public MRefObject<MLink>
{
protected:
	MServerTcpSocket&			m_TCPSocket;
	MUID						m_UID;
	MCommandStream*				m_pCommandStream;
	bool						m_bAllowed;
	MPacketCrypter				m_PacketCrypter;
	void InitCrypt( MCommandStream& commandStream, MUID uidServer, MUID uidNew );

	MKeepingAliveData			m_KeepingAliveData;
public:
	MLink(MUID uidServer, MUID uidNew, MServerTcpSocket& tcp_socket, minet::MCommandManager* pCM);
	virtual ~MLink();

	virtual bool Disconnect();
	virtual void HardDisconnect();
	virtual void Send(BYTE* buf, int buflen);

	MCommandStream* GetCommandStream() { return m_pCommandStream; }
	MPacketCrypter* GetPacketCrypter() { return &m_PacketCrypter; }

	void ReAllocUID(MUID uidAlloc);
	MUID GetUID()						{ return m_UID; }

	void SetAllowed(bool bAllowed) { m_bAllowed = bAllowed; }
	bool IsAllowed() { return m_bAllowed; }

	bool CheckAlive(uint32 nNowTime, uint32 nAllowedTime) { return m_KeepingAliveData.CheckAlive(nNowTime, nAllowedTime); }
	void SetRecvTime(uint32 nTime)		{ m_KeepingAliveData.SetRecvTime(nTime); }

	SOCKET GetSocket() const	{ return m_TCPSocket.GetSocket(); }
};







} // namespace minet {