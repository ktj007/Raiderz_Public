#include "stdafx.h"
#include "MDBThread.h"
#include "MDBThreadTaskQ.h"
#include "MDBThreadTaskResultQ.h"


namespace mdb
{
	MDBThread::MDBThread()
	{
		m_bIsExit			= true;
		m_bIsExitCompleted	= false;
		m_nDBReconnectCount	= 0;
	}


	MDBThread::~MDBThread()
	{

	}


	HANDLE MDBThread::GetHandle()		
	{ 
		return m_hThread; 
	}


	DWORD MDBThread::GetThreadID()	
	{ 
		return m_dwThreadID; 
	}


	bool MDBThread::Create(const wstring& strDSN
						  , MDBThreadTaskQ* pTaskQ
						  , MDBThreadTaskResultQ* pTaskResultQ)
	{
		if (!m_DB.ConnectW(strDSN))
		{
			return false;
		}

		m_pTaskQ		= pTaskQ;
		m_pTaskResultQ	= pTaskResultQ;
		m_bIsExit		= false;			/// Create가 성공하면 false로 한다.
											/// Create성공하지 않으면 쓰레드가 활성화 되지 않는다.

		m_hThread = CreateThread(NULL, 0, _WorkThread, this, 0, &m_dwThreadID);
		if (NULL == m_hThread)
		{
			_ASSERT(NULL != m_hThread);
			m_bIsExit = true;

			return false;
		}

		return true;
	}

	void MDBThread::Release()
	{
		m_DB.Disconnect();
		m_nDBReconnectCount = 0;
	}

	void MDBThread::DecDBReconnectCount()
	{
		if (0 < m_nDBReconnectCount)
			--m_nDBReconnectCount;
	}

	void MDBThread::IncDBReconnectCount()
	{
		if (MAX_RECONNECT_COUNT > m_nDBReconnectCount)
			++m_nDBReconnectCount;
	}

	int MDBThread::GetDBReconnectCount()
	{
		return m_nDBReconnectCount;
	}

	bool MDBThread::IsDBConnected()
	{
		if (m_DB.IsConnected())
		{
			DecDBReconnectCount();	
			return true;
		}

		IncDBReconnectCount();

		if (m_DB.Reconnect())
			return true;
		
		Sleep(1000 * GetDBReconnectCount());

		if (MAX_RECONNECT_COUNT <= GetDBReconnectCount())
			return true;	// 최대 시도 횟수를 초과하면 테스크 1개를 실행해서 에러를 APP로 넘겨준다.
							// 별도로 에러를 넘겨줄 수 있는 기능이 필요하다.

		return false;		
	}

	void MDBThread::Run()
	{
		_ASSERT(NULL != m_pTaskQ);
		_ASSERT(NULL != m_pTaskResultQ);

		if (!IsDBConnected())
			return;

		MDBThreadTask* pTask = m_pTaskQ->FetchTask();
		if (NULL == pTask)
		{
			return;
		}

		pTask->OnExecute(m_DB);

		m_pTaskResultQ->Push(pTask);
	}


	bool MDBThread::IsExit()
	{
		return m_bIsExit;
	}


	void MDBThread::SetExit()
	{
		m_bIsExit = true;
	}


	bool MDBThread::IsExitCompleted()
	{
		return m_bIsExitCompleted;
	}


	void MDBThread::SetExitCompleted()
	{
		m_bIsExitCompleted = true;
	}


	DWORD WINAPI MDBThread::_WorkThread(LPVOID pContext)
	{
		MDBThread* pThread = reinterpret_cast<MDBThread*>(pContext);

		_ASSERT(false == pThread->IsExit());
		
		while (false == pThread->IsExit())
		{
			pThread->Run();
		}

		pThread->SetExitCompleted();

		return 0;
	}
}