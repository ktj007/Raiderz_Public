#include "stdafx.h"
#include "MCommandStream.h"
#include "MPacketCrypter.h"

namespace minet {


MCommandStream::MCommandStream(MCommandManager*	pCmdMgr) 
{	
	m_pCommandManager = pCmdMgr;
	m_pPacketCrypter = NULL;

	m_nBufferNext = 0;
}

MCommandStream::~MCommandStream() 
{
	Clear();
}

bool MCommandStream::IsBufferEmpty() 
{
	if (m_nBufferNext == 0) return true;
	else return false;
}

bool MCommandStream::EstimateBufferToCmd() 
{
	if (m_nBufferNext < sizeof(MPacketHeader)) return false;

	MPacketHeader* pPacket = (MPacketHeader*)m_Buffer;
	if (m_nBufferNext < pPacket->nSize) return false;

	return true;
}

void MCommandStream::AddBuffer(char* pBuffer, int nLen) 
{
	if (nLen <= 0) return;
	if ((m_nBufferNext + nLen) >= COMMAND_BUFFER_LEN) 
	{
		_ASSERT(FALSE);
		return;
	}
	CopyMemory(m_Buffer+m_nBufferNext, pBuffer, nLen);
	m_nBufferNext += nLen;
}

int MCommandStream::JustAddBuffer( char* pBuffer, int nLen )
{
	if (nLen <= 0) return 0;
	
	int nRemainLen = nLen;

	// 헤더 만들기에도 부족하면 다 더한다.
	if (m_nBufferNext + nLen < sizeof(MPacketHeader))
	{		
		AddBuffer(pBuffer, nLen);
		return 0;
	}

	// 헤더가 없다면, 헤더를 만든다.		
	if (m_nBufferNext < sizeof(MPacketHeader))
	{
		int nLackSize = sizeof(MPacketHeader) - m_nBufferNext;
		
		AddBuffer(pBuffer, nLackSize);
		nRemainLen -= nLackSize;
	}
	
	// 커맨드 만들기에 부족하면 다 더한다.
	MPacketHeader* pPacket = (MPacketHeader*)m_Buffer;
	int nReadedLen = nLen - nRemainLen;

	if (m_nBufferNext + nRemainLen < pPacket->nSize)
	{
		AddBuffer(pBuffer + nReadedLen, nRemainLen);
		return 0;
	}
	
	// 커맨드 하나만 더한다.
	int nLackSize = pPacket->nSize - m_nBufferNext;
	AddBuffer(pBuffer + nReadedLen, nLackSize);
	nRemainLen -= nLackSize;

	return nRemainLen;
}

int MCommandStream::MakeCommand(char* pBuffer, int nBufferLen) 
{
	return m_CommandBuilder.BuildCommand(pBuffer, nBufferLen, m_CommandList, m_pPacketCrypter);
}

void MCommandStream::Clear()
{
	if (!m_CommandList.empty())
	{
		for (MCommandList::iterator itorCmd = m_CommandList.begin(); itorCmd != m_CommandList.end(); ++itorCmd)
		{
			MCommand* pCmd = (*itorCmd);
			SAFE_DELETE(pCmd);
		}
		m_CommandList.clear();
	}

}

bool MCommandStream::ReadBuffer( char* pBuffer, int nBufferLen )
{
	MPacketHeader* pPacket = (MPacketHeader*)pBuffer;

	if ( (nBufferLen < sizeof(MPacketHeader)) || (nBufferLen < pPacket->nSize) ) 
	{	
		AddBuffer(pBuffer, nBufferLen);	// 내부 Buffer에 저장
	} 
	else 
	{	// Build Command --------------
		int nSpareData = MakeCommand(pBuffer, nBufferLen);			// Arg로 넘어온 외부버퍼에서 바로 Cmd생성
		if (nSpareData > 0) 
		{
			AddBuffer(pBuffer+(nBufferLen-nSpareData), nSpareData);	// 남은부분 내부 Buffer에 저장(내부버퍼처음사용)
		}
		else if (nSpareData < 0) 
		{
			return false;
		}
	}

	return true;
}

bool MCommandStream::Read(char* pBuffer, int nBufferLen) 
{
	if (IsBufferEmpty() == true)  
	{
		return ReadBuffer(pBuffer, nBufferLen);
	} 
	else 
	{
		// 저장된 부분 패킷에 부족한 사이즈만큼만 더한다.
		int nRemainLen = JustAddBuffer(pBuffer, nBufferLen);

		if (EstimateBufferToCmd())
		{
			// 버퍼에서 커맨드 생성; 딱 한개분량만 들어 있다.
			int nSpareData = MakeCommand(m_Buffer, m_nBufferNext);
			_ASSERT(nSpareData == 0);

			if (0 <= nSpareData)
			{			
				m_nBufferNext = 0;
			}
			else
			{
				return false;
			}

			// 읽을 데이터가 남아 있으면 추가로 읽는다.
			if (0 < nRemainLen)
			{
				int nUnreadedBegin = nBufferLen - nRemainLen;
				return ReadBuffer(pBuffer + nUnreadedBegin, nRemainLen);
			}
		}
	}

	return true;
}

MCommand* MCommandStream::GetCommand() 
{
	MCommandList::iterator itorCmd = m_CommandList.begin();
	if (itorCmd != m_CommandList.end()) 
	{
		MCommand* pCmd = (*itorCmd);
		m_CommandList.pop_front();
		return (pCmd);
	} 
	else 
	{
		return NULL;
	}
}


void MCommandStream::InitCrypt(MPacketCrypter* pPacketCrypter, bool bCheckCommandSerialNumber)
{
	m_pPacketCrypter = pPacketCrypter;
	m_CommandBuilder.SetSerialCheckerActive(bCheckCommandSerialNumber);
}

} // namespace minet

