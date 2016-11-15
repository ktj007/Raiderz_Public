#ifndef _MARRAY_H
#define _MARRAY_H

// 2006.3.2 dubble
// std::vector 와 같으나 clear 시에 capacity 를 초기화 하지 않는다.

// thread 에 safe하지 않으므로 사용에 주의를 요함.

#include <cassert>

using namespace std;

#define INITIAL_SIZE		16			// 초기 예약 개수

template <class _TYPE>

class MArray {

protected:
	_TYPE *pArray;					// 실제 메모리 블럭
	unsigned int m_nCapasity;		// 전체 저장공간의 수
	unsigned int m_nCount;			// 아이템이 들어있는 공간의 수

public:
	MArray(){
		m_nCount = m_nCapasity = 0;
		pArray = NULL;

		if(INITIAL_SIZE) Reserve(INITIAL_SIZE);
	}
	
	~MArray(){
		free(pArray);
	}

	_TYPE &operator [] (const unsigned int index) const {
		return pArray[index];
	}

	_TYPE & Get(const unsigned int index) const {
		return pArray[index];
	}

	_TYPE *GetMArray() const { return pArray; }
	
	unsigned int GetCount() const { return m_nCount; }
	unsigned int GetCapasity() const { return m_nCapasity; }

	unsigned int Add(const _TYPE object){
		if (m_nCount >= m_nCapasity){
			Reserve(m_nCapasity*2);
		}

		pArray[m_nCount++] = object;
		return m_nCount-1;
	}

//	void Remove(const unsigned int index)	// 꼭 필요하면 만들되 웬만하면 쓰지맙시다

	void Reserve(const unsigned int newCapasity){
		assert(newCapasity>=m_nCapasity);
		m_nCapasity = newCapasity;
		pArray = (_TYPE *) realloc(pArray, m_nCapasity * sizeof(_TYPE));		
	}

	void Clear(){ m_nCount = 0; }
};


#endif