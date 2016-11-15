#pragma once

#include <windows.h>

namespace rs3 {

/**
 Trail 이펙트에 쓰이는 원형 큐 -
 인터페이스는 CParticleArray와 비슷하다.

*/

template <class T>
class CParticleQueue
{
public:
	CParticleQueue()
	{
		m_pPool = NULL;
		m_uSize = 0;

		Init();
	}
	~CParticleQueue()
	{
		Release();
	}

	T* push(const T& value)
	{
		if ( m_uCount == m_uSize )
			return NULL;

		T *ptr = &m_pPool[m_nHeadIndex];
		*ptr = value;

		++m_nHeadIndex;
		m_nHeadIndex%=m_uSize;
		++m_uCount;

		return ptr;
	}

	T* peekTail()
	{
		if (empty())
			return NULL;

		return &m_pPool[m_nTailIndex];
	}

	T* peekHead()
	{
		if (empty())
			return NULL;
		
		int nIndex = (m_nHeadIndex-1);
		if ( nIndex < 0 )
			nIndex += m_uSize;

		return &m_pPool[nIndex];
	}

	void pop()
	{
		if (!empty())
		{
			++m_nTailIndex;
			m_nTailIndex %= m_uSize;
			--m_uCount;
		}
	}

	void pop(unsigned int n)
	{
		if (!empty() && n>0)
		{
			n = min(m_uCount, n);
			
			m_nTailIndex += n;
			m_nTailIndex %= m_uSize;
			m_uCount -= n;
		}
	}

	// count index 를 넣도록 함.
	T& operator[](unsigned int uIndex)
	{
		_ASSERT(m_pPool!=NULL);
		_ASSERT(uIndex<m_uCount);
		_ASSERT(uIndex>=0);

		return m_pPool[(m_nTailIndex +uIndex)%m_uSize];
	}

	void resize(unsigned int uSize)
	{
		Release();

		m_pPool = new T[uSize];
		m_uSize = uSize;

		Init();
	}

	unsigned int size()
	{
		return m_uSize;
	}

	unsigned int count()
	{
		return m_uCount;
	}

	bool empty()
	{
		return (m_uCount == 0);
	}

	int beginIndex()
	{
		return m_nHeadIndex;
	}

	int endIndex()
	{
		return m_nTailIndex;
	}

	void clear()
	{
		Init();
	}

protected:
	T* m_pPool;

	unsigned int m_uSize;

	int m_nHeadIndex;
	int m_nTailIndex;
	unsigned int m_uCount;

	void Init()
	{
		m_nHeadIndex = 0;
		m_nTailIndex = 0;
		m_uCount = 0;
	}

	void Release()
	{
		if ( m_pPool )
		{
			delete [] m_pPool;
			m_pPool = NULL;
		}
	}
};

/**

파티클을 위해 사용되는 클래스 CParticleArray
- 전체 공간을 m_pMemPool 에 만들고, 각각 요소를 포인터 배열 m_ppActive에 넣었다.
- m_ppActive는 m_uActiveCount 까지 사용되는 공간의 포인터를 담고 있다.
- 그러기 위해서, 삭제할 때 빈 공간이 없도록 끝의 요소를 끌어와 채운다.

*/

template <typename T> class CParticleArray
{
protected:
	T *m_pMemPool;
	T **m_ppActive;

	unsigned int m_uNum;
	unsigned int m_uActiveCount;

	void Init()
	{
		m_pMemPool = new T[m_uNum];
		if ( m_pMemPool == NULL )
			return;

		m_ppActive = new T*[m_uNum];
		memset(m_ppActive, 0, sizeof(T*)*m_uNum);

		for ( unsigned int ui = 0; ui<m_uNum; ++ui )
			m_ppActive[ui] = &m_pMemPool[ui];

		m_uActiveCount = 0;
	}
	void Release()
	{
		if ( m_ppActive )
		{
			delete [] m_ppActive;
			m_ppActive = NULL;
		}
		if ( m_pMemPool )
		{
			delete [] m_pMemPool;
			m_pMemPool = NULL;
		}

		m_uNum = m_uActiveCount = 0;
	}
public:
	CParticleArray() : m_pMemPool(NULL), m_ppActive(NULL), m_uNum(0), m_uActiveCount(0) {}
	CParticleArray(unsigned int uNum) : m_pMemPool(NULL), m_ppActive(NULL), m_uNum(uNum)
	{
		Init();
	}

	~CParticleArray()
	{
		Release();
	}

	T& operator[](unsigned int uIndex)
	{
		// 즉 Index밖의 것은 건드리며 안됨.
		assert(uIndex<m_uActiveCount);

		return *m_ppActive[uIndex];
	}

	T erase(unsigned int uIndex)
	{
		assert(uIndex<m_uActiveCount);
		assert(m_uActiveCount>0);

		// PackArray와 같은 구조이나, 간접 Point Indexing을 이용 조금 더 최적화 시켰다.
		unsigned int uLastIndex = m_uActiveCount - 1;
		T* ptr = m_ppActive[uIndex];
		m_uActiveCount--;

		if ( uLastIndex == uIndex )
			return *ptr;

		m_ppActive[uIndex] = m_ppActive[uLastIndex];
		m_ppActive[uLastIndex] = ptr;

		return *ptr;
	}

	void resize(unsigned int uNum)
	{
		Release();

		m_uNum = uNum;
		Init();
	}

	void reset()
	{
		m_uActiveCount = 0;
	}

	T* getAddPtr()	// 공간 하나 늘이고, 그 공간의 포인터를 얻는다.
	{
		if ( m_uActiveCount == m_uNum )
			return NULL;

		T* ptr = m_ppActive[m_uActiveCount];
		++m_uActiveCount;

		return ptr;
	}

	unsigned int count()
	{
		return m_uActiveCount;
	}

	unsigned int size()
	{
		return m_uNum;
	}

	T* getPtr(unsigned int uIndex)
	{
		// 즉 Index밖의 것은 건드리며 안됨.
		assert(uIndex<m_uActiveCount);
		return m_ppActive[uIndex];
	}

	T** getActiveBeginPtr()
	{
		return &m_ppActive[0];
	}

	T** getActiveEndPtr()
	{
		return &m_ppActive[m_uActiveCount];
	}
};

}
