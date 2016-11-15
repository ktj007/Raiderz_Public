#include "stdafx.h"
#include "MIOCP.h"
#include "MUtil.h"
#include "MThread2.h"

#include <process.h>

namespace {
	HANDLE g_startEvt = NULL;
}

MIOCP::MIOCP()
: m_handle(NULL)
, m_working(false)
, m_GQCSWaitTime(INFINITE)
, m_maxProcessGQCSTime(0)
{
}

MIOCP::~MIOCP()
{
}

bool MIOCP::Init(const int& workerCnt, const DWORD& GQCSWaitTime, const DWORD& maxProcessGQCSTime)
{
	if (NULL != m_handle)
	{
		Term();
	}

	// IOCP 핸들을 얻기 위해 INVALID_HANDLE_VALUE를 인자로 넣는다.
	m_handle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_handle)
	{
		return false;
	}
	m_GQCSWaitTime = GQCSWaitTime;
	m_maxProcessGQCSTime = maxProcessGQCSTime;

	g_startEvt = CreateEvent(0, FALSE, FALSE, 0);
	if (NULL == g_startEvt)
	{
		return false;
	}

	m_working = true;
	for (int i = 0; i < workerCnt; ++i)
	{
		unsigned int id = 0;
		HANDLE handle = rcast<HANDLE>(_beginthreadex(NULL, 0, MIOCP::Threading, (void*)this, 0, &id));
		if (NULL == handle)
		{
			Term();
			return false;
		}
		m_threads.push_back(handle);

		if (WAIT_OBJECT_0 != WaitForSingleObject(g_startEvt, INFINITE))
		{
			return false;
		}

	}

	return true;
}

void MIOCP::Term()
{
	if (NULL == m_handle)
	{
		return;
	}

	if (true == m_threads.empty())
	{
		return;
	}

	m_working = false;

	for (size_t i = 0; i < m_threads.size(); ++i)
	{
		// WorkerThread에 종료 메시지를 보낸다.
		PostQueuedCompletionStatus(m_handle, 0, EXITCODE, NULL);
	}

	DWORD nThreadCount = scast<DWORD>(m_threads.size());
	WaitForMultipleObjects(nThreadCount, &m_threads[0], TRUE, 30000);

	for each (const HANDLE& threadHandle in m_threads)
	{
		CloseHandle(threadHandle);
	}
	m_threads.clear();

	CloseHandle(m_handle);
	m_handle = NULL;

	if (NULL != g_startEvt && INVALID_HANDLE_VALUE != g_startEvt)
	{
		CloseHandle(g_startEvt);
		g_startEvt = NULL;
	}
	m_GQCSWaitTime = INFINITE;
}

bool MIOCP::Register(const IHandler& handler)  const
{
	if (NULL == m_handle)
	{
		return false;
	}

	if (NULL == handler.Handle())
	{
		return false;
	}

	if (NULL == m_handle)
	{
		return false;
	}

	if (INVALID_HANDLE_VALUE != handler.Handle()) {
		if (m_handle != CreateIoCompletionPort(handler.Handle(), m_handle, rcast<ULONG_PTR>(&handler), 0))
		{
			return false;
		}
	}

	return true;
}

bool MIOCP::Post(const IHandler& handler,OVERLAPPED* const overlapped) const
{
	if (NULL == m_handle) {
		return false;
	}

	return TRUE == ::PostQueuedCompletionStatus(m_handle, 0, rcast<ULONG_PTR>(&handler), overlapped);
}

bool MIOCP::Process()
{
	DWORD startTime = ::timeGetTime();
	while (true)
	{
		DWORD numOfByteTransfered = 0;
		ULONG_PTR key = NULL;
		LPOVERLAPPED overlapped = NULL;

		assert(NULL!=m_handle);
		BOOL result = ::GetQueuedCompletionStatus( m_handle, &numOfByteTransfered, &key, &overlapped, m_GQCSWaitTime );

		if (EXITCODE == key)
		{
			return false;
		}

		IHandler* const handler(rcast<IHandler*>(key));
		if (TRUE == result)
		{
			handler->OnIO(numOfByteTransfered, overlapped);
		}
		else
		{
			if (NULL == overlapped)
			{
				// m_GQCSWaitTime이 INFINITE이 아닐때 timeout일 경우 overlapped가 null이 된다
				break;
			}

			handler->OnError(GetLastError());
		}

		if (startTime + m_maxProcessGQCSTime < ::timeGetTime())
			break;
	}

	AfterGQCS();

	return true;
}

UINT WINAPI MIOCP::Threading(void* const data)
{
	assert(NULL!=data);
	MIOCP* const iocp = rcast<MIOCP*>(data);

#ifdef _DEBUG
	{
		static LONG threadID = 0;
		char szThreadID[64];
		sprintf_s(szThreadID, "IOCP Thread(%d)", InterlockedIncrement(&threadID));
		cml2::SetThisThreadName(szThreadID);
	}
#endif

	SetEvent(g_startEvt);
	while (true == iocp->m_working)
	{
		if (false == iocp->Process())
		{
			return 0;
		}
	}

	return 0;
}
