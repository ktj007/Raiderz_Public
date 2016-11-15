#include "StdAfx.h"
#include ".\MBlob.h"
#include <memory.h>
#include <assert.h>

#define MAX_BLOB_DATA_SIZE		(0x100000)			// 1메가바이트

MBlob::MBlob(void)
{
	Clear();
}

MBlob::MBlob(const void* pData, unsigned short nTypeSize, unsigned short nBlobCount)
: m_nTypeSize(nTypeSize)
, m_nBlobCount(nBlobCount)
{
	m_vecData.resize(nTypeSize*nBlobCount);
	if ((nBlobCount>0) && (pData != NULL))
		memcpy(&m_vecData[0], pData, nTypeSize * nBlobCount);
}

int MBlob::GetBlobTotalSize()
{
	return ((int)m_vecData.size() + sizeof(unsigned short) + sizeof(unsigned short));
}

void* MBlob::GetBlobElement(int i)
{
	if(i < 0 || i >= GetBlobCount()) return NULL;
	return ((unsigned char*)&m_vecData[0] + GetBlobSize() * i);
}

bool MBlob::SetBlobElement( int i, const void* pData, const int nTypeSize )
{
	if (i < 0 || i >= GetBlobCount())	return false;
	if (NULL == pData)					return false;
	if (nTypeSize != m_nTypeSize)		return false;

	memcpy(&m_vecData[0]+m_nTypeSize*i, pData, m_nTypeSize);
	return true;
}

void* MBlob::GetBlobPointer()
{
	if (m_vecData.empty()) return NULL;

	return ((unsigned char*)&m_vecData[0]);
}

void MBlob::Clear()
{
	m_vecData.clear();
	m_nTypeSize = 0;
	m_nBlobCount = 0;
}

int MBlob::GetData(char* pBuffer, const int nBufferSize)
{
	if (NULL == pBuffer)	return 0;
	if (0 >= nBufferSize)	return 0;

	int nBlobSize = (int)GetBlobTotalSize();
	int nDataSize = (int)m_vecData.size();
	
	if (nBlobSize > nBufferSize) return 0;
	
	memcpy(pBuffer, &m_nTypeSize, sizeof(unsigned short));
	memcpy(pBuffer + sizeof(unsigned short), &m_nBlobCount, sizeof(unsigned short));

	if (nDataSize > 0)
		memcpy(pBuffer + sizeof(unsigned short) + sizeof(unsigned short), &m_vecData[0], nDataSize);

	return GetBlobTotalSize();
}

int MBlob::SetData(const char* pData, const int nDataSize)
{
	if (NULL == pData)	return -1;
	if (0 >= nDataSize)	return -1;

	int nDataIndex = 0;

	unsigned short nTypeSize = 0;
	memcpy(&nTypeSize,	pData + nDataIndex, sizeof(unsigned short));
	nDataIndex += sizeof(unsigned short);

	unsigned short nBlobCount = 0;
	memcpy(&nBlobCount,	pData + nDataIndex, sizeof(unsigned short));
	nDataIndex += sizeof(unsigned short);

	int nSize = (nTypeSize * nBlobCount);
	if( nSize > nDataSize || nSize > MAX_BLOB_DATA_SIZE )
	{
		_ASSERT(!"Data가 너무 크다");
		Clear();
		return -1;
	}
	else if (nSize<=0)
	{
		Clear();
		return GetBlobTotalSize();
	}

	m_vecData.resize(nSize);
	memcpy(&m_vecData[0], pData + nDataIndex, nSize);
	m_nTypeSize = nTypeSize;
	m_nBlobCount = nBlobCount;

	return GetBlobTotalSize();
}
