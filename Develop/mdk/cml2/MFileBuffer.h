#pragma once

#include "MFileSystem.h"

class CML2_API MFileBuffer
{
public:
	MFileBuffer(void);
	~MFileBuffer(void);

public:
	bool	Open(const char* szFileName);
	bool	Read(void* pBuffer,size_t nBytesToRead);
	void	Close(void);
	void	SeekFromCurrent(long nOffset);

private:
	MFile	m_MFile;
	char*	m_pBuffer;

	size_t	m_FileSize;
	size_t	m_StartingByteToRead;
};