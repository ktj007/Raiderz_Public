#ifndef _MMEM_POOL_H
#define _MMEM_POOL_H

#include "MSync.h"
#include <exception>
#include <assert.h>

#define NO_USE_MEMPOOLCOUNTER
//#define TEST_MEMPOOL

#ifndef CML2_EXPORT
#ifndef CML2_IMPORT
#ifndef NO_USE_MEMPOOLCOUNTER
	#include "MemPoolObjCounter.h"
#endif
#endif
#endif


#pragma push_macro ("new")
#ifdef new
#undef new
#endif

//////////////////////////////////////////////////////////////////////////
//
//	MemoryPool
//	* new, delete operator overload
//	* 메모리 할당관리
//		- 객체별로 하나의 MemoryPool 생성
//		- delete : 실제 delete하지 않고 memory pool에 저장
//		- new : 현재 비어 있는 메모리가 있으면 할당 else 실제 객체의 new를 호출
//
//	* 사용법
//		- MMemPool을 사용하는 객체는 MMemPool을 상속받는다
//		- 마지막에는 꼭 할당된 메모리를 해제하기 위해 xxx:release()를 호출한다
//
//	* 예
//
//	class CTest:public MMemPool<CTest>
//	{
//	public:
//		CTest();
//		virtual ~CTest();
//		...
//	};		
//
//
//	* 참고 
//		- new 한 뒤 delete하지 않은 메모리에 대한 책임을 지지 않는다(메모리 누수)
//		- new[]와 delete[]는 적용되지 않는다
//
//////////////////////////////////////////////////////////////////////////


template <class T, int OBJS_IN_A_BLOCK = 100>
class MMemPool
{
private:
	static MCriticalSectionSpinLock	_m_csLock;
	static unsigned char* _m_pFree;
	static unsigned char* _m_pChunkFirst;
	static unsigned char* _m_pChunkLast;
	static size_t _m_nCapacity;
	static size_t _m_nSize;

	static void allocate_block() 
	{ 
		assert(sizeof(T) >= sizeof(unsigned char*));

		unsigned char* prev = _m_pFree;

		// 청크의 마지막은 다음 청크의 포인터이다.
		_m_pFree = new unsigned char[sizeof(T) * OBJS_IN_A_BLOCK + sizeof(int)];
		unsigned char** curr = reinterpret_cast<unsigned char**>(_m_pFree);
		unsigned char* next = _m_pFree;
		for (int i = 0; i < OBJS_IN_A_BLOCK - 1; ++i) 
		{
			next += sizeof(T);
			*curr = next;
			curr = reinterpret_cast<unsigned char**>(next);
		}

		*curr = prev;

		unsigned char** new_chunk_block = reinterpret_cast<unsigned char**>(_m_pFree + (sizeof(T) * OBJS_IN_A_BLOCK));
		*new_chunk_block = 0;

		if (_m_nCapacity == 0)
		{
			_m_pChunkFirst = _m_pFree;
		}

		if (_m_pChunkLast != 0)
		{
			unsigned char** last_chunk_block = reinterpret_cast<unsigned char**>(_m_pChunkLast + (sizeof(T) * OBJS_IN_A_BLOCK));
			*last_chunk_block = _m_pFree;
		}

		_m_pChunkLast = _m_pFree;
		_m_nCapacity += OBJS_IN_A_BLOCK;
	}
protected:
#ifndef CML2_EXPORT
#ifndef CML2_IMPORT
#ifndef NO_USE_MEMPOOLCOUNTER
	// MMemPool 클래스의 접근을 도와 줄 클로져.
	class MemPoolClouser : public MemPoolObjCounter::IMemPool
	{
		long Count()
		{
			return T::TotalCount();
		}
	};
	static MemPoolClouser m_clouser;
#endif
#endif
#endif


public:
	// 클래스 특성 상 가상함수테이블이 생성되면 안되기 때문에 생성자나 소멸자를 만들어주면 안된다
// 	MMemPool()
// 	{
// 	}
// 
// 	~MMemPool() {}

	static void* operator new (size_t size) 
	{
		// Leaf 클래스만 MemPool을 사용할 수 있다. - birdkr
		assert(sizeof(T) == size);

		MCriticalSectionSpinLock::Guard guard(_m_csLock);

		if (_m_pFree == 0) 
		{
			allocate_block();

#ifndef CML2_EXPORT
#ifndef CML2_IMPORT
#ifndef NO_USE_MEMPOOLCOUNTER
			MemPoolObjCounter::Instance().Register<T>(&m_clouser);
#endif
#endif
#endif
		}
		unsigned char* p = _m_pFree;
		_m_pFree = *reinterpret_cast<unsigned char**>(p);

		++_m_nSize;

		return p;
	}
	static void* operator new (size_t size, LPCSTR lpszFileName, int nLine)
	{
		return MMemPool<T, OBJS_IN_A_BLOCK>::operator new (size);
	}
	static void* operator new (size_t size, int, LPCSTR lpszFileName, int nLine)
	{
		return MMemPool<T, OBJS_IN_A_BLOCK>::operator new (size);
	}

	static void operator delete (void* p) 
	{
		if (p == 0) return;

		MCriticalSectionSpinLock::Guard guard(_m_csLock);
		if (_m_nSize == 0) throw std::runtime_error("delete invalid pointer");

		*reinterpret_cast<unsigned char**>(p) = _m_pFree;
		_m_pFree = static_cast<unsigned char*>(p);

		--_m_nSize;
	}
	static void operator delete (void* p, LPCSTR lpszFileName, int nLine) 
	{
		MMemPool<T, OBJS_IN_A_BLOCK>::operator delete(p);
	}
	static void operator delete (void* p, int, LPCSTR lpszFileName, int nLine) 
	{
		MMemPool<T, OBJS_IN_A_BLOCK>::operator delete(p);
	}

	static void reserve(size_t size) 
	{
		MCriticalSectionSpinLock::Guard guard(_m_csLock);

		if (_m_nCapacity > size) return;

		size_t needed = size - _m_nCapacity;
		size_t count = needed / OBJS_IN_A_BLOCK;
		if (needed % OBJS_IN_A_BLOCK != 0) ++count;

		for (size_t i = 0; i < count; ++i) {
			allocate_block();
		}
	}

	// 구조상 size==0 이어야 release 함수를 실행할 수 있다.
	static void release()
	{
		MCriticalSectionSpinLock::Guard guard(_m_csLock);

		assert(_m_nSize == 0);
		if (_m_nSize != 0)
		{
			return;
		}

		unsigned char* pChunk = _m_pChunkFirst;

		while (pChunk != NULL)
		{
			unsigned char** ppNextChunk = reinterpret_cast<unsigned char**>(pChunk + (sizeof(T) * OBJS_IN_A_BLOCK));
			unsigned char* next = *ppNextChunk;
			delete [] pChunk;

			pChunk = next;
			_m_pChunkLast = pChunk;
		}

		_m_nCapacity = 0;
		_m_nSize = 0;
		_m_pChunkFirst = 0;
		_m_pChunkLast = 0;
		_m_pFree = 0;
	}

	static size_t capacity()	{ return _m_nCapacity; }
	static size_t TotalCount()		{ return _m_nSize; }
};

template <class T, int OBJS_IN_A_BLOCK>
size_t MMemPool<T, OBJS_IN_A_BLOCK>::_m_nCapacity = 0;

template <class T, int OBJS_IN_A_BLOCK>
size_t MMemPool<T, OBJS_IN_A_BLOCK>::_m_nSize = 0;

template <class T, int OBJS_IN_A_BLOCK>
unsigned char* MMemPool<T, OBJS_IN_A_BLOCK>::_m_pFree = 0;

template <class T, int OBJS_IN_A_BLOCK>
MCriticalSectionSpinLock MMemPool<T, OBJS_IN_A_BLOCK>::_m_csLock;

template <class T, int OBJS_IN_A_BLOCK>
unsigned char* MMemPool<T, OBJS_IN_A_BLOCK>::_m_pChunkFirst = 0;

template <class T, int OBJS_IN_A_BLOCK>
unsigned char* MMemPool<T, OBJS_IN_A_BLOCK>::_m_pChunkLast = 0;

#ifndef CML2_EXPORT
#ifndef CML2_IMPORT
#ifndef NO_USE_MEMPOOLCOUNTER
	template <class T, int OBJS_IN_A_BLOCK>
	typename MMemPool<T, OBJS_IN_A_BLOCK>::MemPoolClouser MMemPool<T, OBJS_IN_A_BLOCK>::m_clouser;
#endif
#endif
#endif



#ifdef TEST_MEMPOOL
template <class T>
class MTestMemPool : public MMemPool<T> {
	char* _mempool_buffer[2];
};
#else
template <class T>
class MTestMemPool {};
#endif

#pragma pop_macro("new")


#endif // _MMEM_POOL_H