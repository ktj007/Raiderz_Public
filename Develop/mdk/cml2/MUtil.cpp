#include "stdafx.h"
#include "MUtil.h"
#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

MRingBuffer::MRingBuffer() : m_pBuf(NULL)
{
	Reserve(DEFAULT_RINGBUFFER_SIZE);
}

MRingBuffer::MRingBuffer(int nBufSize) : m_pBuf(NULL)
{
	Reserve(nBufSize);
}

MRingBuffer::~MRingBuffer()
{
	// 전체 락----
	MCriticalSection::Guard guard(m_csLock);

	if (m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = NULL;
	}
}

void MRingBuffer::Reserve(int nBufSize)
{
	// 전체 락----
	MCriticalSection::Guard guard(m_csLock);

	if (nBufSize < 0) nBufSize = 0;

	if (m_pBuf)
	{
		delete [] m_pBuf; m_pBuf = NULL;
	}

	m_nDataSize = 0;
	m_nBufSize = nBufSize;

	m_pBuf = new char[m_nBufSize+1];

	m_pStartData = m_pBuf;
	m_pCursor = m_pBuf;
	m_pEndBuf = &m_pBuf[m_nBufSize];

}

bool MRingBuffer::Push(void* pBuf, int nSize)
{
	// 전체 락----
	MCriticalSection::Guard guard(m_csLock);

	if (nSize > (m_nBufSize-m_nDataSize)) return false;

	int nSpareBack = (int)(m_pEndBuf - m_pCursor);		// 뒤쪽에 남은 버퍼
	if (nSpareBack >= nSize)
	{
		memcpy(m_pCursor, pBuf, nSize);
		m_pCursor += nSize;
	}
	else
	{
		memcpy(m_pCursor, pBuf, nSpareBack);
		memcpy(m_pBuf, (char *)pBuf + nSpareBack, nSize - nSpareBack);
		m_pCursor = m_pBuf + nSize - nSpareBack;
	}
	m_nDataSize += nSize;
	return true;
}

bool MRingBuffer::Pop(void* pBuf, int nSize)
{
	// 전체 락----
	MCriticalSection::Guard guard(m_csLock);

	if (nSize > m_nDataSize || m_nDataSize == 0) return false;

	int nSpare = (int)(m_pEndBuf - m_pStartData);
	if (nSpare >= nSize)
	{
		memcpy(pBuf, m_pStartData, nSize);
		m_pStartData += nSize;
	}
	else
	{
		memcpy(pBuf, m_pStartData, nSpare);
		memcpy((char*)pBuf+nSpare, m_pBuf, nSize-nSpare);
		m_pStartData = m_pBuf + nSize - nSpare;
	}

	m_nDataSize -= nSize;
	return true;
}

bool MRingBuffer::ReadFront(void* pBuf, int nSize)
{
	// 전체 락----
	MCriticalSection::Guard guard(m_csLock);

	if (nSize > m_nDataSize || m_nDataSize == 0) return false;

	int nSpare = (int)(m_pEndBuf - m_pStartData);
	if (nSpare >= nSize)
	{
		memcpy(pBuf, m_pStartData, nSize);
	}
	else
	{
		memcpy(pBuf, m_pStartData, nSpare);
		memcpy((char*)pBuf+nSpare, m_pBuf, nSize-nSpare);
	}

	return true;
}

int MRingBuffer::GetSize()
{
	int nDataSize=0;
	{
		MCriticalSection::Guard guard(m_csLock);
		nDataSize = m_nDataSize;
	}

	return nDataSize;
}

int MRingBuffer::GetCapacity()
{
	int nBufSize=0;
	{
		MCriticalSection::Guard guard(m_csLock);
		nBufSize = m_nBufSize;
	}

	return nBufSize;
}

//---------------------------------------------------------------------

char*	
MByteBuffer::GetBuffer()
{
	return m_Buf;
}

char*&	
MByteBuffer::GetBufferRef()
{
	return m_Buf;
}

void	
MByteBuffer::SetBuffer(char* buf)
{
	this->m_Buf = buf;
}

void	
MByteBuffer::WritePos(size_t pos)
{
	this->m_nWritePos = pos;
}

size_t	
MByteBuffer::WritePos()
{
	return this->m_nWritePos;
}

void	
MByteBuffer::ReadPos(size_t pos)
{
	this->m_nReadPos = pos;
}

size_t	
MByteBuffer::ReadPos()
{
	return this->m_nReadPos;
}

int operator<<(MByteBuffer& lhs, char data)
{
	return lhs.WriteChar(data);
}

int operator<<(MByteBuffer& lhs, int data)
{
	return lhs.WriteInt(data);
}

int operator<<(MByteBuffer& lhs, unsigned int data)
{
	return lhs.WriteUInt(data);
}

int operator<<(MByteBuffer& lhs, short data)
{
	return lhs.WriteShort(data);
}

int operator<<(MByteBuffer& lhs, unsigned short data)
{
	return lhs.WriteUShort(data);
}

int operator<<(MByteBuffer& lhs, long data)
{
	return lhs.WriteLong(data);
}

int operator<<(MByteBuffer& lhs, unsigned long data)
{
	return lhs.WriteULong(data);
}

int operator<<(MByteBuffer& lhs, float data)
{
	return lhs.WriteFloat(data);
}

int operator<<(MByteBuffer& lhs, double data)
{
	return lhs.WriteDouble(data);
}

int operator<<(MByteBuffer& lhs, string data)
{
	return lhs.WriteString(data);
}

int operator<<(MByteBuffer& lhs, MUID data)
{
	return lhs.WriteUID(data);
}

int operator>>(MByteBuffer& lhs, char& data)
{
	return lhs.ReadChar(data);
}

int operator>>(MByteBuffer& lhs, int& data)
{
	return lhs.ReadInt(data);
}

int operator>>(MByteBuffer& lhs, unsigned int& data)
{
	return lhs.ReadUInt(data);
}

int operator>>(MByteBuffer& lhs, short& data)
{
	return lhs.ReadShort(data);
}

int operator>>(MByteBuffer& lhs, unsigned short& data)
{
	return lhs.ReadUShort(data);
}

int operator>>(MByteBuffer& lhs, long& data)
{
	return lhs.ReadLong(data);
}

int operator>>(MByteBuffer& lhs, unsigned long& data)
{
	return lhs.ReadULong(data);
}

int operator>>(MByteBuffer& lhs, float& data)
{
	return lhs.ReadFloat(data);
}

int operator>>(MByteBuffer& lhs, double& data)
{
	return lhs.ReadDouble(data);
}

int operator>>(MByteBuffer& lhs, string& data)
{
	return lhs.ReadString(data);
}

int operator>>(MByteBuffer& lhs, MUID& data)
{
	return lhs.ReadUID(data);
}

void 
MByteBuffer::SetMaxSize(size_t nSize)
{
	if (nSize == 0)
		m_nMaxBufSize = nSize;

	if (m_nMaxBufSize < DEFAULT_BYTE_BUFFER_SIZE)
		m_nMaxBufSize = DEFAULT_BYTE_BUFFER_SIZE;

	char* temp_buffer = new char[m_nMaxBufSize];

	if (m_Buf)
	{
		memcpy(temp_buffer, m_Buf, m_nMaxBufSize);
		delete m_Buf;
	}

	m_Buf = temp_buffer;
}

void 
MByteBuffer::AutoSetMaxSizeImpl(size_t data_size)
{
	while(1)
	{
		if (m_nWritePos+data_size < this->m_nMaxBufSize)
			break;

		this->m_nMaxBufSize *= 2;
	}
}

int 
MByteBuffer::WriteChar(const char data)
{
	size_t size = sizeof(char);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteInt(const int data)
{
	size_t size = sizeof(int);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteUInt(const unsigned int data)
{
	size_t size = sizeof(unsigned int);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteShort(const short data)
{
	size_t size = sizeof(short);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteUShort(const unsigned short data)
{
	size_t size = sizeof(unsigned short);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteLong(const long data)
{
	size_t size = sizeof(long);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteULong(const unsigned long int data)
{
	size_t size = sizeof(unsigned long);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteFloat(const float data)
{
	size_t size = sizeof(float);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int 
MByteBuffer::WriteDouble(const double data)
{
	size_t size = sizeof(double);

	if (this->WriteData(&data, size) == -1)
		return -1;

	this->m_nWritePos += size;

	return 0;
}

int  
MByteBuffer::WriteString(string data)
{
	short size = static_cast<short>(data.length())+1;

	if (this->WriteShort(size) == -1)
		return -1;

	if (this->WriteData(data.c_str(), size) == -1)
		return -1;

	this->m_nWritePos += sizeof(size);
	this->m_nWritePos += size;

	return 0;
}

int  
MByteBuffer::WriteUID(MUID data)
{
	if (this->WriteULong(data.High) == -1)
		return -1;

	if (this->WriteULong(data.Low) == -1)
		return -1;

	this->m_nWritePos += sizeof(data.High) + sizeof(data.Low);

	return 0;
}

int  
MByteBuffer::WriteData(const void* data, size_t size)
{
	if (size <= 0)
		return -1;

	if (m_nWritePos+size >= m_nMaxBufSize)
	{
		this->AutoSetMaxSizeImpl(size);
		this->SetMaxSize();
	}

	memcpy(m_Buf+m_nWritePos, data, size);

	return 0;
}


int 
MByteBuffer::ReadChar(char& data)
{
	size_t size = sizeof(char);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadInt(int& data)
{
	size_t size = sizeof(int);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadUInt(unsigned int& data)
{
	size_t size = sizeof(unsigned int);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadShort(short& data)
{
	size_t size = sizeof(short);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadUShort(unsigned short& data)
{
	size_t size = sizeof(unsigned short);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadLong(long& data)
{
	size_t size = sizeof(long);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadULong(unsigned long& data)
{
	size_t size = sizeof(unsigned long);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadFloat(float& data)
{
	size_t size = sizeof(float);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadDouble(double& data)
{
	size_t size = sizeof(double);

	if (this->ReadData((void*)&data, size) == -1)
	{
		data = 0;
		return -1;
	}

	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadString(string& data)
{
	short size = 0;

	if (this->ReadShort(size) == -1)
		return -1;

	if (size <= 0)
		return -1;

	char* temp = new char[size];
	if (this->ReadData(temp, size) == -1)
	{
		delete temp;
		data = "";
		return -1;
	}

	data = temp;
	delete temp;

	this->m_nReadPos += sizeof(size);
	this->m_nReadPos += size;

	return 0;
}

int 
MByteBuffer::ReadUID(MUID& data)
{
	if (this->ReadULong(data.High) == -1)
		return -1;

	if (this->ReadULong(data.Low) == -1)
		return -1;

	this->m_nReadPos += sizeof(data.High)+sizeof(data.Low);

	return 0;
}

int 
MByteBuffer::ReadData(void* data, size_t size)
{
	if (size <= 0)
		return -1;

	if (m_nReadPos+size >= m_nMaxBufSize)
		size = m_nMaxBufSize-m_nReadPos;

	//char* temp_buffer = new char[size];

	memcpy(data, m_Buf+m_nReadPos, size);

	return 0;
}

bool SplitStrIP( const string& strIP, vector<BYTE>& vIP )
{
	if( strIP.empty() || (7 > strIP.length()) ) 
		return false;

	size_t a, b, c;
	char szPos1[ 4 ] = {0,};
	char szPos2[ 4 ] = {0,};
	char szPos3[ 4 ] = {0,};
	char szPos4[ 4 ] = {0,};

	a = strIP.find( "." );
	if( string::npos == a ) 
		return false;

	b = strIP.find( ".", a + 1 );
	if( string::npos == b ) 
		return false;

	c = strIP.find( ".", b + 1 );
	if( string::npos == c )
		return false;

	strncpy( szPos1, &strIP[0], a );
	strncpy( szPos2, &strIP[a + 1], b - a - 1 );
	strncpy( szPos3, &strIP[b + 1], c - b - 1 );
	strncpy( szPos4, &strIP[c + 1], strIP.length() - c - 1 );

	vIP.push_back( static_cast<BYTE>(atoi(szPos1)) );
	vIP.push_back( static_cast<BYTE>(atoi(szPos2)) );
	vIP.push_back( static_cast<BYTE>(atoi(szPos3)) );
	vIP.push_back( static_cast<BYTE>(atoi(szPos4)) );

	return true;
}



// http://dslweb.nwnexus.com/~ast/dload/guicon.htm
void RedirectIOToConsole()
{
	const WORD MAX_CONSOLE_LINES = 500;

	int hConHandle;
	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text

	GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		&coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	// point to console as well
	ios::sync_with_stdio();
}