
#include "stdafx.h"
#include "XAppErrorCollector.h"


// XAppErrorCollector
XAppErrorCollector::XAppErrorCollector()
{
	m_hWnd = NULL;
	m_nLastErrorType = ERRORTYPE_NONE;
	m_bExitApp = false;
}


// ~XAppErrorCollector
XAppErrorCollector::~XAppErrorCollector()
{
	if ( m_nLastErrorType != ERRORTYPE_NONE  &&  m_strLastErrorMsg.empty() == false)
		MessageBox( NULL, m_strLastErrorMsg.c_str(), L" 레이더즈 에러", MB_OK);
}


// SetWindowHandle
void XAppErrorCollector::SetWindowHandle( HWND hWnd)
{
	m_hWnd = hWnd;
}


// SetLastError
void XAppErrorCollector::SetLastError( int nType, const _CHAR* szMsg)
{
	m_nLastErrorType = nType;
	m_strLastErrorMsg = szMsg;
}


// ExitApplication
void XAppErrorCollector::ExitApplication( DWORD dwDelay)
{
	if ( m_bExitApp == false)
	{
		m_bExitApp = true;

		m_dwExitTimer = timeGetTime() + dwDelay;

		if ( dwDelay == 0)
			_exit_application();
	}
}


// GetLastErrorType
int XAppErrorCollector::GetLastErrorType() const
{
	return m_nLastErrorType;
}


// GetLastErrorMsg
const _CHAR* XAppErrorCollector::GetLastErrorMsg()
{
	return m_strLastErrorMsg.c_str();
}


// GetInstance
XAppErrorCollector* XAppErrorCollector::GetInstance()
{
	static XAppErrorCollector instance;
	return &instance;
}


// Update
void XAppErrorCollector::Update()
{
	if ( m_bExitApp == false)
		return;

	if ( timeGetTime() > m_dwExitTimer)
		_exit_application();
}


// _exit_application
void XAppErrorCollector::_exit_application()
{
	if ( m_hWnd != NULL)
	{
		PostMessage( m_hWnd, WM_CLOSE, 0, 0);
		m_hWnd = NULL;
	}
}
