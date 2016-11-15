#ifndef _MPACKET_CRYPTER_H
#define _MPACKET_CRYPTER_H

#include "MiNetPrerequisites.h"
#include "MiNet.h"

namespace minet {

class MINET_API MPacketCrypter
{
private:
	static const int PACKET_CRYPTER_KEY_LEN = 32;

	struct MPacketCrypterKey
	{
		char szKey[PACKET_CRYPTER_KEY_LEN];
	};

	MPacketCrypterKey	m_Key;
	int					m_nSHL;
	unsigned char		m_ShlMask;

	char _Enc(char s, char key);
	char _Dec(char s, char key);
public:
	MPacketCrypter();
	~MPacketCrypter() {}
	bool InitKey(void* pKey, int nKeyLen);

	void Encrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen);
	bool Decrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen);

	void Encrypt(char* pSource, int nSrcLen);
	bool Decrypt(char* pSource, int nSrcLen);
};

inline void MMakeCryptKey(unsigned char* poutKey, int nKeyLen, MUID uidServer, MUID uidClient, unsigned int nTimeStamp)
{
	// key
	memset(poutKey, 0, nKeyLen);
	int nUIDSize = sizeof(MUID);

	memcpy(poutKey, &nTimeStamp, sizeof(unsigned int));
	memcpy(poutKey+sizeof(unsigned int), &uidServer.Low, sizeof(unsigned int));
	memcpy(poutKey+nUIDSize, &uidClient, nUIDSize);

	const BYTE XOR[16] = {87, 2, 91, 4, 52, 6, 1, 8, 55, 10, 18, 105, 65, 56, 15, 120};

	for (int i = 0; i < 16; i++)
	{
		poutKey[i] ^= XOR[i];
	}

	poutKey = poutKey + 16;

	// iv
	poutKey[0] = 27;
	poutKey[7] = 83;
	poutKey[1] = 4;
	poutKey[2] = 36;
	poutKey[3] = 34;
	poutKey[4] = 67;
	poutKey[5] = MCOMMAND_VERSION;
	poutKey[11] = 53;
	poutKey[6] = 73;
	poutKey[8] = 80;
	poutKey[9] = 5;
	poutKey[10] = 19;
	poutKey[12] = 79;
	poutKey[13] = 2;
	poutKey[14] = 77;
	poutKey[15] = 5;
}

} // namespace minet

#endif