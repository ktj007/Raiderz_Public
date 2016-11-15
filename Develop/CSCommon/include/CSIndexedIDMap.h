#pragma once

#include "MTypes.h"
#include "CSDef.h"
#include <deque>

template <class T, int OBJ_BLOCK_SIZE = 1000>
class CSIndexedIDMap
{
protected:
	UIID				m_nSeed;		///< UID 초기값
	UIID				m_nTop;
	std::vector<T>		m_Vector;
	std::deque<UIID>	m_NotUsedQueue;

	void IncreaseVectorBlockSize()
	{
		size_t nSize = m_nTop + OBJ_BLOCK_SIZE;
		m_Vector.resize(nSize);
	}

	UIID PushBack( T uid )
	{
		_ASSERT(m_nTop < USHRT_MAX-1);
		if (m_nTop >= USHRT_MAX-1)
		{
			return UIID_INVALID;
		}

		if (m_nTop >= m_Vector.size())
		{
			IncreaseVectorBlockSize();
		}

		const UIID nUIID = m_nTop + 1;		// uiid는 1부터 시작

		m_Vector[m_nTop] = uid;
		m_nTop++;

		return (m_nSeed + nUIID);
	}

	UIID PushFromNotUsedQueue( T uid )
	{
		UIID nUIID = *m_NotUsedQueue.begin();

		m_NotUsedQueue.pop_front();

		int nVecIndex = nUIID - 1;

		m_Vector[nVecIndex] = uid;

		return (m_nSeed + nUIID);
	}
public:
	CSIndexedIDMap(UIID nSeed=0) : m_nTop(0), m_nSeed(nSeed) {}
	virtual ~CSIndexedIDMap() {}
	virtual void Clear()
	{
		m_nTop = 0;
		m_Vector.clear();
		m_NotUsedQueue.clear();
	}

	void Reserve(size_t nSize)
	{
		m_Vector.resize(nSize);
	}

	T Find(UIID nUIID)
	{
		nUIID = nUIID - m_nSeed;

		const int nVecIndex = (int)(nUIID) - 1;
		if (nVecIndex < 0 || nVecIndex >= (int)m_Vector.size()) return 0;

		return m_Vector[nVecIndex];
	}

	UIID Add(T uid)
	{
		_ASSERT(uid != 0);

		if (m_NotUsedQueue.empty() == false)
		{
			return PushFromNotUsedQueue(uid);
		}

		return PushBack(uid);
	}

	void Del(UIID nUIID)
	{
		nUIID = nUIID - m_nSeed;

		int nVecIndex = (int)(nUIID) - 1;
		if (nVecIndex < 0 || nVecIndex >= (int)m_Vector.size()) return;

		if (m_Vector[nVecIndex] == 0) return;

		m_NotUsedQueue.push_back(nUIID);
		m_Vector[nVecIndex] = 0;
	}
};

