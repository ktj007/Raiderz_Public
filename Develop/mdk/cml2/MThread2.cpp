#include "stdafx.h"
#include "MThread2.h"

namespace cml2 {


MThread::MThread(void) : m_Handle(NULL), m_bFreeOnTerminate(false), m_bTerminated(false), m_bFinished(false),
						m_nReturnValue(0), m_nThreadID(0)
{
}

MThread::~MThread(void)
{
	Stop();
}

bool MThread::Start(void)
{
	typedef unsigned int (WINAPI *ThreadProc)(void *);

	m_Handle = reinterpret_cast<HANDLE>(
					_beginthreadex( NULL, 0, &MThread::ThreadProc, this, 0, &m_nThreadID) );
	if (m_Handle == NULL)
	{
		return false;
	}

	m_bTerminated = false;
	m_bFinished = false;
	m_nReturnValue = 0;

	return true;
}

void MThread::Stop(void)
{
	if (m_nThreadID != 0 && m_bFinished == false)
	{
		Terminate();
		Wait();
	}

	if (m_Handle != NULL)
	{
		CloseHandle(m_Handle);
		m_Handle = NULL;
	}
}

void MThread::Terminate(void)
{
	m_bTerminated = true;
}

bool MThread::Wait(unsigned int timeout)
{
	if (m_Handle == NULL) return false;

	DWORD result = WaitForSingleObject(m_Handle, timeout);
	if (result == WAIT_ABANDONED || result == WAIT_TIMEOUT)
	{
		return false;
	}

	return true;
}

void MThread::SetFreeOnTerminate(bool value)
{
	m_bFreeOnTerminate = value;
}

bool MThread::GetFreeOnTerminate(void) const
{
	return m_bFreeOnTerminate;
}

void MThread::SetFinished(bool value)
{
	m_bFinished = value;
}

bool MThread::GetFinished(void) const
{
	return m_bFinished;
}

HANDLE MThread::GetHandle(void) const
{
	return m_Handle;
}

unsigned int MThread::GetThreadID(void) const
{
	return m_nThreadID;
}

bool MThread::Terminated(void) const
{
	return m_bTerminated;
}

void MThread::SetThreadName(const char* name)
{
	SetThisThreadName(name);
}

void MThread::SetReturnValue(int value)
{
	m_nReturnValue = value;
}

int MThread::GetReturnValue(void) const
{
	return m_nReturnValue;
}

// static
unsigned int WINAPI MThread::ThreadProc(void* data)
{
	class ScopedFree
	{
	private:
		MThread* m_pInstance;
		int& m_nReturnValue;

	public:
		ScopedFree(MThread *instance, int &returnValue) : m_pInstance(instance), m_nReturnValue(returnValue)
		{
		}

		~ScopedFree(void)
		{
			bool freeThread = m_pInstance->GetFreeOnTerminate();
			m_nReturnValue = m_pInstance->GetReturnValue();

			m_pInstance->SetFinished(true);
			if (freeThread == true)
			{
				delete m_pInstance;
			}

			_endthreadex(m_nReturnValue);
		}
	};

	if (data == NULL)
	{
		return -1;
	}

	MThread* pThread = static_cast<MThread*>(data);

	int value = 0;

	{
		ScopedFree scopedFree(pThread, value);
		pThread->Run();
	}

	return value;
}

void SetThisThreadName(const char* name)
{
	unsigned int threadID = -1;

	struct ThreadNameInfo
	{
		unsigned long	nType;        // must be 0x1000
		const char*		szName;         // pointer to name (in user addr space)
		unsigned long	nThreadID;    // thread ID (-1=caller thread)
		unsigned long	nFlags;       // reserved for future use, must be zero
	};

	ThreadNameInfo info;

	info.nType = 0x1000;
	info.szName = name;
	info.nThreadID = threadID;
	info.nFlags = 0;

	__try
	{
		RaiseException(0x406D1388, 0,
			sizeof(info) / sizeof(unsigned long),
			reinterpret_cast<ULONG_PTR *>(&info));
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{}
}


} // namespace cml2