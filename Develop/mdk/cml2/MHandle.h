#ifndef _MHANDLE
#define _MHANDLE

// 2005.7.22 by dubble.
// handle 의 templete
// 상수시간에 insert/delete가 가능하며 사이즈가 넘어가면 재할당하는데 시간이 많이 걸릴수 있다.

// thread 에 safe하지 않으므로 사용에 주의를 요함.

#include <cassert>
#include <list>
#include <vector>
//#include <set>

using namespace std;

#define INITIAL_SIZE		16			// 초기 예약 개수

struct MHandleInfo {
	bool m_bUsed;
	list<int>::iterator m_position;
};

template <class _TYPE, int _HANDLE_BASE = 1>

class MHandle {

protected:
	vector<MHandleInfo> m_infoArray;
	_TYPE		*pArray;		// 실제 메모리 블럭
	unsigned int m_nCapasity;	// 전체 저장공간의 수
	unsigned int m_nCount;		// 아이템이 들어있는 공간의 수
	list<int>	m_unused;		// 아직 사용하지 않은 (인덱스)들의 리스트
	list<int>	m_used;			// 사용된 (핸들)의 리스트
//	set<int>	m_used;

private:
	bool IsUsed(const unsigned int nIndex) const {
		return m_infoArray[nIndex].m_bUsed;
//		return (m_used.find(nIndex)!=m_used.end());
	};

public:
	MHandle(){
		m_nCount = m_nCapasity = 0;
		pArray = NULL;

		if(INITIAL_SIZE) Reserve(INITIAL_SIZE);
	}
	
	~MHandle(){
		assert(m_nCount==0);
//		assert(m_used.size()==0);
		free(pArray);
	}

	_TYPE& operator [] (const unsigned int nHandle) const { return Get(nHandle); }

	_TYPE& Get(const unsigned int nHandle) const {
		int nIndex = nHandle - _HANDLE_BASE;
		assert(IsUsed(nIndex));
		return pArray[nIndex];
	}

//	_TYPE *GetArray() const { return pArray; }
	
	unsigned int GetCount() const { return m_nCount; }
	unsigned int GetCapasity() const { return m_nCapasity; }

	unsigned int Add(const _TYPE object){
		if (m_nCount >= m_nCapasity){
			Reserve(m_nCapasity*2);
		}

		int nUnusedIndex = m_unused.front();
		m_unused.pop_front();

		m_used.push_back(nUnusedIndex + _HANDLE_BASE);
	
		assert(!IsUsed(nUnusedIndex));
		m_infoArray[nUnusedIndex].m_bUsed = true;
		m_infoArray[nUnusedIndex].m_position = --m_used.end();
		pArray[nUnusedIndex] = object; // memcpy 를 써야하나.
		m_nCount++;

//		m_used.insert(nUnused);

		return nUnusedIndex + _HANDLE_BASE;
	}

	void Remove(const unsigned int nHandle){
		int nIndex = nHandle - _HANDLE_BASE;
		if(IsUsed(nIndex))
		{
//			m_used.erase(nIndex);
			m_infoArray[nIndex].m_bUsed = false;
			m_used.erase(m_infoArray[nIndex].m_position);

			m_nCount--;
			m_unused.push_front(nIndex);
		}else {
			// 잘못된핸들
			assert(false);
		}
	}

	void Reserve(const unsigned int newCapasity){
		assert(newCapasity>=m_nCapasity);
		pArray = (_TYPE*) realloc(pArray, newCapasity * sizeof(_TYPE) );
		for(unsigned int i=m_nCapasity;i<newCapasity;i++) {
			m_unused.push_back(i);
			MHandleInfo info;
			info.m_bUsed = false;
			m_infoArray.push_back(info);
		}
		m_nCapasity = newCapasity;
	}

//	void Clear(){ m_nCount = 0; }

	int GetFirst() {
//        return *m_used.begin() + _HANDLE_BASE;
		return m_used.front();
	}

	const list<int>& GetUsedHandleList() { return m_used; }
};


#endif