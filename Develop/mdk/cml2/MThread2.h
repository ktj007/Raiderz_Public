#ifndef _MTHREAD2_H
#define _MTHREAD2_H

#include <windows.h>
#include <WinBase.h>
#include <process.h>
#include <cassert>
#include <intrin.h>
#include "RLib.h"

namespace cml2 {

// MThread /////////////////////////
class CML2_API MThread
{
private:
	HANDLE			m_Handle;
	bool			m_bFreeOnTerminate;
	bool			m_bTerminated;
	bool			m_bFinished;
	int				m_nReturnValue;
	unsigned int	m_nThreadID;
private:
	static unsigned int WINAPI ThreadProc(void *data);

	void SetFinished(bool value);
	bool GetFinished(void) const;
protected:
	bool Terminated(void) const;
	void SetThreadName(const char *name);
	void SetReturnValue(int value);
	int GetReturnValue(void) const;
protected:
	virtual void Run(void) = 0;
public:
	MThread(void);
	virtual ~MThread(void);

	bool Start(void);
	void Stop(void);

	void Terminate(void);
	bool Wait(unsigned int timeout = INFINITE);

	void SetFreeOnTerminate(bool value);		/// Terminate시 알아서 Thread를 delete 시킬지 여부 설정
	bool GetFreeOnTerminate(void) const;

	HANDLE GetHandle(void) const;
	unsigned int GetThreadID(void) const;
};


// MNonCopyable /////////////////////////
class MNonCopyable
{
protected:
	MNonCopyable(void)
	{}
	~MNonCopyable(void)
	{}

private:
	MNonCopyable(const MNonCopyable &);
	const MNonCopyable &operator=(const MNonCopyable &);
};

// MCriticalSection /////////////////////////
class MCriticalSection 
{
private:
	CRITICAL_SECTION	m_cs;

public:
	MCriticalSection()	{ InitializeCriticalSection(&m_cs); }
	~MCriticalSection() { DeleteCriticalSection(&m_cs); }

	void Lock()			{ EnterCriticalSection(&m_cs); }
	void Unlock()		{ LeaveCriticalSection(&m_cs); }

	class Guard : private MNonCopyable
	{
	private:
		MCriticalSection&	m_csSyncObject;
		bool				m_bLocked;
	public:
		explicit Guard(MCriticalSection& cs, bool bInitiallyLocked = true) : m_csSyncObject(cs), m_bLocked(false)
		{
			if (bInitiallyLocked == true)
			{
				Lock();
			}
		}
		explicit Guard(MCriticalSection* cs, bool bInitiallyLocked = true) : m_csSyncObject(*cs), m_bLocked(false)
		{
			if (bInitiallyLocked == true)
			{
				Lock();
			}
		}
		~Guard()
		{
			if (m_bLocked == true)
			{
				Unlock();
			}
		}
		void Lock()
		{
			if (m_bLocked == true)
			{
				assert(!"MCriticalSection-Guard : Lock()");
				throw "MCriticalSection-Guard : Lock()";
			}

			m_csSyncObject.Lock();
			m_bLocked = true;
		}
		void Unlock()
		{
			if (m_bLocked == false)
			{
				assert(!"MCriticalSection-Guard : Unlock()");
				throw "MCriticalSection-Guard : Unlock()";
			}

			m_bLocked = false;
			m_csSyncObject.Unlock();
		}
	};
};

/// 클래스 단위 다중 쓰레드 동기화
/// MClassLevelLockable ////////////////////////////
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


/// MThreadSafeObject //////////////////////////////////
template<typename T>
class MThreadSafeObject : private MNonCopyable
{
private:
	T					m_Object;
	MCriticalSection	m_Lock;
public:
	MThreadSafeObject(void)	{}
	~MThreadSafeObject(void) {}
	T* Lock(void)
	{
		m_Lock.Lock();
		return &m_Object;
	}
	void Unlock(void)
	{
		m_Lock.Unlock();
	}
};

class MSignalEvent
{
protected:
	HANDLE m_hEvent;
public:
	MSignalEvent() : m_hEvent(NULL) {}
	~MSignalEvent()  {}
	bool Signal()
	{
		return (SetEvent(m_hEvent) == TRUE);
	}
	bool Reset()
	{
		return (ResetEvent(m_hEvent) == TRUE);
	}
	bool Pulse()
	{
		return (PulseEvent(m_hEvent) == TRUE);
	}

	bool Wait(unsigned int milliseconds = INFINITE)
	{
		return (WaitForSingleObject(m_hEvent, milliseconds) != WAIT_TIMEOUT);
	}

	HANDLE GetEvent() { return m_hEvent; }
};

class MAutoSignalEvent : public MSignalEvent
{
private:
	MAutoSignalEvent(const MAutoSignalEvent &);
	const MAutoSignalEvent &operator=(const MAutoSignalEvent &);
public:
	MAutoSignalEvent(bool initialState = false) : MSignalEvent()
	{
		m_hEvent = CreateEvent(NULL, FALSE, initialState, NULL);
	}
	~MAutoSignalEvent(void)
	{
		CloseHandle(m_hEvent);
	}
};

class MManualSignalEvent : public MSignalEvent
{
private:
	MManualSignalEvent(const MManualSignalEvent &);
	const MManualSignalEvent &operator=(const MManualSignalEvent &);
public:
	MManualSignalEvent(bool initialState = false) : MSignalEvent()
	{
		m_hEvent = CreateEvent(NULL, TRUE, initialState, NULL);
	}
	~MManualSignalEvent(void)
	{
		CloseHandle(m_hEvent);
	}
};


inline int __stdcall Increment(volatile int *value)
{
	return static_cast<int>(_InterlockedIncrement(reinterpret_cast<volatile long *>(value)));
}

inline unsigned int __stdcall Increment(volatile unsigned int *value)
{
	return static_cast<unsigned int>(_InterlockedIncrement(reinterpret_cast<volatile long *>(value)));
}

inline int __stdcall Decrement(volatile int *value)
{
	return static_cast<int>(_InterlockedDecrement(reinterpret_cast<volatile long *>(value)));
}

inline unsigned int __stdcall Decrement(volatile unsigned int *value)
{
	return static_cast<unsigned int>(_InterlockedDecrement(reinterpret_cast<volatile long *>(value)));
}

inline int __stdcall Exchange(volatile int *dest, int value)
{
	return static_cast<int>(_InterlockedExchange(
		reinterpret_cast<volatile long *>(dest), static_cast<long>(value)));
}

inline unsigned int __stdcall Exchange(volatile unsigned int *dest, unsigned int value)
{
	return static_cast<unsigned int>(_InterlockedExchange(reinterpret_cast<volatile long *>(dest), static_cast<long>(value)));
}

inline int __stdcall CompareExchange(volatile int *dest, int value, int comperand)
{
	return static_cast<int>(_InterlockedCompareExchange(
		reinterpret_cast<volatile long *>(dest),
		static_cast<long>(value), static_cast<long>(comperand)));
}

inline unsigned int __stdcall CompareExchange(volatile unsigned int *dest, unsigned int value, unsigned int comperand)
{
	return static_cast<unsigned int>(_InterlockedCompareExchange(
		reinterpret_cast<volatile long *>(dest),
		static_cast<long>(value), static_cast<long>(comperand)));
}

CML2_API void SetThisThreadName(const char* name);

} // namespace cml2 {


#endif // _MTHREAD2_H