#pragma once

#include <vector>
#include <list>
#include "RParticle.h"

namespace rs3 {

// 핸들 기반 파티클 풀. MHandle을 약간 변형.
class RParticlePool
{
	struct RParticlePoolHandleInfo
	{
		bool m_bUsed;
		list<unsigned int>::iterator m_position;
	};

protected:
	vector<RParticlePoolHandleInfo> m_infoArray;
	RParticle*			pArray;		// 실제 메모리 블럭
	unsigned int		m_nCapacity;// 전체 저장공간의 수
	unsigned int		m_nCount;	// 아이템이 들어있는 공간의 수
	list<unsigned int>	m_unused;	// 아직 사용하지 않은 (인덱스)들의 리스트
	list<unsigned int>	m_used;		// 사용된 (핸들)의 리스트

private:
	bool IsUsed(unsigned int nIndex) const
	{
		return m_infoArray[nIndex].m_bUsed;
	};

public:
	RParticlePool(unsigned int reserveCount)
	{
		m_nCount = 0;
		m_nCapacity = 0;
		pArray = NULL;

		bool bSuccess = Reserve(reserveCount);
		assert(bSuccess);
	}

	~RParticlePool()
	{
		assert(m_nCount==0);

		while( GetCount() )
			Remove(GetFirst());

		free(pArray);
	}

	RParticle* Get(unsigned int nHandle) const
	{
		assert(IsUsed(nHandle));
		return &pArray[nHandle];
	}

	unsigned int GetNew()
	{
		if(m_nCount >= m_nCapacity)
		{
			bool bSuccess = Reserve(m_nCapacity*2);
			assert(bSuccess);
			//if( !bSuccess )
			//	return -1;
		}

		int nUnusedIndex = m_unused.front();
		m_unused.pop_front();

		m_used.push_back(nUnusedIndex);

		assert(!IsUsed(nUnusedIndex));
		m_infoArray[nUnusedIndex].m_bUsed = true;
		m_infoArray[nUnusedIndex].m_position = --m_used.end();
		m_nCount++;

		pArray[nUnusedIndex].Clear();

		return nUnusedIndex;
	}

	unsigned int GetCount() const { return m_nCount; }
	unsigned int GetCapacity() const { return m_nCapacity; }

	void Remove(unsigned int nHandle)
	{
		assert(IsUsed(nHandle));

		m_infoArray[nHandle].m_bUsed = false;
		m_used.erase(m_infoArray[nHandle].m_position);

		m_nCount--;
		m_unused.push_front(nHandle);

		pArray[nHandle].Destroy();
	}

	bool Reserve(unsigned int newCapacity)
	{
		assert(newCapacity>=m_nCapacity);

		void* pNewArray = realloc(pArray, newCapacity * sizeof(RParticle));
		if( pNewArray == NULL )
			return false;

		pArray = (RParticle*)pNewArray;
		for(unsigned int i=m_nCapacity;i<newCapacity;i++)
		{
			m_unused.push_back(i);
			RParticlePoolHandleInfo info;
			info.m_bUsed = false;
			m_infoArray.push_back(info);
		}
		m_nCapacity = newCapacity;
		return true;
	}

	unsigned int GetFirst()
	{
		return m_used.front();
	}
};

}
