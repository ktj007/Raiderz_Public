#include "stdafx.h"
#include "MDBThreadPool.h"
#include "MDBThread.h"


namespace mdb
{
	MDBThreadPool::MDBThreadPool()
	{

	}
	

	MDBThreadPool::~MDBThreadPool()
	{

	}


	bool MDBThreadPool::Create(const DWORD dwThreadCount
		, const wstring& strDSN
		, MDBThreadTaskQ* pTaskQ
		, MDBThreadTaskResultQ* pTaskResultQ)
	{
		for (DWORD i = 0; i < dwThreadCount; ++i)
		{
			MDBThread* pThread = new MDBThread;
			
			if (!pThread->Create(strDSN, pTaskQ, pTaskResultQ))
			{
				return false;
			}

			m_ThreadMap.insert(MDB_THREAD_MAP::value_type(pThread->GetHandle(), pThread));
		}

		return true;
	}


	void MDBThreadPool::Release()
	{
		for (MDB_THREAD_MAP::iterator it = m_ThreadMap.begin()
			; it != m_ThreadMap.end()
			; ++it)
		{
			MDBThread* pThread = it->second;

			CloseHandle(pThread->GetHandle());

			pThread->Release();
			delete pThread;
		}

		m_ThreadMap.clear();
	}


	void MDBThreadPool::SetExit()
	{
		for (MDB_THREAD_MAP::iterator it = m_ThreadMap.begin()
			; it != m_ThreadMap.end()
			; ++it)
		{
			MDBThread* pThread = it->second;
			pThread->SetExit();		
		}
	}


	bool MDBThreadPool::IsExitComplete()
	{
		for (MDB_THREAD_MAP::iterator it = m_ThreadMap.begin()
			; it != m_ThreadMap.end()
			; ++it)
		{
			MDBThread* pThread = it->second;

			if (!pThread->IsExitCompleted())
			{
				return false;
			}
		}

		return true;
	}

	size_t MDBThreadPool::GetThreadCount()
	{
		return m_ThreadMap.size();
	}
}