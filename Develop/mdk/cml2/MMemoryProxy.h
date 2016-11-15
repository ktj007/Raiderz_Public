#pragma once

#include "MMath.h"

#define WRAPING_TIME 1000	//메모리 블럭을 바꿔주는 시간...1초에 한번씩...
#define NUM_OF_MEMORY 20	//하나의 메모리 블럭에 잡히는 메모리 개수..

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif


template <typename _data>
struct MMemoryBlock
{
	_data * m_pMemory[NUM_OF_MEMORY];
};

template <typename _data>
class MMemoryProxy
{
	MMemoryBlock<_data> * MB1;
	MMemoryBlock<_data> * MB2;
	MMemoryBlock<_data> * MB3;

	MMemoryBlock<_data> * MB;

	_data m_initial;

	DWORD m_dwCurrentTime;
	int m_nCorrectDataIndex;
	int m_nIndex;
	
public:
	MMemoryProxy()
	{
		Init();
	}

	MMemoryProxy(_data init)
	{
		m_initial = init;
		m_nIndex = 0;
		m_nCorrectDataIndex = 0;
		Init();
	}

	~MMemoryProxy()
	{
		Destroy();
	}

	void Init()
	{
		MB1 = new MMemoryBlock<_data>();
		MB2 = new MMemoryBlock<_data>();
		MB3 = new MMemoryBlock<_data>();
		
		for(int i=0; i<NUM_OF_MEMORY; i++)
		{
			MB1->m_pMemory[i] = new _data;
			*(MB1->m_pMemory[i]) = m_initial;

			MB2->m_pMemory[i] = new _data;
			*(MB2->m_pMemory[i]) = m_initial;

			MB3->m_pMemory[i] = new _data;
			*(MB3->m_pMemory[i]) = m_initial;
		}
		
		m_nIndex = 1;
		MB = MB1;
		m_dwCurrentTime = GetTickCount();

		SetRandomData( m_initial);
	}
	void Destroy()
	{
		for(int i=0; i<NUM_OF_MEMORY; i++)
		{
			SAFE_DELETE(MB1->m_pMemory[i]);
			SAFE_DELETE(MB2->m_pMemory[i]);
			SAFE_DELETE(MB3->m_pMemory[i]);
		}

		SAFE_DELETE(MB1);
		SAFE_DELETE(MB2);
		SAFE_DELETE(MB3);
	}

	void SetData(const _data & data)
	{
		SetRandomData(data);
	}

	_data & GetData()
	{
		return * MB->m_pMemory[m_nCorrectDataIndex];
	}

	void SetRandomData(const _data & data)
	{
		m_nCorrectDataIndex = RandomNumber(0, NUM_OF_MEMORY-1);
		*(MB->m_pMemory[m_nCorrectDataIndex]) = data;
	}
	void SetWarpingAdd(DWORD tick)
	{
		if( tick - m_dwCurrentTime > WRAPING_TIME)
		{
			int i = RandomNumber(0, NUM_OF_MEMORY-1);
			if(m_nIndex == 1)
			{
				*(MB2->m_pMemory[i]) = *(MB1->m_pMemory[m_nCorrectDataIndex]);
				MB = MB2;
				*(MB1->m_pMemory[m_nCorrectDataIndex]) = m_initial;	//흔적 안남기기
				m_nCorrectDataIndex = i;
			}
			else if(m_nIndex == 2)
			{
				*(MB3->m_pMemory[i]) = *(MB2->m_pMemory[m_nCorrectDataIndex]);
				MB = MB3;
				*(MB2->m_pMemory[m_nCorrectDataIndex]) = m_initial;
				m_nCorrectDataIndex = i;
			}
			else
			{
				*(MB1->m_pMemory[i]) = *(MB3->m_pMemory[m_nCorrectDataIndex]);
				MB = MB1;
				*(MB3->m_pMemory[m_nCorrectDataIndex]) = m_initial;
				m_nCorrectDataIndex = i;
			}
			m_nIndex++;
			if(m_nIndex>3)
				m_nIndex = 1;

			m_dwCurrentTime = tick;
		}	
	}
};