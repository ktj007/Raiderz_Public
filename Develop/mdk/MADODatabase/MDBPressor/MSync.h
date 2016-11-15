#ifndef _MSYNC_H
#define _MSYNC_H

//#pragma once


class MCriticalSection 
{
private:
	CRITICAL_SECTION	m_cs;

public:
#ifdef _DEBUG
	MCriticalSection() : m_nCount(0) { InitializeCriticalSection(&m_cs); m_bUsableCS = true; }
	~MCriticalSection() { DeleteCriticalSection(&m_cs); m_bUsableCS = false; }
#else
	MCriticalSection()	{ InitializeCriticalSection(&m_cs); }
	~MCriticalSection() { DeleteCriticalSection(&m_cs); }
#endif

	void Lock()			{ EnterCriticalSection(&m_cs); }
	void Unlock()		{ LeaveCriticalSection(&m_cs); }

	class Guard
	{
	private:
		MCriticalSection& m_csSyncObject;

		// 객체를 복사하면 낭패이므로..
		Guard(const Guard& rhs);
		Guard &operator=(const Guard& rhs);

	public:
		explicit Guard(MCriticalSection & cs) : m_csSyncObject(cs)
		{
//			_ASSERT( m_csSyncObject.m_bUsableCS == true );
			m_csSyncObject.Lock();
			#ifdef _DEBUG
			++ m_csSyncObject.m_nCount;
			_ASSERT( m_csSyncObject.m_nCount <= 1);
			#endif

		}
		~Guard()
		{
			#ifdef _DEBUG
			-- m_csSyncObject.m_nCount;
			#endif
			_ASSERT( m_csSyncObject.m_nCount == 0 );
			m_csSyncObject.Unlock();
		}
	};

// by pok
#ifdef _DEBUG
	int  m_nCount;
	bool m_bUsableCS;
#endif

};

/// 클래스 단위 다중 쓰레드 동기화
template <class T>
class MClassLevelLockable
{
private:
	static MCriticalSection		m_stCS;
public:
	class Lock;
	friend class Lock;
	typedef volatile T VolatileType;

	MClassLevelLockable() {}

	class Lock
	{
	public:
		Lock(T& host) { MClassLevelLockable<T>::m_stCS.Lock(); }
		Lock(MClassLevelLockable<T> &) { MClassLevelLockable<T>::m_stCS.Lock(); }
		Lock()	{ MClassLevelLockable<T>::m_stCS.Lock(); }
		~Lock()	{ MClassLevelLockable<T>::m_stCS.Unlock(); }
	};
};

template<class T> MCriticalSection MClassLevelLockable<T>::m_stCS;


class MSignalEvent {
	HANDLE		m_hEvent;

public:
	MSignalEvent() { m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); }
	~MSignalEvent() { if (m_hEvent) { CloseHandle(m_hEvent); m_hEvent = NULL; } }
	HANDLE GetEvent() { return m_hEvent; }
	BOOL SetEvent() { return ::SetEvent(m_hEvent); }
	BOOL ResetEvent() { return ::ResetEvent(m_hEvent); }
};


// 어플리케이션이 동시에 두개뜨는것을 막거나 기다리게 도와주는 클래스
class MSingleRunController {
protected:
	char	m_szAppName[_MAX_DIR];
	HANDLE	m_Mutex;
public:
	//Usage : 	if (g_SingleRunController.Create(true) == false) return FALSE;
	MSingleRunController(const char* pszAppName)	{ 
		m_Mutex = NULL;
		strcpy_s(m_szAppName, pszAppName); 
	}
	virtual ~MSingleRunController()	{ 
		Destroy();
	}	
	bool Create(bool bWaitMode=false) {	// bMode=true(실행가능때까지 블럭대기), bMode==false(늦게 실행된 인스턴스 바로종료)
		if (bWaitMode == true) {
			int tmTotalWait = 0;
			while(true) {
				m_Mutex = CreateMutex(NULL, TRUE, m_szAppName);
				if (GetLastError() == ERROR_ALREADY_EXISTS)	{
					CloseHandle(m_Mutex);

					Sleep(100);
					tmTotalWait += 100;
					if (tmTotalWait >= 5*60*1000) // 5분이상은 강제종료
						return false;
				} else {
					return true;
				}
			}
		} else {
			m_Mutex = CreateMutex(NULL, TRUE, m_szAppName);
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				return false;
			else
				return true;
		}
	}
	void Destroy() {
		if (m_Mutex) {
			ReleaseMutex(m_Mutex);	
			m_Mutex = NULL;
		}
	}
};


#endif