#include "stdafx.h"
#include "MPacketBuilder.h"

namespace minet {

MClientPacketBuilder::MClientPacketBuilder() : m_nSerialNumber(0)
{

}

MClientPacketBuilder::~MClientPacketBuilder()
{

}

int MClientPacketBuilder::BuildPacket( char* pOutPacket, int nMaxPacketSize, MCommand* pCommand, MPacketCrypter* pPacketCrypter )
{
	MCommandMsg* pMsg = (MCommandMsg*)pOutPacket;

	int nCmdSize = nMaxPacketSize - sizeof(MPacketHeader);

	pMsg->Buffer[0] = 0;
	pMsg->nCheckSum = 0;
	int nPacketSize = 0;

	if (pCommand->m_pCommandDesc->IsFlag(MCDT_NON_ENCRYPTED))
	{
		// 암호화하지 않는 커맨드
		pMsg->nMsg = MSGID_RAWCOMMAND;

		nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);
		pMsg->nSize = (unsigned short)(sizeof(MPacketHeader) + nCmdSize);

		nPacketSize = pMsg->nSize;

		pMsg->nCheckSum = MBuildCheckSum(pMsg, nPacketSize);
	}
	else
	{
		// 암호화되는 커맨드
		pMsg->nMsg = MSGID_COMMAND;

		nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);
		nPacketSize = sizeof(MPacketHeader) + nCmdSize;
		pMsg->nSize = (unsigned short)(nPacketSize);


		// 커맨드 암호화
		if (pPacketCrypter)
		{
			pPacketCrypter->Encrypt(pMsg->Buffer, nCmdSize);
		}

		pMsg->nCheckSum = MBuildCheckSumAndSerialNumber(pMsg, nPacketSize, ++m_nSerialNumber);
	}


	return nPacketSize;
}

//////////////////////////////////////////////////////////////////////////
int MServerPacketBuilder::BuildPacket( BYTE* pOut, int nPacketSize, MCommand* pCommand, MPacketCrypter* pPacketCrypter )
{
	MCommandMsg* pMsg = (MCommandMsg*)(pOut);

	int nCmdSize = nPacketSize - sizeof(MPacketHeader);

	if(pCommand->m_pCommandDesc->IsFlag(MCDT_NON_ENCRYPTED))
	{
		pMsg->nMsg = MSGID_RAWCOMMAND;
		nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);
	}
	else
	{
		pMsg->nMsg = MSGID_COMMAND;
		{
			nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);

			if (pPacketCrypter)
			{
				pPacketCrypter->Encrypt(pMsg->Buffer, nCmdSize);
			}
		}
	}

	if (nCmdSize <= 0)
	{
		return 0;
	}

	pMsg->nSize = nPacketSize;
	pMsg->nCheckSum = MBuildCheckSum(pMsg, nPacketSize);

	return nPacketSize;
}

int MServerPacketBuilder::BuildRawPacket( BYTE* pOut, int nPacketSize, MCommand* pCommand )
{
	MCommandMsg* pMsg = (MCommandMsg*)(pOut);

	int nCmdSize = nPacketSize - sizeof(MPacketHeader);

	if(pCommand->m_pCommandDesc->IsFlag(MCDT_NON_ENCRYPTED))
	{
		pMsg->nMsg = MSGID_RAWCOMMAND;
	}
	else
	{
		pMsg->nMsg = MSGID_COMMAND;
	}

	nCmdSize = pCommand->GetData(pMsg->Buffer, nCmdSize);

	if (nCmdSize <= 0)
	{
		return 0;
	}

	pMsg->nSize = nCmdSize + sizeof(MPacketHeader);

	return pMsg->nSize;
}

void MServerPacketBuilder::BuildPacketEncryptionAndChecksum( MCommandMsg* pOutMsg, MCommand* pCommand, int nPacketSize, MPacketCrypter* pPacketCrypter )
{
	if (pPacketCrypter)
	{
		int nCmdSize = nPacketSize - sizeof(MPacketHeader);

		pPacketCrypter->Encrypt(pOutMsg->Buffer, nCmdSize);
	}

	pOutMsg->nCheckSum = MBuildCheckSum(pOutMsg, nPacketSize);
}

} // namespace minet