#include "stdafx.h"
#include "MPacketCrypter.h"
#include "MPacket.h"
#include "MCommandTable.h"
#include "MiNet.h"

namespace minet {

////////////////////////////////////////////////////////////////////////
MPacketCrypter::MPacketCrypter()
{
	m_nSHL = (MCOMMAND_VERSION % 6) + 1;
	m_ShlMask=0;

	for (int i = 0; i < m_nSHL; i++)
	{
		m_ShlMask += (1 << i);
	}
	memset(&m_Key, 0, sizeof(MPacketCrypterKey));
}

bool MPacketCrypter::InitKey(void* pKey, int nKeyLen)
{
	if (nKeyLen > PACKET_CRYPTER_KEY_LEN)
	{
		assert(0);
		return false;
	}

	memcpy(&m_Key, pKey, nKeyLen);
	return true;
}

char MPacketCrypter::_Enc(char s, char key)
{
	WORD w;
	BYTE b, bh;
	b = s ^ key;
	w = b << m_nSHL;
	bh = (w&0xFF00)>>8;
	b = w&0xFF;
	return( BYTE( b | bh ) ^ 0xF0 );
}

char MPacketCrypter::_Dec(char s, char key)
{
	BYTE b, bh, d;

	b = s^0xF0;
	bh = b&m_ShlMask;
	d = (bh<<(8-m_nSHL))|(b>>m_nSHL);

	return ( d ^ key );
}


void MPacketCrypter::Encrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pTarget = _Enc(*pSource, m_Key.szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pTarget++;
		pSource++;
	}
}

void MPacketCrypter::Encrypt(char* pSource, int nSrcLen)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pSource = _Enc(*pSource, m_Key.szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pSource++;
	}
}

bool MPacketCrypter::Decrypt(const char* pSource, int nSrcLen, char* pTarget, int nTarLen)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pTarget = _Dec(*pSource, m_Key.szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pTarget++;
		pSource++;
	}

	return true;
}


bool MPacketCrypter::Decrypt(char* pSource, int nSrcLen)
{
	int nKeyIndex = 0;
	for (int i = 0; i < nSrcLen; i++)
	{
		*pSource = _Dec(*pSource, m_Key.szKey[nKeyIndex]);

		nKeyIndex++;
		if (nKeyIndex >= PACKET_CRYPTER_KEY_LEN) nKeyIndex = 0;
		pSource++;
	}

	return true;
}


} // namespace minet