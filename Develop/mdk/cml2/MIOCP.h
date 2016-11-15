#pragma once

#include <vector>
#include "RLib.h"

class CML2_API MIOCP
{
public:
	class IHandler
	{
		// 지워지지 않는 객체로 쓰길 권장(자신에게 io가 오지 않는 것을 보장하기 힘들다면)
	public:
		virtual void			OnIO(const DWORD& numOfByteTransfered,OVERLAPPED* const overlapped) = 0;
		virtual void			OnError(const int& errCode) = 0;

	public:
		IHandler() : m_h(INVALID_HANDLE_VALUE) {}

		void			SetHandle(const HANDLE& h) { m_h = h; }
		const HANDLE&	Handle() const { return m_h; }

	private:
		HANDLE m_h;
	};

public:
	MIOCP();
	virtual ~MIOCP();

	bool Init(const int& workerCnt,const DWORD& GQCSWaitTime = INFINITE,const DWORD& maxProcessGQCSTime = 300);
	void Term();

	// 실패 시 GetLastError()로 확인
	bool Register(const IHandler& handler) const;
	bool Post(const IHandler& handler,OVERLAPPED* const overlapped) const;

	bool Process();

private:
	virtual void AfterGQCS() {};

	static UINT WINAPI Threading(void* const data);

private:
	typedef std::vector<HANDLE> THREADS;

	const static DWORD EXITCODE = -1;

private:
	HANDLE	m_handle;
	THREADS	m_threads;
	bool	m_working;
	DWORD	m_GQCSWaitTime;
	DWORD	m_maxProcessGQCSTime;
};