#ifndef _MIOCP_OVERLAPPED_H
#define _MIOCP_OVERLAPPED_H

namespace minet {

typedef enum MIOCP_OPERATION
{
	// server
	MIOCP_OP_NONE = 0,
	MIOCP_OP_ACCEPT,
	MIOCP_OP_SEND,
	MIOCP_OP_RECV,

	// client
	MIOCP_OP_CONNECT,
	MIOCP_OP_DISCONNECT,
};

class MIOCP_Overlapped : public WSAOVERLAPPED
{
protected:
	MIOCP_OPERATION		m_IOOperation;
public:
	MIOCP_Overlapped(MIOCP_OPERATION nIO) 
	{
		ZeroMemory(this, sizeof(MIOCP_Overlapped));	
		m_IOOperation = nIO;
	}
	MIOCP_OPERATION GetIOOperation()			{ return m_IOOperation; }

	virtual ~MIOCP_Overlapped() {}
};

class MIOCP_OverlappedSend : public MIOCP_Overlapped
{
protected:
	int		m_nTotalBytes;
	int		m_nTransBytes;
	char*	m_pData;
public:
	MIOCP_OverlappedSend() : MIOCP_Overlapped(MIOCP_OP_SEND)
	{
		m_nTotalBytes = 0;
		m_nTransBytes = 0;
		m_pData = NULL;
	}
	~MIOCP_OverlappedSend() 
	{
		free(m_pData);	
		m_pData = NULL;
	}
	void SetData(char* pData, int nDataLen) {
		m_pData = pData;
		m_nTotalBytes = nDataLen;
	}
	int		GetTotalBytes()				{ return m_nTotalBytes; }
	int		GetTransBytes()				{ return m_nTransBytes; }
	void	AddTransBytes(int nBytes)	{ m_nTransBytes += nBytes; }
	char*	GetData()					{ return m_pData; }
};

class MIOCP_OverlappedRecv : public MIOCP_Overlapped
{ 
protected:
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	MIOCP_OverlappedRecv() : MIOCP_Overlapped(MIOCP_OP_RECV)
	{
		m_pBuffer = 0;
		m_nBufferSize = 0;
	}
	~MIOCP_OverlappedRecv() {}
	void SetBuffer(char* pBuffer, int nBufferSize) 
	{
		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
	char* GetBuffer()				{ return m_pBuffer; }
	int GetBufferSize()				{ return m_nBufferSize; }
};

class MIOCP_OverlappedAccept : public MIOCP_Overlapped 
{
protected:
	SOCKET	m_Socket;
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	MIOCP_OverlappedAccept() : MIOCP_Overlapped(MIOCP_OP_ACCEPT)
	{
		m_Socket = INVALID_SOCKET;
		m_pBuffer = 0;
		m_nBufferSize = 0;
	}
	~MIOCP_OverlappedAccept()		{}
	void SetSocket(SOCKET sd)		{ m_Socket = sd; }
	SOCKET GetSocket()				{ return m_Socket; }
	void SetBuffer(char* pBuffer, int nBufferSize)
	{
		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
	char* GetBuffer()				{ return m_pBuffer; }
	int GetBufferSize()				{ return m_nBufferSize; }
};

} // namespace minet


#endif