#pragma once

#include "MiNetPrerequisites.h"

#include <assert.h>
#include "MActor.h"
#include "MThread2.h"

namespace minet {

class MProactor
{
public:
	HANDLE					m_hIOCP;
	DWORD					m_nNumOfThreads;
	std::vector<HANDLE>		m_vecThreads;
	bool					m_bWorkThread;
private:
	bool CreateWorkerIOCP();
public:
	static UINT WINAPI ThreadProc(void* pProactor);
public:
	MProactor();
	~MProactor();
	bool Init(int numofthreads);
	void Fini();

	bool CreateWorkerThreads( int numofthreads );
	bool RegisterToCompletionPort(HANDLE hSocketHandle);
	void PostPrivateEvent(DWORD pId, MAct* pActor);

	// worker Thread에서 호출 --
	void Run();		
	// worker Thread에서 호출 --
};


} // namespace minet {