#include "stdafx.h"
#include "MProactor.h"

namespace minet {

MProactor::~MProactor()
{
	Fini();
}

void MProactor::Fini()
{
	if (m_hIOCP && !m_vecThreads.empty())
	{
		m_bWorkThread = false;

		for (size_t i = 0; i < m_vecThreads.size(); ++i)
		{
			// WorkerThread에 종료 메시지를 보낸다.
			PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
		}

		DWORD nThreadCount = static_cast<DWORD>(m_vecThreads.size());
		WaitForMultipleObjects(nThreadCount, &m_vecThreads[0], TRUE, 30000);

		for (size_t i = 0; i < m_vecThreads.size(); ++i)
		{
			CloseHandle(m_vecThreads[i]);
		}

		CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
		m_vecThreads.clear();
	}
}

UINT WINAPI MProactor::ThreadProc( void* pProactor )
{
	static_cast<MProactor*>(pProactor)->Run();
	return 0;
}

bool MProactor::Init( int numofthreads )
{
	m_nNumOfThreads = numofthreads;

	if (!CreateWorkerIOCP()) return false;
	if (!CreateWorkerThreads(numofthreads)) return false;

	return true;
}

void MProactor::PostPrivateEvent( DWORD pId, MAct* pActor )
{
	BOOL ret = ::PostQueuedCompletionStatus(m_hIOCP, 0, pId, pActor);
}

void MProactor::Run()
{
#ifdef _DEBUG
	{
		static int thread_id = 0;
		char szThreadID[64];
		sprintf_s(szThreadID, "IOCP Thread(%d)", thread_id++);
		cml2::SetThisThreadName(szThreadID);
	}
#endif

	DWORD numberOfBytes = 0;
	DWORD completionKey = 0;

	while (m_bWorkThread)
	{
		LPOVERLAPPED pOverlapped = NULL;
		PULONG_PTR lpCompletionKey = static_cast<PULONG_PTR>(&completionKey);

		BOOL status = ::GetQueuedCompletionStatus( m_hIOCP, 
												   &numberOfBytes, 
												   lpCompletionKey, 
												   &pOverlapped, 
												   INFINITE );


		if (status == TRUE)
		{
			if (pOverlapped == NULL)
			{
				// 종료 처리
				if (numberOfBytes == 0 && completionKey == 0)
				{
					continue;
				}

				int error = WSAGetLastError();

				if (error)
				{
					minet_log( "minet> Proactor ProcEvents() Ov NULL Error!!! (error=%d)\n", error );
				}

				continue;
			}

			MAct* pAct = static_cast<MAct*>(pOverlapped);

			pAct->Complete( numberOfBytes );

			delete pAct;
			pAct = NULL;
		}
		else
		{
			int error = WSAGetLastError();

			if (pOverlapped != NULL)
			{
				MAct* pAct = static_cast<MAct*>(pOverlapped);
				pAct->Error( numberOfBytes, error );

				delete pAct;
				pAct = NULL;

				continue;
			}

			minet_logd( "minet> Proactor ProcEvents() status false Error!!! (error=%d)\n", error );
			continue;
		}
	}
}

MProactor::MProactor() : m_hIOCP(0), m_bWorkThread(true)
{

}

bool MProactor::RegisterToCompletionPort( HANDLE hSocketHandle )
{
	_ASSERT(m_hIOCP != 0);

	// 관리할 소켓 핸들 추가
	return NULL != ::CreateIoCompletionPort(hSocketHandle, m_hIOCP, 0, 0);
}

bool MProactor::CreateWorkerIOCP()
{
	// IOCP 핸들을 얻기 위해 INVALID_HANDLE_VALUE를 인자로 넣는다.
	m_hIOCP = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (0 == m_hIOCP)
	{
		minet_log("minet> CreateIoCompletionPort failed to create I/O completion port: (error=%d)\n", GetLastError());
		return false;
	}
	return true;
}

bool MProactor::CreateWorkerThreads( int numofthreads )
{
	for (int i = 0; i < numofthreads; i++)
	{
		unsigned int thread_id;
		HANDLE handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, MProactor::ThreadProc, (void*)this, 0, &thread_id));
		if (handle != NULL)
		{
			m_vecThreads.push_back(handle);
		}
		else
		{
			minet_log("minet> CreateThread failed to create worker thread: (error=%d)\n", GetLastError());
			return false;
		}
	}
	return true;
}

} // namespace minet {