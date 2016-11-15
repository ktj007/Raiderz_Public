#include "StdAfx.h"
#include "SServerMonitorEventListener.h"


SServerMonitorEventListener::SServerMonitorEventListener()
: m_hServerMonitorSemaphore(INVALID_HANDLE_VALUE)
, m_bIsAssociatedServerMonitor(false)
{
	
}

SServerMonitorEventListener::~SServerMonitorEventListener()
{
	if (INVALID_HANDLE_VALUE != m_hServerMonitorSemaphore)
		CloseHandle(m_hServerMonitorSemaphore);
}

bool SServerMonitorEventListener::Create(std::wstring strServerName)
{
	/// 시그널을 감지할 Mutex 오브젝트 생성
	m_hServerMonitorSemaphore = CreateSemaphore(NULL, 0, 1, strServerName.c_str());
	if (INVALID_HANDLE_VALUE == m_hServerMonitorSemaphore)
		return false;

	if (ERROR_ALREADY_EXISTS == ::GetLastError())
		m_bIsAssociatedServerMonitor = true;

	return true;
}

bool SServerMonitorEventListener::CheckSignal_Terminate()
{
	if (false == m_bIsAssociatedServerMonitor) return false;

	/// ServerMonitor 에서 시그널을 보냈는지 체크.
	DWORD dwRet = WaitForSingleObject(m_hServerMonitorSemaphore, 0);
	if (WAIT_OBJECT_0 == dwRet)
	{
		ReleaseSemaphore(m_hServerMonitorSemaphore, 1, NULL);
		return true;
	}

	return false;
}