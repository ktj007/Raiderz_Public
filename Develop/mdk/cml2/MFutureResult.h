#pragma once

#include "MThread2.h"

namespace cml2 {

/// Future Pattern에서 사용하는 결과값

template <typename ResultT>
class MFutureResult
{
private:
	MCriticalSection		m_csLock;
	MAutoSignalEvent		m_ResultReadySignal;
	bool m_bResultReady;
	ResultT m_Result;
public:
	MFutureResult() :  m_bResultReady(false) {}

	// find whether result can be retrieved yet
	bool IsResultReady() const
	{
		MCriticalSection::Guard lock(m_csLock);
		return m_bResultReady;
	}

	// retrieve result, blocking if not yet available
	ResultT& GetResult()
	{
		MCriticalSection::Guard lock(m_csLock);

		if(m_bResultReady == false)
		{
		  // 결과값이 세팅될 때까지 기다린다.
		  m_ResultReadySignal.Wait();
		}
		return m_Result;
	}

	void SetResult(const ResultT& result)
	{
		MCriticalSection::Guard lock(m_csLock);

		m_Result = result;
		m_bResultReady = true;

		// 결과값을 기다리고 있을 쓰레드에게 알려준다.
		m_ResultReadySignal.Signal();
	}

};
 

} // namespace cml2