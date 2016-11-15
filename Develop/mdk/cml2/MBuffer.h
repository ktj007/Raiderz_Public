#pragma once

#include "RLib.h"

class CML2_API MBuffer
{
public:
	MBuffer(void);
	~MBuffer(void);

public:
	//할당/해제
	void	AllocateBuffer(int Size);
	void	FreeBuffer(void);
	void	SetBuffer(char* pSource,int Size);

	//초기화
	void	Reset(void);

	//쓰기/읽기
	int		Append(char* pSource,int Size);
	void	Read(char* pDest,int ReadSize);

	//접근자들
	int		GetBufferSize(void)		{	return m_BufferSize;	}	// 버퍼에 할당된 저장공간의 크기
	int		GetDataSize(void)		{	return m_SavePointer;	}	// 버퍼에 저장된 데이타의 크기
	char*	GetBufferPointer(void)	{	return m_pBuffer;		}

public:
	int		GetEmptySize(void)			{	return m_BufferSize - m_SavePointer;			}
	int		GetRemainReadSize(void)		{	return m_SavePointer - m_ReadPointer;	}

private:
	char*	m_pBuffer;
	int		m_BufferSize;
	int		m_SavePointer;	//저장을 시작해야할 위치 ( 위치는 0 부터 시작 ), 이값이 4라면 0~3까지는 버퍼에 자료가 저장되었다는 의미
	int		m_ReadPointer;	//읽기를 시작해야할 위치 ( 위치는 0 부터 시작 ) 
};