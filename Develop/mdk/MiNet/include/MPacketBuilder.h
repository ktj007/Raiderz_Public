#pragma once

#include "MPacketCrypter.h"
#include "MCommand.h"
#include "MPacket.h"

namespace minet {

class MClientPacketBuilder
{
private:
	uint8 m_nSerialNumber;
public:
	MClientPacketBuilder();
	~MClientPacketBuilder();

	int BuildPacket(char* pOutPacket, int nMaxPacketSize, MCommand* pCommand, MPacketCrypter* pPacketCrypter);
};

class MServerPacketBuilder
{
private:

public:
	MServerPacketBuilder() {}
	~MServerPacketBuilder() {}

	int BuildPacket(BYTE* pOut, int nPacketSize, MCommand* pCommand, MPacketCrypter* pPacketCrypter);
	int BuildRawPacket(BYTE* pOut, int nPacketSize, MCommand* pCommand);
	void BuildPacketEncryptionAndChecksum(MCommandMsg* pOutMsg, MCommand* pCommand, int nPacketSize, MPacketCrypter* pPacketCrypter);
};

} // namespace minet