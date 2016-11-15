#pragma once

#include "MCommandManager.h"

namespace minet {

class MPacketCrypter;

class MCommandSNChecker
{
private:
	uint8	m_nLastSerialNumber;
	bool	m_bActive;
public:
	MCommandSNChecker();
	~MCommandSNChecker();
	bool CheckValidate(unsigned short nPacketCheckSum);
	void SetActive(bool b) { m_bActive = b; }
	bool IsActive() { return m_bActive; }
};

class MCommandBuilder
{
private:
	MCommandSNChecker	m_CommandSNChecker;
public:
	MCommandBuilder();
	~MCommandBuilder();

	int BuildCommand(char* pBuffer, int nBufferLen, MCommandList& commandList, MPacketCrypter* pPacketCrypter);
	void SetSerialCheckerActive(bool b);
};


} // namespace minet