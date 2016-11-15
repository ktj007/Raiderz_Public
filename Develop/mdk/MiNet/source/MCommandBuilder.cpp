#include "stdafx.h"
#include "MCommandBuilder.h"
#include "MPacketCrypter.h"
#include "MPacket.h"

namespace minet {

////////////////////////////////////////////////////////////////////////////////////
// MCommandSNChecker ///////////////////////////////////////////////////////////////
MCommandSNChecker::MCommandSNChecker() : m_nLastSerialNumber(UCHAR_MAX), m_bActive(false)
{

}

MCommandSNChecker::~MCommandSNChecker()
{

}


bool MCommandSNChecker::CheckValidate(unsigned short nPacketCheckSum)
{
	if (m_bActive == false) return true;

	uint8 nSerialNumber = GetPacketSerialNumberFromChecksum(nPacketCheckSum);
	if (nSerialNumber == m_nLastSerialNumber) return false;

	m_nLastSerialNumber = nSerialNumber;

	return true;
}


int MCommandBuilder::BuildCommand( char* pBuffer, int nBufferLen, MCommandList& commandList, MPacketCrypter* pPacketCrypter )
{
	const int RET_ERROR = -1;

	unsigned int nOffset = 0;
	int nCurrentBufferLen = nBufferLen;
	MPacketHeader* pPacket = (MPacketHeader*)(pBuffer+nOffset);
	int nCmdCount = 0;

	while (nCurrentBufferLen >= sizeof(MPacketHeader))
	{
		int nPacketSize = pPacket->nSize;
		if ((nPacketSize > nCurrentBufferLen) || (nPacketSize <= 0)) break;
		if (nPacketSize > MAX_PACKET_SIZE)
		{
			return RET_ERROR;
		}

		// Msg ID 검사
		if (pPacket->nMsg != MSGID_COMMAND && pPacket->nMsg != MSGID_RAWCOMMAND)
		{
			return RET_ERROR;
		}

		// 체크섬 검사
		if (IsEqualCheckSum(pPacket->nCheckSum, pPacket, nPacketSize) == false)
		{
			return RET_ERROR;
		}

		// 시리얼 번호 검사
		if (pPacket->nMsg == MSGID_COMMAND)
		{
			if (!m_CommandSNChecker.CheckValidate(pPacket->nCheckSum))
			{
				return RET_ERROR;
			}
		}

		MCommand* pNewCmd = new MCommand();

		int nCmdSize = nPacketSize - sizeof(MPacketHeader);

		// 암호화되었으면 디코딩
		if (pPacket->nMsg == MSGID_COMMAND) 
		{
			if (pPacketCrypter)
			{
				if (!pPacketCrypter->Decrypt((char*)((MCommandMsg*)pPacket)->Buffer, nCmdSize))
				{
					delete pNewCmd; pNewCmd = NULL;
					return RET_ERROR;
				}
			}
		}

		if (pNewCmd->SetData(((MCommandMsg*)pPacket)->Buffer, (unsigned short)nCmdSize))
		{
			commandList.push_back(pNewCmd);
		}
		else
		{
			delete pNewCmd; pNewCmd = NULL;
			return RET_ERROR;
		}

		nOffset += nPacketSize;
		nCurrentBufferLen -= nPacketSize;			
		nCmdCount++;

		pPacket = (MPacketHeader*)(pBuffer+nOffset);
	}

	return nCurrentBufferLen;
}

MCommandBuilder::MCommandBuilder()
{

}

MCommandBuilder::~MCommandBuilder()
{

}

void MCommandBuilder::SetSerialCheckerActive( bool b )
{
	m_CommandSNChecker.SetActive(b);
}

} // namespace minet