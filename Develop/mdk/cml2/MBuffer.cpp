#include "stdafx.h"
#include "MBuffer.h"


MBuffer::MBuffer( void )
:m_pBuffer(NULL),m_BufferSize(0),m_SavePointer(0),m_ReadPointer(0)
{
}

MBuffer::~MBuffer( void )
{
	FreeBuffer();
}

void MBuffer::AllocateBuffer( int Size )
{
	_ASSERT(NULL == m_pBuffer && 0 == m_BufferSize);
	_ASSERT(Size>0);

	m_pBuffer = new char[Size];
	m_BufferSize = Size;
}

void MBuffer::FreeBuffer( void )
{
	if( NULL != m_pBuffer )
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	m_BufferSize = 0;
	m_SavePointer = 0;
	m_ReadPointer = 0;
}

int	 MBuffer::Append( char* pSource,int Size )
{
	_ASSERT(NULL != m_pBuffer);

	if( GetEmptySize() < Size )
	{
		_ASSERT(false);	// Buffer의 사이즈가 작습니다.
		return 0;
	}

	memcpy(m_pBuffer+m_SavePointer,pSource,Size);
	m_SavePointer+= Size;

	return Size;
}

void MBuffer::Read( char* pDest,int ReadSize )
{
	_ASSERT( NULL != m_pBuffer );
	_ASSERT( NULL != pDest );
	_ASSERT( ReadSize <= GetRemainReadSize() );

	memcpy(pDest,m_pBuffer+m_ReadPointer,ReadSize);
	m_ReadPointer+= ReadSize;
}

void MBuffer::SetBuffer( char* pSource,int Size )
{
	FreeBuffer();
	AllocateBuffer(Size);
	Append(pSource,Size);
}

void MBuffer::Reset( void )
{
	m_SavePointer = 0;
	m_ReadPointer = 0;
}