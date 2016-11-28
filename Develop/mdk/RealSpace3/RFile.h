#pragma once

#include "MFileSystem.h"

namespace rs3
{
	class RS_API RFile
	{
	public:
		RFile(void);
		~RFile(void);
	
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
}