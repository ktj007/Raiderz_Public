#include "stdafx.h"
#include "RFile.h"

namespace rs3
{
	RFile::RFile( void )
		:m_pBuffer(NULL),m_StartingByteToRead(0),m_FileSize(0)
	{
	}

	RFile::~RFile( void )
	{
		Close();
	}

	bool RFile::Open( const char* szFileName )
	{
		_ASSERT(NULL == m_pBuffer);

		if( false == m_MFile.Open(szFileName) )	return false;

		size_t SizeRead = 0;
		size_t FileSize = m_MFile.GetLength();
		m_pBuffer = new char[FileSize];

		if( m_MFile.Read(m_pBuffer,FileSize,&SizeRead) )
		{
			if( FileSize != SizeRead )
			{
				SAFE_DELETE_ARRAY(m_pBuffer);
				m_MFile.Close();
				return false;
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_pBuffer);
			m_MFile.Close();
			return false;
		}

		m_FileSize = FileSize;
		return true;
	}

	bool RFile::Read( void* pBuffer,size_t nBytesToRead)
	{
		if( NULL == m_pBuffer )	return false;
		
		memcpy(pBuffer,m_pBuffer+m_StartingByteToRead,nBytesToRead);
		m_StartingByteToRead+= nBytesToRead;

		_ASSERT( m_StartingByteToRead <= m_FileSize );

		return true;
	}

	void RFile::Close( void )
	{
		m_MFile.Close();
		SAFE_DELETE_ARRAY(m_pBuffer);
		m_StartingByteToRead = 0;
		m_FileSize = 0;
	}

	void RFile::SeekFromCurrent( long nOffset )
	{
		m_StartingByteToRead+= nOffset;
		_ASSERT( m_StartingByteToRead < m_FileSize );
	}
}