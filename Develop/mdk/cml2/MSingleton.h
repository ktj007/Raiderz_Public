#ifndef _MSINGLETON_H
#define _MSINGLETON_H

#include <cassert>

template <typename T> class MSingleton
{
    static T* m_pSingleton;

public:
    MSingleton( void )
    {
        assert( !m_pSingleton );

		/* 
		다중상속을 받았을때 포인터가 바르지 않은 문제를 해결한 싱클톤 템플릿

		gpg 에 나온 방법, VS6 이하버전용
        int offset = (int)((T*)1) - (int)(MSingleton <T>*)((T*)1);
        m_pSingleton = (T*)((int)this + offset);
		*/
        
		// 이것이 표준에 가까운 방법인듯.
        m_pSingleton = static_cast<T *>(this); 

	}
   ~MSingleton( void )
        {  assert( m_pSingleton );  m_pSingleton = 0;  }
    static T& GetInstance( void )
        {  assert( m_pSingleton );  return ( *m_pSingleton );  }
    static T* GetInstancePtr( void )
        {  return ( m_pSingleton );  }
};

template <typename T> T* MSingleton <T>::m_pSingleton = 0;

//////////////////////////////////////////////////////////////////////////
// _num 갯수만큼 인스턴스를 제한하는 싱글턴
//
template <typename T, int _num = 1> class MSingletonRestricted
{
	static int m_nCount;

public:
	MSingletonRestricted( void )
	{
		++m_nCount;
		assert( m_nCount <= _num );
	}
	~MSingletonRestricted( void )
	{
		--m_nCount;
		assert( m_nCount >= 0 );
	}
};

template <typename T, int _num > int MSingletonRestricted<T, _num>::m_nCount = 0;


#endif