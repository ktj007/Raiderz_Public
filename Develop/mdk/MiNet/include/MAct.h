#pragma once

#include "MMemPool.h"
#include "MiNetPrerequisites.h"
#include "Constant.h"

namespace minet {

#define SOCK_BUFSIZE			4096
#define MAX_SEND_PENDING_LIMIT	1000			// 한 세션의 완료 통보없는 send 요청 수가, 이 값을 넘어가면 세션을 끊는다.

class MActor;
class MProactor;
class MDisconnector;
class MSender;
class MReceiver;
class MAcceptor;
class MServerTcpSocket;

class MAct : public WSAOVERLAPPED
{
protected:
	MActor*				m_pActor;
public:
	MAct() : m_pActor(NULL)
	{
		hEvent = NULL;
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
	}
	virtual ~MAct() {}

	// worker Thread에서 호출 ----
	virtual void Complete( DWORD bytes_transferred ) = 0;
	virtual void Error( DWORD bytes_transferred, DWORD error ) = 0;
	// worker Thread에서 호출 ----^
};

class MTcpAct : public MAct
{
public:
	MServerTcpSocket*		m_pTcpSocket;
	MUID					m_uidLink;
public:
	MTcpAct() : MAct(), m_pTcpSocket(NULL), m_uidLink(MUID::ZERO) {}
	virtual ~MTcpAct() {}

	// worker Thread에서 호출 ----
	void Complete(DWORD bytes_transferred);
	void Error(DWORD bytes_transferred, DWORD error);
	// worker Thread에서 호출 ----^
};


class MTcpActSend : public MTcpAct
{
protected:
	char*		m_pData;
	int			m_nTotalBytes;
public:
	MTcpActSend() : MTcpAct(), m_pData(NULL), m_nTotalBytes(0)	{}
	virtual ~MTcpActSend();

	bool Init(MActor* pActor, MServerTcpSocket* pTcpSocket, MUID uidLink, char* pData, int nDataLen);	
};

class MTcpActRecv : public MTcpAct
{
protected:
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	MTcpActRecv() : MTcpAct(), m_pBuffer(NULL), m_nBufferSize(0) 
	{

	}
	virtual ~MTcpActRecv() 
	{

	}

	void Init(MActor* pActor, MServerTcpSocket* pTcpSocket, MUID uidLink, char* pBuffer, int nBufferSize)
	{
		m_pActor = pActor;
		m_pTcpSocket = pTcpSocket;
		m_uidLink = uidLink;

		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
};


class MTcpActAccept : public MTcpAct
{
protected:
	SOCKET	m_hSocket;
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	MTcpActAccept() : MTcpAct(), m_hSocket(INVALID_SOCKET), m_pBuffer(NULL), m_nBufferSize(0) 
	{

	}
	virtual ~MTcpActAccept() 
	{

	}

	void Init(MActor* pActor, MServerTcpSocket* pTcpSocket, char* pBuffer, int nBufferSize, SOCKET hSocket)
	{
		m_pActor = pActor;
		m_pTcpSocket = pTcpSocket;

		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;

		m_hSocket = hSocket;
	}
};

class MTcpActConnect : public MTcpAct
{
public:
	MTcpActConnect(MActor* const pActor, MServerTcpSocket* const pTcpSocket)
	{
		assert(NULL!=pActor);
		assert(NULL!=pTcpSocket);
		m_pActor = pActor;
		m_pTcpSocket = pTcpSocket;
	}

	virtual ~MTcpActConnect() 
	{

	}
};

class MTcpActDisconnect : public MTcpAct
{
protected:
public:
	MTcpActDisconnect() : MTcpAct() 
	{

	}
	virtual ~MTcpActDisconnect() 
	{

	}

	void Init(MActor* pActor, MServerTcpSocket* pTcpSocket, MUID uidLink)
	{
		m_pActor = pActor;
		m_pTcpSocket = pTcpSocket;

		m_uidLink = uidLink;
	}
};

} // namespace minet