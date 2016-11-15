#ifndef _MUTIL_H
#define _MUTIL_H

#include "MSync.h"
#include <vector>
using namespace std;

// 10진수값에 대한 집합형
#define SetBitSet(sets, item)		(sets |= (1 << item))
#define ClearBitSet(sets, item)		(sets &= ~(1 << item))
#define CheckBitSet(sets, item)		(sets & (1 << item))

/* 사용법
enum Item
{
	Foo1 = 1,
	Foo2 = 2,
	FooMax
};

unsigned long int sets = 0;
SetBitSet(sets, Foo1);
if (CheckBitSet(sets, Foo1)) extr;

*/


/** 멀티쓰레드에 안전한 원형버퍼

	- 남기룡(2006/02/22)
*/
class MRingBuffer
{
private:
	static const int	DEFAULT_RINGBUFFER_SIZE = 8192;		///< 기본 버퍼크기는 8k이다.

	char*				m_pBuf;								///< 실제 버퍼
	char*				m_pStartData;						///< 데이터 시작 위치
	char*				m_pEndBuf;							///< 버퍼 끝 위치
	char*				m_pCursor;							///< 데이터 끝 위치
	int					m_nBufSize;							///< 버퍼 사이즈
	int					m_nDataSize;						///< 버퍼에 존재하는 데이터 사이즈

	MCriticalSection	m_csLock;							///< 쓰레드에 안전하기 위해서
public:
						MRingBuffer();						///< 기본 버퍼크기로 생성
						MRingBuffer(int nBufSize);			///< 버퍼 크기를 정하여 생성
	virtual				~MRingBuffer();						///< 소멸자
	void				Reserve(int nBufSize);				///< 버퍼크기를 재할당. 버퍼안에 있던 데이터는 초기화된다.
	bool				Push(void* pBuf, int nSize);		///< 버퍼에 데이터를 넣는다.
	bool				Pop(void* pBuf, int nSize);			///< 버퍼에서 데이터를 뺀다.
	bool				ReadFront(void* pBuf, int nSize);	///< 버퍼의 데이터 앞부분을 읽는다. 빼진 않음(주의).
	int					GetSize();							///< 버퍼에 들어있는 데이터 크기를 반환한다.
	int					GetCapacity();						///< 버퍼 전체 크기를 반환한다.
};




// 바이트 버퍼
//		:패킷 마샬링용으로 구현되었음
//
//  [7/25/2006 pyo] 시작.

#include "MUID.h"
#include <string>
using namespace std;

#define DEFAULT_BYTE_BUFFER_SIZE 512

class MByteBuffer
{
private:
	char*		m_Buf;
	size_t		m_nMaxBufSize,
		m_nWritePos,
		m_nReadPos;
public:
	MByteBuffer(size_t nSize=0):m_Buf(0), m_nMaxBufSize(nSize), m_nWritePos(0), m_nReadPos(0) { this->SetMaxSize(); }
	~MByteBuffer() {}

	void SetMaxSize(size_t nSize=0);

	char*	GetBuffer();
	char*&	GetBufferRef();
	void	SetBuffer(char* buf);

	void	WritePos(size_t pos);
	size_t	WritePos();
	void	ReadPos(size_t pos);
	size_t	ReadPos();

	int WriteChar(const char data);
	int WriteInt(const int data);
	int WriteUInt(const unsigned int data);
	int WriteShort(const short data);
	int WriteUShort(const unsigned short data);
	int WriteLong(const long data);
	int WriteULong(const unsigned long data);
	int WriteFloat(const float data);
	int WriteDouble(const double data);
	int WriteString(const string data);
	int WriteUID(const MUID data);
	int WriteData(const void* data, size_t size);

	int ReadChar(char& data);
	int ReadInt(int& data);
	int ReadUInt(unsigned int& data);
	int ReadShort(short& data);
	int ReadUShort(unsigned short& data);
	int ReadLong(long& data);
	int ReadULong(unsigned long& data);
	int ReadFloat(float& data);
	int ReadDouble(double& data);
	int ReadString(string& data);
	int ReadUID(MUID& data);
	int ReadData(void* data, size_t size);

	friend int operator<<(MByteBuffer& lhs, char data);
	friend int operator<<(MByteBuffer& lhs, int data);
	friend int operator<<(MByteBuffer& lhs, unsigned int data);
	friend int operator<<(MByteBuffer& lhs, short data);
	friend int operator<<(MByteBuffer& lhs, unsigned short data);
	friend int operator<<(MByteBuffer& lhs, long data);
	friend int operator<<(MByteBuffer& lhs, unsigned long data);
	friend int operator<<(MByteBuffer& lhs, float data);
	friend int operator<<(MByteBuffer& lhs, double data);
	friend int operator<<(MByteBuffer& lhs, string data);
	friend int operator<<(MByteBuffer& lhs, MUID data);

	friend int operator>>(MByteBuffer& lhs, char& data);
	friend int operator>>(MByteBuffer& lhs, int& data);
	friend int operator>>(MByteBuffer& lhs, unsigned int& data);
	friend int operator>>(MByteBuffer& lhs, short& data);
	friend int operator>>(MByteBuffer& lhs, unsigned short& data);
	friend int operator>>(MByteBuffer& lhs, long& data);
	friend int operator>>(MByteBuffer& lhs, unsigned long& data);
	friend int operator>>(MByteBuffer& lhs, float& data);
	friend int operator>>(MByteBuffer& lhs, string& data);
	friend int operator>>(MByteBuffer& lhs, MUID& data);

private:
	void AutoSetMaxSizeImpl(size_t nSize);
};

bool SplitStrIP( const string& strIP, vector<BYTE>& vIP );

/// Win32 Windows 프로그램에서 콘솔을 따로 띄우기
void RedirectIOToConsole();

// 타이핑 줄이고 싶어요
#define scast static_cast
#define rcast reinterpret_cast
#define ccast const_cast
#define dcast dynamic_cast

// 자동 삭제 도우미
template <class T>
class AutoDelete
{
public:
	AutoDelete(T* const obj) : m_obj(obj), m_delete(true) {}
	~AutoDelete() { if (true == m_delete) delete m_obj; }

	void Cancel() { m_delete = false; }

private:
	bool		m_delete;
	T* const	m_obj;
};

#endif