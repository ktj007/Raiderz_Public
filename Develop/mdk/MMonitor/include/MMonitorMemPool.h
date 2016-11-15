#ifndef _MMONITORMEMPOOL
#define _MMONITORMEMPOOL


// CML에 있는 MemPool.h와 같음.
// MMonitor프로젝트에서만 사용해야 함.
// MMonitor는 다른 프로젝트와 독립적이어야 해서 따로 복사함. - by SungE 2007-06-07




#include "Windows.h"
#include "Winbase.h"
// #include "MDebug.h"
#include "assert.h"


namespace mmonitor {


#define InitMonitorMemPool(T)		MMonitorMemPool<T>::_InitCS()
#define UninitMonitorMemPool(T)		MMonitorMemPool<T>::_DeleteCS();
#define ReleaseMonitorMemPool(T)	MMonitorMemPool<T>::_Release();

template< typename T >
class MMonitorMemPool
{
protected:
	static T*	m_list;
	T*			m_next;

	// Multi Thread에서 메모리를 보호하기 위해
	static CRITICAL_SECTION m_csLock;

public:
	static void	_InitCS()
	{
		InitializeCriticalSection( &m_csLock );
	}

	static void	_DeleteCS()
	{
		DeleteCriticalSection( &m_csLock );
	}
	static void	_Release();

public:
	static void* operator new( size_t size_ );
	static void  operator delete( void* deadObject_, size_t size_ );
public:
	MMonitorMemPool(void)	{};
	~MMonitorMemPool(void)	{};
};

// new
template<typename T>
void* MMonitorMemPool<T>::operator new( size_t size_ )
{
	T* instance;
	EnterCriticalSection( &m_csLock );

	if( m_list != NULL )
	{
		instance	= m_list;
		m_list	= m_list->m_next;
	}
	else
	{
		instance = (T*)::operator new(size_);
	}

	LeaveCriticalSection( &m_csLock );

#ifdef _DEBUG
	if(size_ != sizeof(*instance))
		assert(0);
#endif

	return instance;
}

// delete
template<typename T>
void MMonitorMemPool<T>::operator delete( void* deadObject_, size_t size_ )
{
	EnterCriticalSection( &m_csLock );

	((T*)deadObject_)->m_next	= m_list;
	m_list	= (T*)deadObject_;

	LeaveCriticalSection( &m_csLock );
}

// Release
// 현재 할당되어 있지 않은 메모리 해제~~
template<typename T>
void MMonitorMemPool<T>::_Release()
{
	// KeeperManager(PatchInterface)는 여러개의 클라이언트를 가질수 있음.
	//  2개이상의 클라이언트가 종료될때 이부분이 중복 호출이 되어, 
	//  유효하지 않은 m_csLock에 크리티컬섹션을 설정하여 예외가 발생함.
	//  리스트의 해더 주소가 '0'이 아닐경우만 수행하도록 수정. - by 추교성.
	if( NULL != m_list ) 
	{
		EnterCriticalSection( &m_csLock );

		T* pInstace		= m_list;
		while( pInstace != NULL )
		{
			pInstace	= m_list->m_next;
			::operator delete( m_list );
			m_list	= pInstace;
		}

		LeaveCriticalSection( &m_csLock );
	}
}

template<typename T> CRITICAL_SECTION MMonitorMemPool<T>::m_csLock;
template<typename T> T* MMonitorMemPool<T>::m_list;


////////////////////////////////////////////////////////////////////////////
// 쓰레드 고려 안하는~
////////////////////////////////////////////////////////////////////////////

template < typename T >
class MMonitorMemPoolSm
{
protected:
	static T*	m_list;
	T*			m_next;

public:
	static void* operator new( size_t size_ );
	static void  operator delete( void* deadObject_, size_t size_ );

	static void _Release();

public:
	MMonitorMemPoolSm(void) {};
	~MMonitorMemPoolSm(void) {};
};


template<typename T>
void* MMonitorMemPoolSm<T>::operator new( size_t size_ )
{
	T* instance;

	if( m_list != NULL ) {
		instance = m_list;
		m_list = m_list->m_next;
	} else {
		instance = (T*)::operator new(size_);
	}

#ifdef _DEBUG
	if(size_ != sizeof(*instance))
		assert(0);
#endif

	return instance;
}

template<typename T>
void MMonitorMemPoolSm<T>::operator delete( void* deadObject_, size_t size_ )
{
	((T*)deadObject_)->m_next = m_list;
	m_list	= (T*)deadObject_;
}



// 현재 할당되어 있지 않은 메모리 해제~~
template<typename T>
void MMonitorMemPoolSm<T>::_Release()
{
	T* pInstace	= m_list;
	while( pInstace != NULL ) {
		pInstace = m_list->m_next;
		::operator delete( m_list );
		m_list = pInstace;
	}
}

/////////////////////////////////////////////////////////////////////////

template<typename T> T* MMonitorMemPoolSm<T>::m_list;

}

#endif